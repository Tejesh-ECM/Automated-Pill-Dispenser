# LED-guided refill — 9 October 2026

## Operation
1. Connect USB and open the matching budget software (protocol 4).
2. Wait for IDLE and read the dispenser settings. The refill dropdown shows saved container numbers and medicine names.
3. Select the required container and click **Light selected container**. Wait for the software to confirm the actual device selection.
4. Only that container's LED lights; the other three remain off. The LCD shows the saved medicine label and container number.
5. Open the shared cover. The LEDs sit beside the ports on the fixed plate and become visible. Open only the indicated cap and refill manually.
6. Close its cap. To refill another container, select that container in software; the previous LED turns off before the new one turns on.
7. Close all caps and the shared cover. After one second, all LEDs turn off and the device returns idle. **Finish refill** also ends the selection with the cover closed.
8. A selected LED means **this is the container you chose**. It does not mean the machine detected a completed refill or verified the pills.

A due schedule is skipped during REFILL. The cover interlock remains active. Opening it during dispensing still faults the device.
On app disconnection, the app sends a light-cancel command if possible. Otherwise USB loss or a missing heartbeat clears selection within five seconds. No LEDs restore at boot. Offline manual refill remains possible without LED guidance.

## Additional materials
- Four diffused, efficient 5 mm green indicator LEDs, allowance INR 20 total.
- Four additional 1 kohm resistors, allowance INR 8 total.
- Wire from the existing allowance; four printed mounting tabs if needed.

Added LED electronics allowance: **INR 28**. Current electronics subtotal, including the separate INR 2 piezo discharge-resistor correction: **INR 5,594**, excluding delivery, spares and printing.

## Wiring change — do this before loading the new sketch
Container LEDs 1/2/3/4 use A0/A1/A2/A3 as digital outputs.
Each pin connects through its own 1 kohm resistor to its LED anode; cathodes connect to GND.
**Disconnect the old raw optical sensor wires from A0-A3.** Keep each phototransistor node connected to the correct SN74HC14 input and an insulated measurement test pad.
The actual counted beam inputs remain D0, D1, D8 and D12. Pill-counting function is retained.
The 1 kohm resistors limit LED current below 5 mA on a 5 V rail, within the board's 8 mA-per-pin specification. [Official Arduino pinout](https://docs.arduino.cc/resources/pinouts/ABX00080-full-pinout.pdf)

## Mechanical change
Use the refill_led_tab_print_four STL/OpenSCAD template beside the four fixed refill ports.
Label the tabs 1–4 consistently with the software. Keep leads outside pill passages and cap seating surfaces; insulate and strain-relieve them.
The example tab aperture is 5.4 mm square and needs a suitable retaining bezel/fixture or a dimension change for the purchased LED. It is not a verified universal press-fit.
Do not mount a wired LED on a removable cap. The shared cover remains removable and unwired.

## Validation
Firmware/app tests exercise one-at-a-time selection, changing containers, invalid selection, open-cover operation, completion, stop/fault, disconnection and heartbeat expiry.
Actual LED current, brightness, physical mapping and counting integrity still require bench verification.

