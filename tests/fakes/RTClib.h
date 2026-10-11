#pragma once
inline int rtcMinute=480,rtcDay=9;
class DateTime {
 public:DateTime(){}DateTime(uint32_t){}
 int year()const{return 2026;}int month()const{return 10;}int day()const{return rtcDay;}
 int hour()const{return rtcMinute/60;}int minute()const{return rtcMinute%60;}
 bool isValid()const{return true;}uint32_t unixtime()const{return 1791504000+rtcMinute*60;}
};
struct RTC_DS3231 {bool lost=false;bool begin(){return true;}bool lostPower(){return lost;}DateTime now(){return DateTime();}void adjust(DateTime){lost=false;}};

