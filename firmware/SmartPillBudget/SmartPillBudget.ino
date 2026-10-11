/* Budget revision, protocol 4. Inert-object prototype; physical validation required. */
#include <Arduino.h>
#include <Wire.h>
#include <WDT.h>
#include <Servo.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include "config.h"
#include "journal.h"
#include "beam_counter.h"

enum State { IDLE, REFILL, DISPENSING, CLOSING, BENCH, FAULT };
State state=IDLE;
Journal journal;
Servo motors[4];
RTC_DS3231 rtc;
hd44780_I2Cexp lcd(LCD_ADDRESS);
volatile BeamCounter beams[4]={};
bool hardwareOK=false,rtcOK=false,armed=false,scheduled=false;
uint8_t counts[4]={},idx=0,countNow=0,done=0,total=0;
uint32_t doseAt=0,pillAt=0,stateAt=0,moveAt=0,hostAt=0,lidClosedAt=0,benchUntil=0,bootAt=0;
int pulse=2000,direction=-1;
char statusDetail[48]="Configure and commission",job[24]="";
char input[600];size_t used=0;bool overflow=false;
char tx[800];size_t txLen=0,txSent=0;
char rows[2][17]={{0}},clockRow[17]="Set RTC via USB";
bool configPending=false,refillOpened=false;
bool startupPending=true;
uint32_t busyDays[3]={},busyStamp[3]={};
uint8_t refillSelected=0; // 0=all off; otherwise container 1..4
uint8_t beeps=0;bool buzzing=false;uint32_t beepAt=0;
extern int getIrqIndexFromPin(uint32_t pinNumber);
void rememberBusyPeriods();
void copyText(char* d,const char* s,size_t n){if(n){strncpy(d,s,n-1);d[n-1]=0;}}
const char* stateName(){switch(state){case IDLE:return "IDLE";case REFILL:return "REFILL";case DISPENSING:return "DISPENSING";case CLOSING:return "CLOSING";case BENCH:return "BENCH";default:return "FAULT";}}
const char* outcomeName(uint8_t v){switch(v){case STARTED:return "STARTED";case DISPENSED:return "DISPENSED";case MISSED:return "MISSED";case INTERRUPTED:return "INTERRUPTED";default:return "NONE";}}
bool moving(){return state==DISPENSING||state==CLOSING||state==BENCH;}
bool blocked(int i){return digitalRead(BEAM_PINS[i])==LOW;}
bool lidSafe(){return !digitalRead(LID_PIN)&&lidClosedAt&&millis()-lidClosedAt>=1000;}
void off(){for(int i=0;i<4;i++){motors[i].detach();pinMode(SERVO_PINS[i],OUTPUT);digitalWrite(SERVO_PINS[i],LOW);}}
void selectRefillLED(uint8_t container){
  for(int i=0;i<4;i++)digitalWrite(REFILL_LED_PINS[i],LOW);
  refillSelected=container;
  if(container>=1&&container<=4)digitalWrite(REFILL_LED_PINS[container-1],HIGH);
}
void serviceRefillLEDs(){
  if(refillSelected&&(!Serial||millis()-hostAt>HOST_TIMEOUT_MS)){
    selectRefillLED(0);
    // The physical cover still inhibits motion after the guidance link is lost.
    if(state==REFILL&&lidSafe()){rememberBusyPeriods();state=IDLE;}
    copyText(statusDetail,"Refill light off: USB lost",sizeof(statusDetail));
  }
}
void sound(uint8_t n){beeps=n;beepAt=0;}
void buzzer(){
  if(!beeps&&!buzzing)return;if(millis()-beepAt<140)return;beepAt=millis();
  if(buzzing){noTone(BUZZER_PIN);buzzing=false;if(beeps)beeps--;}
  else{tone(BUZZER_PIN,state==FAULT?700:1800);buzzing=true;}
}
void edge(int i){beams[i].edge(blocked(i),micros(),MIN_BEAM_PULSE_US);}
void edge0(){edge(0);}void edge1(){edge(1);}void edge2(){edge(2);}void edge3(){edge(3);}
void (*const callbacks[4])()={edge0,edge1,edge2,edge3};
uint8_t take(int i){noInterrupts();uint8_t n=beams[i].pending;beams[i].pending=0;interrupts();return n;}
void queue(JsonDocument& j){
  if(!Serial||txLen)return;size_t n=measureJson(j);if(n+1>=sizeof(tx))return;
  txLen=serializeJson(j,tx,sizeof(tx));tx[txLen++]='\n';txSent=0;
}
void flush(){
  if(!Serial){txLen=txSent=0;return;}if(!txLen)return;
  size_t n=min((size_t)Serial.availableForWrite(),txLen-txSent);
  if(n)txSent+=Serial.write((uint8_t*)tx+txSent,n);if(txSent==txLen)txLen=txSent=0;
}
void emit(const char* event,const char* why=""){
  JsonDocument o;o["protocol"]=4;o["firmware"]="SmartPillBudget-R4-4";o["event"]=event;
  o["state"]=stateName();o["armed"]=armed;o["id"]=job;o["done"]=done;o["total"]=total;
  o["detail"]=why[0]?why:statusDetail;o["lid_closed"]=lidSafe();o["rtc_ok"]=rtcOK;
  o["refill_selected"]=refillSelected;
  o["sequence"]=journal.value.sequence;o["outcome"]=outcomeName(journal.value.outcome);
  o["timestamp"]=journal.value.timestamp;
  JsonArray a=o["lcd"].to<JsonArray>();a.add(rows[0]);a.add(rows[1]);queue(o);
}
void fail(const char* why){
  off();selectRefillLED(0);armed=false;state=FAULT;copyText(statusDetail,why,sizeof(statusDetail));sound(5);
  Snapshot s=journal.value;
  if(!s.fault||s.active){s.fault=1;s.outcome=s.active?INTERRUPTED:s.outcome;s.done=done;
    // Preserve active marker until an explicit inspection and clear.
    if(journal.good&&(!journal.damaged||journal.present))journal.save(s);
  }
  emit("fault",why);
}
bool save(Snapshot s){if(!journal.save(s)){fail("Storage failure; inspect tray");return false;}return true;}
bool timeNow(DateTime& t){
  if(!rtcOK)return false;t=rtc.now();
  if(rtc.lostPower()||!t.isValid()||t.year()<2025||t.year()>2099){rtcOK=false;return false;}return true;
}
uint32_t dayOf(const DateTime& t){return (uint32_t)t.year()*10000+t.month()*100+t.day();}
void rememberBusyPeriods(){
  DateTime t;
  if(startupPending||!journal.present||!journal.good||journal.damaged||!journal.value.enabled||!timeNow(t))return;
  for(int s=0;s<3;s++){
    int sum=0;for(int k=0;k<4;k++)sum+=journal.value.doses[s][k];
    if(sum&&unclaimed(journal.value,s,dayOf(t),t.hour()*60+t.minute())&&dayOf(t)>busyDays[s]){
      busyDays[s]=dayOf(t);busyStamp[s]=t.unixtime();
    }
  }
}
void display(){
  static uint32_t at=0;if(millis()-at<250)return;at=millis();
  const char* top=clockRow;char bottom[48]="";
  if(state==DISPENSING||state==CLOSING){
    top=journal.value.names[idx];snprintf(bottom,sizeof(bottom),"Pills %u/%u",done,total);
  }else if(state==FAULT){top="FAULT / inspect";copyText(bottom,statusDetail,sizeof(bottom));}
  else if(state==REFILL){
    if(refillSelected){snprintf(bottom,sizeof(bottom),"Fill container %u",refillSelected);top=journal.value.names[refillSelected-1];}
    else{top="Manual refill";copyText(bottom,"Close cover",sizeof(bottom));}
  }
  else if(state==BENCH){top="EMPTY BENCH TEST";copyText(bottom,"Keep hands clear",sizeof(bottom));}
  else if(!armed)copyText(bottom,"Disabled / USB",sizeof(bottom));
  else if(!journal.value.enabled)copyText(bottom,"Schedule off",sizeof(bottom));
  else if((millis()/3000)%2&&journal.value.outcome==DISPENSED)copyText(bottom,"Dispensed",sizeof(bottom));
  else{
    DateTime t;if(timeNow(t)){
      int minute=t.hour()*60+t.minute(),best=1441,next=-1;
      for(int s=0;s<3;s++){int sum=0;for(int k=0;k<4;k++)sum+=journal.value.doses[s][k];if(!sum)continue;
        int distance=(journal.value.times[s]+1440-minute)%1440;if(!distance&&dayOf(t)<=journal.value.days[s])distance=1440;
        if(distance<best){best=distance;next=s;}}
      if(next>=0)snprintf(bottom,sizeof(bottom),"Next %02u:%02u",journal.value.times[next]/60,journal.value.times[next]%60);
      else copyText(bottom,"No doses set",sizeof(bottom));
    }
  }
  snprintf(rows[0],17,"%-16.16s",top);snprintf(rows[1],17,"%-16.16s",bottom);
}
void serviceLCD(){
  static char shown[32]={};static uint8_t scan=0;static uint32_t at=0;
  if(millis()-at<3)return;at=millis();
  for(int k=0;k<32;k++){uint8_t i=scan;scan=(scan+1)%32;if(shown[i]!=rows[i/16][i%16]){
    lcd.setCursor(i%16,i/16);lcd.write((uint8_t)rows[i/16][i%16]);shown[i]=rows[i/16][i%16];break;}}
}
bool attachAt(int i,uint16_t us){
  if(digitalRead(STOP_PIN)||!lidSafe()){fail("Check stop and cover");return false;}
  // Renesas attach loads a default pulse; replace it before the timer ISR can run.
  noInterrupts();uint8_t ok=motors[i].attach(SERVO_PINS[i],800,2200);
  motors[i].writeMicroseconds(us);interrupts();
  if(!ok||!motors[i].attached()){fail("Servo timer unavailable");return false;}return true;
}
void nextContainer(){
  while(idx<4&&!counts[idx])idx++;
  if(idx==4){
    off();rememberBusyPeriods();Snapshot s=journal.value;s.active=0;s.outcome=DISPENSED;s.done=done;
    if(!save(s))return;state=IDLE;scheduled=false;copyText(statusDetail,"Dispensed; empty tray",sizeof(statusDetail));sound(2);emit("dispensed");job[0]=0;return;
  }
  if(blocked(idx)){fail("Outlet blocked at start");return;}
  take(idx);countNow=0;pillAt=millis();pulse=CLOSED_US[idx];direction=OPEN_US[idx]>pulse?1:-1;
  attachAt(idx,pulse);
}
bool startDose(const uint8_t* c,bool automatic,const char* id){
  if(startupPending||!armed||state!=IDLE||!lidSafe()||digitalRead(STOP_PIN))return false;
  total=done=0;for(int i=0;i<4;i++){counts[i]=c[i];total+=c[i];if(c[i]&&blocked(i)){fail("Outlet blocked at start");return false;}}
  if(!total)return false;
  // Discard idle/refill test pulses; only observe a new job after this boundary.
  noInterrupts();
  for(int i=0;i<4;i++){beams[i].pending=0;beams[i].blocked=blocked(i);beams[i].started=micros();}
  interrupts();
  Snapshot s=journal.value;s.active=1;s.outcome=STARTED;s.done=0;s.total=total;
  DateTime t;if(!timeNow(t)){fail("RTC invalid");return false;}s.timestamp=t.unixtime();
  if(!save(s))return false; // Verify active record before any servo is attached.
  copyText(job,id,sizeof(job));scheduled=automatic;doseAt=pillAt=millis();idx=0;state=DISPENSING;
  sound(3);emit("dispensing");nextContainer();return true;
}
void updateDose(){
  if(state==BENCH){if((int32_t)(millis()-benchUntil)>=0){off();rememberBusyPeriods();state=IDLE;emit("bench_done");}return;}
  if(state!=DISPENSING&&state!=CLOSING)return;
  // No storage writes while moving; detach before persisting faults/results.
  for(int i=0;i<4;i++)if(i!=idx&&(take(i)||blocked(i))){fail("Unexpected outlet activity");return;}
  if(state==CLOSING){
    if(blocked(idx)||take(idx)){fail("Extra pill while closing");return;}
    if(millis()-stateAt>=500){motors[idx].detach();digitalWrite(SERVO_PINS[idx],LOW);idx++;state=DISPENSING;nextContainer();}return;
  }
  if(millis()-doseAt>DOSE_TIMEOUT_MS||millis()-pillAt>PILL_TIMEOUT_MS){fail("Pill timeout or jam");return;}
  uint8_t n=take(idx);
  if(n){
    if((int)countNow+n>counts[idx]){fail("Excess outlet events");return;}
    countNow+=n;done+=n;pillAt=millis();emit("progress");
    if(countNow>=counts[idx]){motors[idx].writeMicroseconds(CLOSED_US[idx]);state=CLOSING;stateAt=millis();return;}
  }
  if(millis()-moveAt>=20){
    moveAt=millis();int low=min(OPEN_US[idx],CLOSED_US[idx]),high=max(OPEN_US[idx],CLOSED_US[idx]);
    pulse+=10*direction;if(pulse>=high){pulse=high;direction=-1;}if(pulse<=low){pulse=low;direction=1;}
    motors[idx].writeMicroseconds(pulse);
  }
}
void sendConfig(){
  if(txLen||moving()){configPending=true;return;}configPending=false;
  JsonDocument o;o["event"]="config";o["protocol"]=4;o["enabled"]=journal.value.enabled!=0;
  JsonArray t=o["times"].to<JsonArray>(),n=o["names"].to<JsonArray>(),d=o["doses"].to<JsonArray>();
  for(int i=0;i<4;i++)n.add(journal.value.names[i]);
  for(int s=0;s<3;s++){t.add(journal.value.times[s]);JsonArray row=d.add<JsonArray>();for(int i=0;i<4;i++)row.add(journal.value.doses[s][i]);}
  queue(o);
}
void command(JsonDocument& p){
  const char* cmd=p["cmd"]|"";
  if(!strcmp(cmd,"ping")){hostAt=millis();emit("status");return;}
  if(!strcmp(cmd,"stop")){fail("Stopped; inspect tray");return;}
  if(!strcmp(cmd,"get_config")){sendConfig();return;}
  if(!strcmp(cmd,"set_time")){
    uint32_t stamp=p["timestamp"]|0UL;
    if((state!=IDLE&&state!=FAULT)||stamp<1735689600UL||stamp>4102444799UL||!rtc.begin()){emit("rejected","Clock unavailable/busy");return;}
    rtc.adjust(DateTime(stamp));rtcOK=!rtc.lostPower();emit("time_saved");return;
  }
  if(!strcmp(cmd,"clear")){
    DateTime t;
    if(state!=FAULT||!timeNow(t)||digitalRead(STOP_PIN)||!lidSafe()||!journal.present||!journal.good||!hardwareOK){
      emit("rejected","Inspect tray, RTC, stop and lid");return;}
    Snapshot s=journal.value;s.active=0;s.fault=0;
    // Never replay a due/past period following inspection or damaged-record repair.
    consumePast(s,dayOf(t),t.hour()*60+t.minute());
    bool ok=journal.damaged?journal.repair(s):journal.save(s);
    if(!ok){fail("Storage repair failed");return;}
    state=IDLE;armed=false;copyText(statusDetail,"Cleared; enable controls",sizeof(statusDetail));emit("cleared");return;
  }
  if(!strcmp(cmd,"set_config")){
    if(state!=IDLE||journal.damaged||!journal.good){emit("rejected","Idle healthy storage required");return;}
    JsonArray t=p["times"].as<JsonArray>(),n=p["names"].as<JsonArray>(),d=p["doses"].as<JsonArray>();
    bool ok=t.size()==3&&n.size()==4&&d.size()==3&&p["enabled"].is<bool>();
    Snapshot s=journal.value;
    for(int k=0;k<3&&ok;k++){
      if(!t[k].is<int>()||t[k].as<int>()<0||t[k].as<int>()>1439||d[k].as<JsonArray>().size()!=4){ok=false;break;}
      s.times[k]=t[k];for(int j=0;j<k;j++)if(s.times[j]==s.times[k])ok=false;
      for(int i=0;i<4;i++){if(!d[k][i].is<int>()||d[k][i].as<int>()<0||d[k][i].as<int>()>10)ok=false;else s.doses[k][i]=d[k][i];}
    }
    for(int i=0;i<4&&ok;i++){
      const char* name=n[i].as<const char*>();if(!name||strlen(name)>16){ok=false;break;}
      for(const char* c=name;*c;c++)if(*c<32||*c>126)ok=false;copyText(s.names[i],name,17);
      for(int k=0;k<3;k++)if(s.doses[k][i]&&!name[0])ok=false;
    }
    if(!ok){emit("rejected","Check 3 times, 4 names, counts 0-10");return;}
    s.enabled=p["enabled"];if(!memcmp(&s,&journal.value,sizeof(s))){emit("config_saved");sendConfig();return;}
    if(save(s)){emit("config_saved");sendConfig();}return;
  }
  if(!strcmp(cmd,"arm")){
    DateTime t;
    if(startupPending||!HARDWARE_COMMISSIONED||!hardwareOK||!journal.good||journal.damaged||!journal.present||journal.value.fault||state!=IDLE||!lidSafe()||digitalRead(STOP_PIN)||!timeNow(t)){
      emit("rejected","Commissioning/hardware/clock required");return;}
    armed=true;hostAt=millis();emit("status");return;
  }
  if(!strcmp(cmd,"refill")){
    if(!Serial||!p["container"].is<int>()){emit("rejected","USB and container 1-4 required");return;}
    int c=p["container"].as<int>();
    if((state!=IDLE&&state!=REFILL)||digitalRead(STOP_PIN)||c<1||c>4){emit("rejected","Refill unavailable");return;}
    if(state==IDLE)refillOpened=false;
    off();state=REFILL;hostAt=millis();selectRefillLED(c);
    snprintf(statusDetail,sizeof(statusDetail),"Refill container %d",c);emit("refill_ready");return;
  }
  if(!strcmp(cmd,"refill_done")){
    if(state!=REFILL||!lidSafe()){emit("rejected","Close cover for one second");return;}
    rememberBusyPeriods();selectRefillLED(0);state=IDLE;copyText(statusDetail,"Refill finished",sizeof(statusDetail));emit("refill_done");return;
  }
  if(!strcmp(cmd,"refill_cancel")){
    selectRefillLED(0);
    if(state==REFILL&&lidSafe()){rememberBusyPeriods();state=IDLE;}
    emit("refill_cancelled","Refill lights off");return;
  }
  if(!strcmp(cmd,"dispense")){
    // Manual test convenience; confirmation is in the computer UI. Never auto-retry.
    const char* id=p["id"]|"";JsonArray a=p["counts"].as<JsonArray>();uint8_t c[4]={};bool ok=a.size()==4&&strlen(id)>0&&strlen(id)<24;
    static char lastID[24]="";if(!strcmp(id,lastID))ok=false;
    for(int i=0;i<4&&ok;i++)if(!a[i].is<int>()||a[i].as<int>()<0||a[i].as<int>()>10)ok=false;else c[i]=a[i];
    if(!ok||!startDose(c,false,id)){emit("rejected","Manual dose unavailable");return;}
    copyText(lastID,id,24);hostAt=millis();return;
  }
  if(!strcmp(cmd,"bench")){
    int i=p["container"]|0,us=p["us"]|0;
    if(HARDWARE_COMMISSIONED||state!=IDLE||!hardwareOK||digitalRead(STOP_PIN)||!lidSafe()||strcmp(p["confirm"]|"","EMPTY")||i<1||i>4||us<800||us>2200){
      emit("rejected","Empty bench only; check cover/stop");return;}
    off();if(!attachAt(i-1,us))return;state=BENCH;benchUntil=millis()+500;hostAt=millis();emit("bench");return;
  }
  emit("rejected","Unknown command");
}
void readHost(){
  for(int budget=32;budget--&&Serial.available();){
    char c=Serial.read();if(c=='\r')continue;
    if(c=='\n'){if(!overflow&&used){input[used]=0;JsonDocument p;if(!deserializeJson(p,input))command(p);else emit("rejected","Bad JSON");}
      else if(overflow)emit("rejected","Line too long");used=0;overflow=false;
    }else if(used<sizeof(input)-1&&!overflow)input[used++]=c;else overflow=true;
  }
}
void scheduler(){
  static uint32_t at=0;if(millis()-at<1000)return;at=millis();
  DateTime t;if(!timeNow(t)){if(armed)fail("RTC invalid");return;}
  snprintf(clockRow,17,"%02u/%02u %02u:%02u",t.day(),t.month(),t.hour(),t.minute());
  if(startupPending||!journal.present||!journal.good||journal.damaged||!journal.value.enabled)return;
  int minute=t.hour()*60+t.minute();uint32_t day=dayOf(t);
  if(moving()){
    // Remember busy-period misses in RAM, avoiding flash writes during motion.
    rememberBusyPeriods();return;
  }
  for(int s=0;s<3;s++)if(busyDays[s]){
    if(busyDays[s]>journal.value.days[s]){
      Snapshot v=journal.value;v.days[s]=busyDays[s];
      // Do not overwrite an interrupted-dose result with an unrelated missed period.
      if(!v.active&&!v.fault){v.outcome=MISSED;v.timestamp=busyStamp[s];v.done=v.total=0;}
      if(!save(v))return;emit("missed","Scheduled time occurred while busy");
    }
    busyDays[s]=busyStamp[s]=0;
  }
  for(int s=0;s<3;s++){
    uint8_t sum=0;for(int k=0;k<4;k++)sum+=journal.value.doses[s][k];
    if(!sum||!unclaimed(journal.value,s,day,minute))continue;
    bool due=minute==journal.value.times[s]&&armed&&state==IDLE&&lidSafe()&&!digitalRead(STOP_PIN);
    Snapshot v=journal.value;v.days[s]=day;
    if(!due){
      if(!v.active&&!v.fault){v.outcome=MISSED;v.timestamp=t.unixtime();v.done=v.total=0;}
      if(!save(v))return;emit("missed","Late, disabled, refill or fault");continue;
    }
    // Claim is committed separately before active marker; interruption skips rather than duplicates.
    if(!save(v))return;char id[24];snprintf(id,24,"S%lu%d",day,s);
    startDose(journal.value.doses[s],true,id);return;
  }
}
void setup(){
  for(int i=0;i<4;i++){digitalWrite(REFILL_LED_PINS[i],LOW);pinMode(REFILL_LED_PINS[i],OUTPUT);}
  selectRefillLED(0);
  off();pinMode(STOP_PIN,INPUT_PULLUP);pinMode(LID_PIN,INPUT_PULLUP);pinMode(BUZZER_PIN,OUTPUT);
  Serial.begin(115200);Wire.begin();Wire.setWireTimeout(25000,true);
  bool irqOK=true;for(int i=0;i<4;i++){pinMode(BEAM_PINS[i],INPUT);beams[i].blocked=blocked(i);beams[i].started=micros();
    attachInterrupt(digitalPinToInterrupt(BEAM_PINS[i]),callbacks[i],CHANGE);irqOK&=getIrqIndexFromPin(BEAM_PINS[i])>=0;}
  hardwareOK=lcd.begin(16,2)==0&&irqOK;rtcOK=rtc.begin()&&!rtc.lostPower();journal.load();
  bool abnormal=(R_SYSTEM->RSTSR1&3U)||(R_SYSTEM->RSTSR0&14U);R_SYSTEM->RSTSR1=0;R_SYSTEM->RSTSR0=0;
  if(journal.damaged||journal.value.active||journal.value.fault||abnormal){
    done=journal.value.done;total=journal.value.total;fail("Restart: inspect tray");
  }
  // Startup arming occurs after cover debounce and clock checks, without waiting for USB.
  hostAt=millis();if(!WDT.begin(2000)){hardwareOK=false;fail("Watchdog unavailable");}
  bootAt=millis();emit("boot");
}
void loop(){
  WDT.refresh();
  if(digitalRead(LID_PIN)){
    lidClosedAt=0;refillOpened=true;if(moving())fail("Cover opened during motion");
    else if(state==IDLE){off();state=REFILL;copyText(statusDetail,"Manual refill; close cover",sizeof(statusDetail));}
  }else if(!lidClosedAt)lidClosedAt=millis();
  if(digitalRead(STOP_PIN)&&state!=FAULT)fail("Physical stop open");
  if(startupPending&&millis()-bootAt>1500){
    startupPending=false;DateTime t;
    armed=HARDWARE_COMMISSIONED&&hardwareOK&&journal.good&&!journal.damaged&&journal.present&&!journal.value.fault&&state==IDLE&&lidSafe()&&!digitalRead(STOP_PIN)&&timeNow(t);
  }
  serviceRefillLEDs();
  if(state==REFILL&&refillOpened&&lidSafe()){rememberBusyPeriods();selectRefillLED(0);state=IDLE;copyText(statusDetail,"Cover closed",sizeof(statusDetail));emit("refill_done");}
  if(moving()&&!scheduled&&millis()-hostAt>HOST_TIMEOUT_MS)fail("Manual USB lost");
  updateDose();buzzer();flush();readHost();scheduler();display();serviceLCD();
  static uint32_t busAt=0,statusAt=0;
  if(millis()-busAt>=500){busAt=millis();Wire.beginTransmission(LCD_ADDRESS);bool ok=Wire.endTransmission()==0;
    Wire.beginTransmission(0x68);ok&=Wire.endTransmission()==0;if(!ok&&state!=FAULT)fail("I2C device failure");}
  if(configPending&&!txLen)sendConfig();
  if(millis()-statusAt>=1000&&!txLen){statusAt=millis();emit("status");}
}


