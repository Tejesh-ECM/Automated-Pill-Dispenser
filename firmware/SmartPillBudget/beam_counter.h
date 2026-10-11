#pragma once
#include <stdint.h>
struct BeamCounter {
  bool blocked;uint32_t started;uint8_t pending;
  void edge(bool value,uint32_t now,uint16_t minimum) volatile {
    if(value==blocked)return;
    if(value)started=now;
    else if((uint32_t)(now-started)>=minimum && pending<255)pending++;
    blocked=value;
  }
};
