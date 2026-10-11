#include <cassert>
#include <iostream>
#include "../firmware/SmartPillBudget/beam_counter.h"
int main(){
 volatile BeamCounter b={};
 b.edge(true,100,300);b.edge(false,399,300);assert(b.pending==0);
 b.edge(true,500,300);b.edge(false,800,300);assert(b.pending==1);
 b.edge(false,900,300);assert(b.pending==1);
 b.edge(true,0xFFFFFF00UL,300);b.edge(false,0x100,300);assert(b.pending==2);
 for(int i=0;i<300;i++){b.edge(true,i*1000,300);b.edge(false,i*1000+500,300);}
 assert(b.pending==255);
 std::cout<<"PASS: short pulse rejection, complete pulse, duplicate edge, micros rollover, event saturation\n";
}

