#pragma once
struct WireStub {bool healthy=true;void begin(){}void setWireTimeout(int,bool){}void beginTransmission(int){}int endTransmission(){return healthy?0:1;}};
inline WireStub Wire;

