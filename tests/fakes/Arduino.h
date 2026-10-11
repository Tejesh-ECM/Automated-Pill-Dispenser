#pragma once
#include <stdint.h>
#include <stddef.h>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <algorithm>
using std::min;using std::max;
#define ARDUINO_UNOR4_MINIMA 1
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define CHANGE 3
inline uint32_t nowMs=10000;
inline int pins[32]={};
inline uint32_t millis(){return nowMs;}
inline uint32_t micros(){return nowMs*1000;}
inline int digitalRead(int p){return pins[p];}
inline void digitalWrite(int p,int v){pins[p]=v;}
inline void pinMode(int,int){}
inline void noInterrupts(){}
inline void interrupts(){}
inline int digitalPinToInterrupt(int p){return p;}
inline void attachInterrupt(int,void(*)(),int){}
inline void tone(int,int){}
inline void noTone(int){}
struct FakeSerial {
 bool connected=false;
 operator bool()const{return connected;}
 void begin(int){}
 int available(){return 0;}int read(){return -1;}
 int availableForWrite(){return 800;}
 size_t write(const uint8_t*,size_t n){return n;}
};
inline FakeSerial Serial;
struct Registers {uint32_t RSTSR1=0,RSTSR0=0;};
inline Registers reg;inline Registers* R_SYSTEM=&reg;

