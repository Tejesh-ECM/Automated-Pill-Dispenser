/* Deliberate recovery utility. Motor-positive supply MUST be disconnected.
   Select the actual UNO R4 board. STOP must be OPEN throughout the reset.
   RTC must already have a valid local date. Type RESET_EMPTY, newline.
   This discards configuration and prevents scheduled operation for the rest of today.
*/
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "journal.h"
RTC_DS3231 rtc;
char line[32];uint8_t used=0;bool overflow=false;
void setup(){
  for(int p=2;p<=5;p++){digitalWrite(p,LOW);pinMode(p,OUTPUT);}
  const uint8_t leds[4]={A0,A1,A2,A3};for(int i=0;i<4;i++){digitalWrite(leds[i],LOW);pinMode(leds[i],OUTPUT);}
  pinMode(9,INPUT_PULLUP);Serial.begin(115200);Wire.begin();rtc.begin();
}
void loop(){
  while(Serial.available()){
    char c=Serial.read();if(c=='\r')continue;
    if(c!='\n'){if(used<31&&!overflow)line[used++]=c;else overflow=true;continue;}
    line[used]=0;
    bool approved=!overflow&&!strcmp(line,"RESET_EMPTY")&&digitalRead(9)==HIGH;
    used=0;overflow=false;
    DateTime t=rtc.now();
    if(!approved||rtc.lostPower()||!t.isValid()||t.year()<2025||t.year()>2099){
      Serial.println("REFUSED: disconnect motor power, open STOP, validate RTC, type RESET_EMPTY");continue;}
    Snapshot s=defaults();s.magic=MAGIC;s.sequence=1;
    uint32_t day=(uint32_t)t.year()*10000+t.month()*100+t.day();
    for(int k=0;k<3;k++)s.days[k]=day;
    s.crc=crc16((uint8_t*)&s,sizeof(s)-2);bool ok=true;
    for(int page=0;page<PAGES;page++){
      if(digitalRead(9)!=HIGH){ok=false;break;}
      EEPROM.put(page*PAGE,s);Snapshot check;EEPROM.get(page*PAGE,check);
      if(memcmp(&s,&check,sizeof(s))||!validSnapshot(check)){ok=false;break;}
    }
    Serial.println(ok?"RESET VERIFIED. Upload operating sketch, configure, inspect. Today is skipped.":"FAILED. Keep motors disconnected.");
  }
}

