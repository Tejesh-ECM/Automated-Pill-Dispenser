#pragma once
struct WDTStub {void refresh(){}bool begin(int){return true;}};inline WDTStub WDT;

