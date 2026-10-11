#pragma once
struct Servo {bool on=false;int us=0;void detach(){on=false;}bool attached(){return on;}uint8_t attach(int,int,int){on=true;return 1;}void writeMicroseconds(int v){us=v;}};

