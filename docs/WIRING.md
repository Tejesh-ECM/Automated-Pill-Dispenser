# Budget wiring — protocol 4 firmware

Use this wiring only with SmartPillBudget. The factory sketch has HARDWARE_COMMISSIONED=false.

| UNO R4 connection | Destination |
|---|---|
| USB-C | Computer data cable for setup/control |
| Barrel jack | Regulated 9 V, centre-positive 5.5 x 2.1 mm adapter |
| 5 V | LCD logic/backlight, DS3231, SN74HC14, optical emitters and buzzer circuit |
| A4 / SDA | LCD SDA and RTC SDA |
| A5 / SCL | LCD SCL and RTC SCL |
| D2, D3, D4, D5 | Servo 1, 2, 3, 4 signal respectively |
| D0, D1, D8, D12 | Conditioned beam signals 1–4 |
| A0, A1, A2, A3 | Refill indicator LEDs for containers 1, 2, 3, 4 respectively, through individual 1 kohm resistors |
| D7 | Shared cover interlock; switch closes to GND ONLY when cover is fitted |
| D9 | Stop sense; normally closed stop contact to GND, opens on stop |
| D10 | Buzzer transistor base through 1 kohm |
| GND | Common logic and motor-supply negative, joined at distribution point |

D0/D1 are optical inputs: do not use Serial1. USB uses Serial. Verify all four interrupt channels on the actual R4 variant.

## Four optical channels
For each emitter: logic 5 V -> initially 220 ohm resistor -> IR LED anode; cathode -> GND. Confirm polarity and safe current from the purchased emitter specification.
Phototransistor emitter -> GND; collector -> raw node. A 10 kohm resistor connects that node to 5 V.
Route each raw node only to its SN74HC14 input and an insulated test pad for a meter/scope. Do NOT connect raw nodes to A0..A3: those pins now drive refill LEDs.
SN74HC14 DIP14: VCC pin 14, GND pin 7, 100 nF directly across them.
Use gate pairs (input -> output): 1->2, 3->4, 5->6, 9->8.
Outputs go to D0, D1, D8, D12 respectively. Tie unused inputs pins 11 and 13 to GND; leave outputs 10 and 12 unconnected.
With this circuit, a correctly aligned illuminated beam reads HIGH at the UNO; blocked reads LOW. Verify before enabling motors.

## Servo power and signal
Each servo red -> its fused MOTOR positive branch; brown/black -> motor GND; orange/yellow -> corresponding D2..D5.
Add one 10 kohm signal-to-GND pull-down per servo at the harness.
Never power the servos through an UNO pin, regulator or solderless breadboard.
Only one servo is attached at a time. The firmware closes that mechanism, waits 500 ms, and detaches it before moving to the next container.
The mechanism MUST remain closed without holding torque; add mechanical restraint or revise the gate if it does not.
Direct Servo.attach on the Renesas library starts a timer; the sketch sets the intended pulse with interrupts temporarily masked. Verify actual startup pulse width and absence of twitch using the purchased servos.

## Motor supply and stop
Enclosed regulated 5 V / 5 A supply -> main fuse -> adequately DC-rated normally closed STOP power contact -> four individually fused positive branches.
The stop has a separate NC contact for D9. The power contact is a hard cut, independent of software.
Starting allowances are a 5 A main time-delay fuse and 2 A branch time-delay fuses. These are NOT finalized ratings: choose from measured starting/stall currents and the weakest wire, connector and contact ratings.
Keep the 1000 uF >=16 V capacitor on the SUPPLY side of the stop contact, close to distribution, so stored energy does not bypass the stop's intended motor-power interruption. Confirm noise/drop behaviour in tests.
Use short suitable copper wiring (starting sizes: 18 AWG trunk, 22 AWG motor branches, 24–26 AWG signals), star returns, soldered insulated joints and strain relief. Confirm temperature rise and voltage drop.
The 5 V motor rail does not connect to the UNO 5 V pin. Only grounds are shared. This also avoids feeding the computer USB supply from the motor adapter.
Do not connect the 5 V motor adapter to the UNO barrel jack: the separate 9 V adapter powers logic while USB is absent.

## Cover switch
Use the switch contact that is CLOSED when the shared cover is fully installed and OPEN when removed. A broken wire must read as open. Contact labels NO/NC refer to the unactuated switch, so select by measured cover position rather than label alone.
D7 uses INPUT_PULLUP and one second of stable closed state. Open cover during motion latches a fault. Open cover while idle enters refill; close it to finish. The switch is a logic interlock, not the motor-power stop.
The bracket and actuator need physical adjustment before printing the final cover.

## Buzzer
Piezo positive -> logic 5 V, negative -> PN2222A collector.
Add a 1 kohm discharge resistor directly across the piezo terminals (logic 5 V to collector). This returns the collector high when the transistor switches off; omitting it can leave charge on the capacitive piezo and prevent reliable sound. This is separate from the base resistor. See [Murata drive-circuit guidance](https://www.murata.com/en-global/support/faqs/sound/diaphragm/char/sch0001).
Emitter -> GND. D10 -> 1 kohm -> base. Base -> 10 kohm -> GND.
Verify the transistor's actual pin order. Use the specified passive piezo; changing to an inductive buzzer requires an appropriate driver/flyback design.

## RTC
Use a DS3231 breakout without a coin-cell charging circuit and a CR2032. The battery keeps time only; it cannot run motors or the controller.
Scan/verify addresses: LCD typically 0x27 (edit config.h if different), RTC 0x68. Check 5 V compatibility and SDA/SCL pull-ups.


## Refill indicator LEDs
Use four efficient diffused 5 mm green indicator LEDs, one beside each numbered refill port.
A0 -> 1 kohm resistor -> LED 1 anode; LED 1 cathode -> GND.
Repeat A1/LED 2, A2/LED 3, A3/LED 4, each with its OWN resistor.
Typical current is about 2-3 mA; even the 5 V / 1 kohm upper bound is 5 mA. Do not use 220 ohm here.
UNO R4 GPIO maximum is 8 mA per pin: [Arduino pinout](https://docs.arduino.cc/resources/pinouts/ABX00080-full-pinout.pdf).
LED anode is normally the longer lead; verify polarity if leads have been trimmed. A0..A3 are configured as digital outputs, not ADC/DAC channels.
Attach LEDs to the fixed plate/tabs beside the ports, outside pill passages. Insulate leads and route wires away from caps and rotors. Do not attach wiring to a removable cap.
All LEDs are off at boot, fault, refill completion or cancellation. Exactly one is on after a valid connected-software selection. Switching selection extinguishes all outputs before enabling the new one.
Unplugging USB/closing the app extinguishes them immediately when link loss is visible, otherwise within the 5-second heartbeat timeout. The cover interlock continues to inhibit dispensing while open.
The LED is a selection guide only. It does not sense pill level, identity, correct filling or refill completion.
