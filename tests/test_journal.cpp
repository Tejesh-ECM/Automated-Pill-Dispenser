#include <cassert>
#include <iostream>
#include "../firmware/SmartPillBudget/journal.h"
int main(){
 Journal j;j.load();assert(!j.present&&!j.damaged);
 auto s=defaults();s.enabled=1;s.doses[0][0]=2;assert(j.save(s));
 for(int n=0;n<30;n++){s=j.value;s.days[0]=20261009+n;assert(j.save(s));}
 Journal r;r.load();assert(r.value.days[0]==20261038&&!r.damaged);
 // Previous active snapshot must survive every interrupted completion write.
 s=r.value;s.active=1;s.outcome=STARTED;s.total=2;s.done=0;assert(r.save(s));
 uint8_t backup[8192];memcpy(backup,EEPROM.bytes,8192);
 for(size_t cut=0;cut<sizeof(Snapshot);cut++){
   memcpy(EEPROM.bytes,backup,8192);EEPROM.cut=-1;Journal a;a.load();
   auto end=a.value;end.active=0;end.done=2;end.outcome=DISPENSED;EEPROM.cut=cut;
   assert(!a.save(end));assert(!a.good);
   Journal b;b.load();assert(b.present&&b.value.active);
 }
 memcpy(EEPROM.bytes,backup,8192);EEPROM.cut=-1;
 Journal a;a.load();s=a.value;s.active=0;s.done=2;s.outcome=DISPENSED;assert(a.save(s));
 Journal b;b.load();assert(!b.value.active&&b.value.outcome==DISPENSED);
 // Torn page + valid snapshot: explicit repair retains claims and replaces bad records.
 EEPROM.bytes[((b.current+1)%8)*1024]=0;Journal bad;bad.load();assert(bad.damaged);
 s=bad.value;s.active=0;s.fault=0;assert(bad.repair(s));Journal fixed;fixed.load();assert(!fixed.damaged);
 s=fixed.value;auto previous=s.days[0];consumePast(s,20261009,800);assert(s.days[0]>=previous);
 s.days[0]=20261009;assert(!unclaimed(s,0,20261009,480));assert(!unclaimed(s,0,20261008,480));
 assert(unclaimed(s,0,20261010,480));assert(!unclaimed(s,0,20261010,479));
 s=defaults();consumePast(s,20261009,840);assert(s.days[0]==20261009&&s.days[1]==20261009&&s.days[2]==0);
 // Storage I/O failure is sticky; invalid schedules cannot be committed.
 EEPROM.fail=true;assert(!fixed.save(s));assert(!fixed.good);EEPROM.fail=false;
 Journal invalid;invalid.load();s=invalid.value;s.times[1]=s.times[0];assert(!invalid.save(s));
 std::cout<<"Journal: interrupted page writes, recovery, rotation, claims, validation and I/O failure passed\n";
}

