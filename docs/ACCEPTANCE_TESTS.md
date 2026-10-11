# Commissioning and acceptance record

No hardware was connected during development. Fill in measured results; a compiled sketch does not validate the dispenser.

1. **Power off:** verify polarity, stop contacts, common ground, fuses, wire gauge, insulated joints and transistor pin order. Confirm the stop power contact's DC breaking capacity. Measure the LCD and every servo mount before printing.
2. **Empty mechanics:** check each rotor closes without servo holding torque; the cover cannot be shut while caps protrude incorrectly. Confirm the tray is mechanically fixed under all outlets. Fit a removable cleanable liner.
3. **Logic only:** disconnect all motor-positive branches. Load factory sketch (HARDWARE_COMMISSIONED=false), connect USB, set RTC, read/write schedule, verify LCD and optical levels. Block each beam individually; test a four-channel pulse capture with inert objects. Verify the passive piezo has its separate 1 kohm discharge resistor across its terminals and produces clear start/completion/fault tones.
4. **Empty servo test:** connect ONE fused servo branch. Close the cover and release stop. In Serial Monitor (115200, newline), send a ping then, e.g. {"cmd":"bench","container":1,"us":1500,"confirm":"EMPTY"}. It attaches for 500 ms then stops. Explore small increments within 800–2200 only after checking the actual servo travel. Record OPEN_US/CLOSED_US values, direction and horn alignment. Never drive a hard stop.
5. **Power qualification:** measure startup, movement, stall/fault current and motor-rail drop using the actual servo model. Run repeated loaded dispensing at 5 V. Reject the 5 V supply option if torque or voltage stability is insufficient. Confirm regulator temperature on the UNO's separate 9 V supply, including LCD backlight load.
6. **Timing qualification:** verify servo attach has no unwanted move, optical pulses exceed the configured minimum (300 us initial value), and no pulses are lost while LCD/serial traffic and buzzer run. Confirm closed gate and stray pulses after closing. Set filter values from measured waveforms.
7. **Controlled dose trials:** after empty calibration, set HARDWARE_COMMISSIONED=true for supervised inert-object tests. Exercise every container at low/full fill, each specified profile, counts 1 and 10, and a dose spanning all four. Compare actual objects to counted events. Record double feeds, jams and missed counts; no acceptance on any unresolved miscount.
8. **No receiver detection:** with an empty fixed tray, dispense and confirm immediate DISPENSED then IDLE, with no collection wait. Leave test pieces there and verify the documented limitation: a later dose is not inhibited. Do not perform this with medicines.
9. **Cover and stop:** open cover during dispensing/closing; assert fault and disabled outputs. Activate stop and verify motor-positive voltage drops independently of firmware. Verify lid broken-wire and stop broken-wire faults. Clear only after inspecting/emptying the tray.
10. **RTC/offline:** set a future schedule, upload and arm, disconnect USB with independent supplies on. Verify exactly one dose. Reboot within that scheduled minute: no repeat. Move clock backward across an already claimed date: no repeat.
11. **Power-cut campaign:** interrupt power before claim, during claim write, during active-marker write, during each motor phase, and during completion write. Repeat across all eight page positions. There must be no automatic redispense and every unfinished/uncertain operation requires inspection. Host simulations do not replace these electrical tests.
12. **Faults:** block a sensor, simulate no pill, inject two optical pulses when only one remains, disconnect RTC/LCD, remove USB during a manual dose, and simulate storage write failure. Verify motor detachment, latched fault, meaningful status and no automatic retries.
13. **Refill:** open the cover while idle; verify refill mode. A schedule due while refilling must be marked missed, never replayed on closing. Closing for one second returns idle without motor movement.
14. **Wired app:** test both UNO R4 variants if used, DTR and reconnection, mismatched protocol rejection, changed settings acknowledgement, invalid times/counts, and manual-dose confirmation. Disconnect after sending a manual command: the app must not resend it.
15. **Wear and recovery:** writes occur only for settings changes, dose claims/start/completion, misses and latched fault/clear. Verify a sustained idle/ping session adds no writes. Internal flash has finite endurance; no lifetime claim is made from host tests.

Record for EACH container: actual pill/test-piece L/W/T, servo model and 5 V rating, bracket dimensions, horn index, closed/open pulse, beam high/low levels, shortest valid pulse, max current, load/fill, attempts, count errors, jams and final disposition.
Retain HARDWARE_COMMISSIONED=false in any distributed uncommissioned build.


## Refill LED acceptance
- Verify the revised A0-A3 wiring. None may remain connected to a phototransistor node.
- Check each LED and resistor separately, current below 5 mA and visibility with the cover lifted. Confirm the physical number matches the software medicine label.
- Select 1, 2, 3, 4 and switch repeatedly: exactly one LED is lit at a time. The LCD and software must report the device-confirmed selection.
- Select while the cover is already open; guided refill must work without enabling motor controls.
- Close the cover; after one second all LEDs must extinguish. Test Finish refill while closed and rejection while open.
- Unplug USB during selection, close the app, or stop heartbeats while the serial link remains open: LEDs must clear within five seconds. The open cover must continue to inhibit motion.
- Assert STOP or another fault: every LED off. Reject selection during motor movement/fault and reject invalid container values.
- Cycle controller power: no LED or selection is restored. Verify pill counting still works on D0/D1/D8/D12.
