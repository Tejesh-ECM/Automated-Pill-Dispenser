#include <cassert>
#include <iostream>
#include "../firmware/SmartPillBudget/SmartPillBudget.ino"
int getIrqIndexFromPin(uint32_t p){return p;}
void reset(){
 off();selectRefillLED(0);EEPROM=FakeEEPROM();journal=Journal();journal.load();
 auto s=defaults();s.enabled=1;s.doses[0][0]=1;assert(journal.save(s));
 state=IDLE;armed=true;hardwareOK=true;rtcOK=true;rtc.lost=false;startupPending=false;
 for(int s=0;s<3;s++)busyDays[s]=busyStamp[s]=0;
 Serial.connected=false;Wire.healthy=true;scheduled=false;refillOpened=false;
 done=total=0;idx=0;nowMs+=10000;lidClosedAt=nowMs-2000;hostAt=nowMs;rtcMinute=480;rtcDay=9;
 for(int i=0;i<4;i++){pins[BEAM_PINS[i]]=HIGH;beams[i].pending=0;beams[i].blocked=false;beams[i].started=micros();}
 pins[STOP_PIN]=pins[LID_PIN]=LOW;
}
void cmd(const char* text){JsonDocument p;assert(!deserializeJson(p,text));command(p);}
int main(){
 // Startup must not consume a due period before automatic enabling settles.
 reset();startupPending=true;scheduler();assert(journal.value.days[0]==0&&state==IDLE);
 startupPending=false;nowMs+=1001;scheduler();assert(state==DISPENSING);
 // Stale idle pulses on another outlet must not poison the next job.
 reset();beams[2].pending=1;scheduler();updateDose();assert(state==DISPENSING);
 // A due period observed during another job is skipped, even if it ends this minute.
 reset();rtcMinute=479;uint8_t one[4]={1,0,0,0};assert(startDose(one,false,"busy"));
 rtcMinute=480;nowMs+=1001;scheduler();assert(busyDays[0]==20261009&&journal.value.days[0]==0);
 beams[0].pending=1;updateDose();nowMs+=501;updateDose();assert(state==IDLE);
 nowMs+=1001;scheduler();assert(state==IDLE&&journal.value.days[0]==20261009&&journal.value.outcome==MISSED);
 // Missing another period cannot erase evidence of an interrupted job.
 reset();state=REFILL;Serial.connected=true;cmd("{\"cmd\":\"refill_done\"}");
 assert(state==IDLE&&busyDays[0]==20261009);nowMs+=1001;scheduler();assert(state==IDLE&&journal.value.outcome==MISSED);
 reset();scheduler();fail("test interruption");rtcMinute=900;nowMs+=1001;scheduler();
 assert(journal.value.outcome==INTERRUPTED&&journal.value.active&&state==FAULT);
 // The interlock is checked at the actual attachment boundary.
 reset();pins[STOP_PIN]=HIGH;assert(!attachAt(0,1500)&&state==FAULT&&!motors[0].attached());
 reset();scheduler();assert(state==DISPENSING&&scheduled&&motors[0].attached());
 assert(journal.value.days[0]==20261009&&journal.value.active);
 beams[0].pending=1;updateDose();assert(state==CLOSING);
 nowMs+=501;updateDose();assert(state==IDLE&&!journal.value.active&&journal.value.outcome==DISPENSED);
 for(auto &m:motors)assert(!m.attached());
 nowMs+=1100;scheduler();assert(state==IDLE); // same day cannot repeat
 // No cup signal, collection state, or computer needed for completion.
 reset();scheduler();nowMs+=6000;updateDose();assert(state==DISPENSING); // offline scheduled operation
 pins[LID_PIN]=HIGH;loop();assert(state==FAULT&&!armed&&!motors[0].attached());
 reset();pins[LID_PIN]=HIGH;loop();assert(state==REFILL);
 scheduler();assert(journal.value.outcome==MISSED&&!motors[0].attached());
 reset();scheduler();beams[0].pending=2;updateDose();assert(state==FAULT);
 reset();scheduler();beams[0].pending=1;updateDose();beams[0].pending=1;updateDose();assert(state==FAULT);
 reset();scheduler();nowMs+=8001;updateDose();assert(state==FAULT);
 reset();EEPROM.fail=true;scheduler();assert(state==FAULT);for(auto &m:motors)assert(!m.attached());
 reset();cmd("{\"cmd\":\"dispense\",\"id\":\"M1\",\"counts\":[1,0,0,0]}");
 assert(state==DISPENSING&&!scheduled);nowMs+=6000;loop();assert(state==FAULT);
 reset();Serial.connected=true;cmd("{\"cmd\":\"refill\",\"container\":1}");loop();assert(state==REFILL); // does not immediately exit with cover shut
 cmd("{\"cmd\":\"refill_done\"}");assert(state==IDLE);
 reset();cmd("{\"cmd\":\"set_config\",\"enabled\":true,\"times\":[480,480,1200],\"names\":[\"A\",\"B\",\"C\",\"D\"],\"doses\":[[1,0,0,0],[0,0,0,0],[0,0,0,0]]}");
 assert(journal.value.times[1]==840);
 reset();pins[STOP_PIN]=HIGH;loop();assert(state==FAULT);
 pins[STOP_PIN]=LOW;lidClosedAt=nowMs-2000;cmd("{\"cmd\":\"clear\"}");
 assert(state==IDLE&&!armed&&journal.value.days[0]==20261009);
 reset();scheduler();Journal reboot;reboot.load();assert(reboot.value.active); // setup must latch interrupted job
 // Refill guide: live selection, exclusive outputs, switching, invalid requests.
 reset();cmd("{\"cmd\":\"refill\",\"container\":1}");assert(refillSelected==0&&state==IDLE);
 Serial.connected=true;
 for(int c=1;c<=4;c++){
   char request[64];snprintf(request,sizeof(request),"{\"cmd\":\"refill\",\"container\":%d}",c);cmd(request);
   assert(refillSelected==c&&state==REFILL);
   for(int i=0;i<4;i++)assert(pins[REFILL_LED_PINS[i]]==(i==c-1?HIGH:LOW));
 }
 cmd("{\"cmd\":\"refill\",\"container\":5}");assert(refillSelected==4);
 cmd("{\"cmd\":\"refill\",\"container\":\"2\"}");assert(refillSelected==4);
 cmd("{\"cmd\":\"refill_done\"}");assert(state==IDLE&&refillSelected==0);
 for(int i=0;i<4;i++)assert(pins[REFILL_LED_PINS[i]]==LOW);
 // An open cover stays inhibited when heartbeat/USB guidance is lost.
 pins[LID_PIN]=HIGH;loop();cmd("{\"cmd\":\"refill\",\"container\":2}");
 assert(state==REFILL&&refillSelected==2);
 cmd("{\"cmd\":\"refill_done\"}");assert(state==REFILL&&refillSelected==2);
 nowMs+=HOST_TIMEOUT_MS+1;loop();assert(state==REFILL&&refillSelected==0);
 cmd("{\"cmd\":\"refill\",\"container\":3}");assert(refillSelected==3);
 Serial.connected=false;loop();assert(state==REFILL&&refillSelected==0);
 Serial.connected=true;cmd("{\"cmd\":\"refill\",\"container\":1}");
 pins[LID_PIN]=LOW;loop();nowMs+=1001;loop();assert(state==IDLE&&refillSelected==0);
 // Fault, cancellation and active dosing never leave a guidance light on.
 cmd("{\"cmd\":\"refill\",\"container\":1}");cmd("{\"cmd\":\"stop\"}");
 assert(state==FAULT&&refillSelected==0);
 cmd("{\"cmd\":\"refill\",\"container\":2}");assert(refillSelected==0);
 reset();Serial.connected=true;cmd("{\"cmd\":\"refill\",\"container\":4}");
 cmd("{\"cmd\":\"refill_cancel\"}");assert(refillSelected==0&&state==IDLE);
 reset();scheduler();Serial.connected=true;cmd("{\"cmd\":\"refill\",\"container\":2}");
 assert(state==DISPENSING&&refillSelected==0);
 std::cout<<"Refill LED tests: exclusivity, selection, finish, invalid input, cover, disconnect, timeout, cancel and fault passed\n";
 std::cout<<"Firmware state tests: offline schedule, no repeat, completion, refill, lid, stop, timeout, excess counts, USB loss and storage failure passed\n";
}

