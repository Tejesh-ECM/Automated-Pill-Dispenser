# Budget project build guide

## Final structure
Computer --USB--> UNO R4 --four servo signals--> four dispensing mechanisms.
UNO R4 also reads four conditioned IR barriers, a stop sense contact and a shared refill-cover switch.
Four low-current LEDs beside the refill ports indicate the software-selected container. An I2C RTC provides time; a 16x2 I2C LCD displays status; a transistor-driven passive piezo provides beeps.
A fixed receiving tray sits beneath the chute. The system has no receiver sensor.
Schedules and recovery records are stored inside the UNO R4.

A separate enclosed 5 V motor adapter supplies four fused motor branches through a hard stop contact.
A separate 9 V adapter supplies the UNO barrel jack. USB is only for computer communication/setup.

## What to buy
Use ELECTRONICS_AND_COST.md for installed electronics only. The major remaining costs are the UNO R4, motor supply and appropriately DC-rated stop.
Obtain the actual servo, LCD, adapters and switch before committing mounting dimensions. The 5 V supply is a candidate pending current/torque tests, not a qualified substitute for every servo.
Reuse an existing UNO R4 and USB data cable if available.

Mechanical materials: four bearing-supported original dispenser assemblies, servo horns/screws, compatible fasteners, a cleanable tray liner, captive guard fasteners/strap and printed parts.
The eight new templates have example dimensions. Determine inlet positions from the actual full assembly rather than guessing from extracted local coordinates.
Tools: soldering iron, multimeter, calipers, insulated hand tools; scope/logic analyser access for pulse and startup checks.

## Assembly order
1. Assemble ONE original bowl/rotor/servo mechanism. Compare the actual servo shaft and mounting dimensions; the original CAD servo label differs from the SG90 purchasing choice.
2. Test the original closure and rotor before trying pill-profile rotor candidates. A shaped opening alone does not prevent two stacked pills.
3. Position four labelled independent fill ports above their respective containers. Replace the motorized selector with direct manual access. Fit caps and a shared cover; adapt the cover-switch bracket so the switch closes only when the cover is fitted.
4. Fit a fixed tray/cradle beneath the common outlet and secure it to the base. Use a removable cleanable liner. Its capacity must accommodate the largest configured dose. There is no detection of remaining contents.
5. Fit the smaller LCD plate and controller plate using insulated standoffs. Keep electronics and joints out of pill/dust paths.
6. Solder the four optical channels and buzzer driver on perfboard. Add one 1 kohm resistor per refill LED, driven from A0-A3; leave raw optical nodes on test pads, not on those pins. Follow the exact Schmitt gate and resistor table in WIRING.md.
7. Connect logic first, with motor branches disconnected. Check every voltage and input before attaching motors.
8. Connect one fused servo at a time. Complete empty-bench pulse calibration before installing all four.

## Firmware installation
Install Arduino Renesas UNO board package 1.6.0. Install Servo 1.3.0, RTClib 2.1.4, Adafruit BusIO 1.17.4, ArduinoJson 7.4.3 and hd44780 1.3.2.
Use the UNO R4 Minima board option for Minima, or UNO R4 WiFi for that board. No network setup is used.
Open firmware/SmartPillBudget/SmartPillBudget.ino. Check LCD address in config.h.
Leave HARDWARE_COMMISSIONED=false for the first upload. Example servo pulse values are not calibrated.
Follow ACCEPTANCE_TESTS.md, including the guarded bench JSON command, then edit pulse values/filter timing and recompile.
Set HARDWARE_COMMISSIONED=true only for the supervised inert-object acceptance phase, then retain only proven settings on the physical prototype.
A hardware upload may reset the controller. It must not resume an unfinished dose.

## Computer software
Install Python 3 with the Tcl/Tk option and run Setup.bat (installs pyserial 3.5).
Run Start.bat, select the USB COM port and Connect. The app checks protocol 4.
Set the device clock from the computer after checking local date/time (Chennai is Asia/Kolkata).
Fill four medicine labels (up to 16 plain ASCII characters), three distinct HH:MM times, and whole-pill counts 0–10.
Check Enable saved daily schedule, then Save to dispenser. Wait for verified settings readback, not just an acknowledgement. Pending uploads disable manual dosing and refill selection until the app receives the device configuration.
Enable controls after commissioning. Keep both external supplies on, then disconnect USB to test the autonomous schedule.
The app does not calculate a prescription or infer a pill count from strength.

A manual dose is EXTRA and does not consume/replace a scheduled dose. It requires an explicit computer confirmation and must not be retried automatically after an uncertain USB outcome.
Connected result snapshots are appended to data/events.jsonl. last_uploaded.json is a convenience copy written only after matching device readback.
Only the latest device outcome is available after offline operation; this is not a full offline history.

## Daily operation
Before each scheduled time: verify the correct medicines/quantities, empty and fit the cleanable tray liner, close all refill caps and the shared cover, release the stop and check the display is enabled.
At the scheduled minute, the controller records a daily claim and an active marker BEFORE motor movement.
It dispenses one container at a time using the IR counters, closes and detaches each servo, stores DISPENSED, beeps twice and returns idle.
No wait for cup presence, removal, collection acknowledgement or computer confirmation occurs for scheduled doses.
If a time is missed, disabled, busy or refilling, it is skipped. There is no automatic catch-up.
If the earlier dose remains, a later dose WILL join it. This design therefore requires a supervised tray-emptying routine.

## Refill
Wait for IDLE. Open the shared cover: firmware enters REFILL and inhibits dosing.
Read the matching label, open only that container's cap, pour its medicine, and close the cap.
Close the shared cover; after one second the device returns idle. Cover opening during motor movement causes a latched fault.
For guided refill, plug in USB, read the saved container labels, select one in the Refill guide selector and click Light selected container. Wait for device confirmation: only that LED illuminates. The LCD shows its number and saved label. Open the cover and fill that port. Replace its cap before selecting another container. Closing the shared cover for one second clears the lights and ends refill; Finish refill also works with a closed cover.
If USB is unplugged or the app stops responding, the indicator clears within five seconds. An open cover continues to inhibit dosing. Offline manual refilling remains available without a light. The LED indicates selection, not detection or confirmation that medicine was added.

## Faults and recovery
Operate the physical stop when needed. Detaching a servo does not physically guarantee gate closure.
Inspect/empty the tray and all paths, remove partial/extra doses, resolve the fault, close the cover and release stop.
Use Clear fault after inspection, then Enable controls. Clear consumes already-due periods for today to prevent catch-up during recovery.
A power failure with an unfinished job gives a fault after restart. The controller does not resume the motor.
The storage journal uses eight independently erased pages with CRC-checked full snapshots. A torn page causes an inspection fault; clear repairs it only when a valid snapshot remains.
If ALL snapshots are damaged, normal clear cannot recover the lost schedule/claims. Use firmware/ResetStorage only with empty mechanics and motor power disconnected, then reconfigure from scratch. This deliberately discards old history/claims and requires inspection before use.
No specific flash lifetime is claimed. Avoid repeatedly uploading unchanged settings or toggling controls as a stress test with real medicine.

## Before relying on the build
Complete every test in ACCEPTANCE_TESTS.md. Keep the record of dimensions, current draw, pulse measurements and counting trials.
A successful compile, simulated power cut or geometrically closed STL does not demonstrate reliable real-pill dispensing.

