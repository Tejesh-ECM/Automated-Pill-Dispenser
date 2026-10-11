#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <EEPROM.h>
// One complete snapshot per independently erased 1024-byte page.
// Never use EEPROM.update byte-by-byte for a transaction on UNO R4.
enum Outcome : uint8_t { NONE, STARTED, DISPENSED, MISSED, INTERRUPTED };
struct __attribute__((packed)) Snapshot {
  uint32_t magic,sequence;
  uint16_t times[3];
  uint8_t doses[3][4];
  char names[4][17];
  uint32_t days[3];
  uint8_t enabled,active,fault,outcome,done,total;
  uint32_t timestamp;
  uint16_t crc;
};
constexpr uint32_t MAGIC=0x42554433;
constexpr int PAGE=1024, PAGES=8;
static_assert(sizeof(Snapshot)<PAGE,"Snapshot crosses erase boundary");
inline uint16_t crc16(const uint8_t* p,size_t n){
  uint16_t c=0xffff;while(n--){c^=*p++;for(int i=0;i<8;i++)c=(c&1)?(c>>1)^0xa001:c>>1;}return c;
}
inline bool validSnapshot(const Snapshot& s){
  if(s.magic!=MAGIC||!s.sequence||s.crc!=crc16((const uint8_t*)&s,sizeof(s)-2))return false;
  if(s.enabled>1||s.active>1||s.fault>1||s.outcome>INTERRUPTED||s.done>s.total||s.total>40)return false;
  for(int k=0;k<3;k++){
    if(s.times[k]>1439)return false;
    for(int j=0;j<k;j++)if(s.times[k]==s.times[j])return false;
    for(int i=0;i<4;i++)if(s.doses[k][i]>10)return false;
  }
  for(int i=0;i<4;i++)if(s.names[i][16])return false;
  return true;
}
inline Snapshot defaults(){
  Snapshot s={};s.times[0]=480;s.times[1]=840;s.times[2]=1200;
  for(int i=0;i<4;i++){strcpy(s.names[i],"Container 1");s.names[i][10]='1'+i;}
  return s;
}
struct Journal {
  Snapshot value=defaults();int current=-1;bool present=false,damaged=false,good=true;
  void load(){
    value=defaults();current=-1;present=false;damaged=false;good=EEPROM.length()>=PAGE*PAGES;
    if(!good){damaged=true;return;}
    for(int i=0;i<PAGES;i++){
      Snapshot s;EEPROM.get(i*PAGE,s);
      if(validSnapshot(s)){
        if(!present||s.sequence>value.sequence){value=s;current=i;present=true;}
      }else{
        const uint8_t* p=(const uint8_t*)&s;bool blank=true;
        for(size_t k=0;k<sizeof(s);k++)if(p[k]!=255)blank=false;
        if(!blank)damaged=true;
      }
    }
  }
  bool save(Snapshot next){
    if(!good||value.sequence==UINT32_MAX){good=false;return false;}
    next.magic=MAGIC;next.sequence=present?value.sequence+1:1;
    next.crc=crc16((const uint8_t*)&next,sizeof(next)-2);
    if(!validSnapshot(next)){good=false;return false;}
    int target=(current+1)%PAGES;
    EEPROM.put(target*PAGE,next);
    Snapshot check;EEPROM.get(target*PAGE,check);
    if(memcmp(&next,&check,sizeof(next))||!validSnapshot(check)){good=false;return false;}
    value=next;present=true;current=target;return true;
  }
  // Only after explicit inspection/clear; rewriting a full rotation replaces torn pages.
  bool repair(Snapshot next){
    if(!present)return false;
    for(int i=0;i<PAGES;i++)if(!save(next))return false;
    damaged=false;return true;
  }
};
inline bool unclaimed(const Snapshot& s,int slot,uint32_t day,int minute){
  return minute>=s.times[slot] && day>s.days[slot];
}
inline void consumePast(Snapshot& s,uint32_t day,int minute){
  for(int i=0;i<3;i++)if(minute>=s.times[i]&&day>s.days[i])s.days[i]=day;
}

