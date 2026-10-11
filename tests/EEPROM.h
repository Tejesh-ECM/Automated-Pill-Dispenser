#pragma once
#include <stdint.h>
#include <string.h>
#include <algorithm>
struct FakeEEPROM {
 uint8_t bytes[8192];int cut=-1,writes=0;bool fail=false;
 FakeEEPROM(){memset(bytes,255,sizeof(bytes));}
 int length(){return sizeof(bytes);}
 template<class T> void get(int a,T& v){memcpy(&v,bytes+a,sizeof(v));}
 template<class T> void put(int a,const T& v){
   writes++;if(fail)return;
   // Model a full independent erase followed by a possibly interrupted page program.
   memset(bytes+(a/1024)*1024,255,1024);
   memcpy(bytes+a,&v,cut<0?sizeof(v):std::min(sizeof(v),(size_t)cut));
 }
};
inline FakeEEPROM EEPROM;

