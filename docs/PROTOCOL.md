# Budget USB protocol 4
115200 baud, newline-delimited ASCII JSON; input maximum 599 characters.
Identification: protocol=4, firmware=SmartPillBudget-R4-4.
Commands: ping; get_config; set_config; set_time; arm; stop; clear; refill; refill_done; refill_cancel; dispense; bench.
Configuration example:
{"cmd":"set_config","enabled":true,"times":[480,840,1200],"names":["A","B","C","D"],"doses":[[1,0,0,0],[0,1,0,0],[0,0,1,1]]}
set_time uses timestamp encoding local calendar fields as a Unix-style integer (desktop uses calendar.timegm on local fields).
Manual example: {"cmd":"dispense","id":"Munique001","counts":[1,0,0,0]}. This is an extra dose; no automatic retries.
Bench example, factory firmware only: {"cmd":"bench","container":1,"us":1500,"confirm":"EMPTY"}. One half-second empty-mechanism movement.
States: IDLE, REFILL, DISPENSING, CLOSING, BENCH, FAULT.
Status includes armed, lid_closed, rtc_ok, done, total, detail, two lcd rows, sequence, outcome and timestamp.
Persistent outcomes: NONE, STARTED, DISPENSED, MISSED, INTERRUPTED. Only the last outcome is retained.
There are no cup, cup_valid, collected, WAIT_CUP or WAIT_COLLECTION fields/states.
Status reconciliation handles bounded/dropped transient messages; commands causing movement are never automatically retried.
read/set configuration is deferred/rejected while moving; no journal writes occur during servo motion.
The app keeps DTR asserted for native USB. The scheduler never waits for Serial.


## Refill LED commands
{"cmd":"refill","container":2} selects container 2, enters/continues REFILL and lights only its LED.
Requires a live USB Serial connection, container integer 1-4, released stop and state IDLE or REFILL. It can be used while the cover is open. Selections during dosing, bench motion or faults are rejected.
{"cmd":"refill_done"} requires REFILL and a cover closed for one second; clears LEDs and returns IDLE.
{"cmd":"refill_cancel"} clears LEDs; remains inhibited if the cover is open.
Periodic status includes refill_selected (0=all off, 1-4=selected).
Ping renews the 5-second USB heartbeat. A lost heartbeat clears the selection without recording a completed refill. Restart never restores a previous selection.
