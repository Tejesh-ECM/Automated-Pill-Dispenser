# Budget revision verification — 9 October 2026

This report covers SmartPillBudget protocol 4 only. No physical board or dispenser was connected.

## Builds
Operating and storage-reset sketches compile and link for UNO R4 Minima and UNO R4 WiFi using official Arduino Renesas UNO core 1.6.0, its ARM GCC 7-2017q4 compiler and the dependencies pinned in firmware/libraries.txt.
The local Windows Arduino CLI discovery issue from the previous revision remains; the official platform compile/link recipe was invoked directly. This is not a claim of a successful Arduino CLI invocation.

| Board | Sketch | .text bytes | .data + .bss bytes |
|---|---|---:|---:|
| Minima | SmartPillBudget | 91132 | 8296 |
| UNO R4 WiFi | SmartPillBudget | 94644 | 11828 |
| Minima | ResetStorage | 55208 | 5672 |
| UNO R4 WiFi | ResetStorage | 67668 | 9560 |

These are section sizes, not maximum runtime RAM use. Each link also reserves an 8192-byte heap and 1024-byte stack plus vector/noinit areas. Full section reports are in validation/.
The factory setting HARDWARE_COMMISSIONED=false remains unchanged.
Internal data-flash page size 1024 and total size 8192 were checked against the installed core variant definitions and EEPROM/BlockDevices implementation.

## Checks actually passed
- C++ tests of actual journal.h: empty memory; rotated-page reload; every truncated snapshot-program prefix during a completion write after page erase; retention of the previous active marker; successful completion; torn-page repair; forward/backward/date claims; failed writes; and invalid schedule rejection.
- C++ tests include the actual SmartPillBudget.ino with host hardware fakes: autonomous start without USB; no same-date repeat; sensorless completion directly to idle; manual USB timeout; cover and stop faults; manual refill transition; refill-time missed schedule; jam timeout; excess pulses; closing-phase extra pulse; no motion after storage failure; schedule validation; explicit clear consuming already-due periods.
- Actual beam_counter.h tests: minimum pulse filtering, complete pulses, duplicate edges, microsecond rollover, pending-count saturation.
- Ten Python tests passed, including seven real Tcl/Tk widget tests using installed Python 3.14 and Tcl/Tk 8.6.15: configuration roundtrip, invalid input, protocol matching/rejection, no automatic command retry after USB write failure, connected refill selection, device-confirmed LED status, pending-upload readback gating and firmware reidentification after reset.
- Refill LED tests on the actual operating sketch with host fakes passed: exactly one output high, switching 1–4, invalid/missing selections, disconnected rejection, cover-open operation, finish gating, cover-close completion, heartbeat expiry, USB loss, cancellation, stop/fault clearing and rejection during dispensing.
- Eight generated candidate STL files pass two-triangles-per-edge, consistent orientation and positive volume checks matching their box geometry. This does not prove assembled fit.
- Operating/reset journal.h copies match.
- Package SHA256SUMS and ZIP integrity checked during packaging.

## Limits and required physical tests
The 9 October audit added regressions for startup schedule gating; skipping a schedule observed during another job or at refill exit within the same minute; stale idle pulses; interrupted-result preservation; and interlock rechecking at servo attachment. All passed. See docs/AUDIT.md.

Separate copied sketches with HARDWARE_COMMISSIONED=true also compiled/linked for both boards. Minima .text 91132, .data+.bss 8296; WiFi .text 94668, .data+.bss 11828. These are compile checks only; distributed config.h remains false.

Host EEPROM tests model independent page erase plus truncated programming, not analogue brownouts or wear. Host servo/timer/RTC fakes cannot validate real interrupt timing, servo twitch, stopped motor power, current draw, count reliability or watchdog brownout behaviour.
No claim is made of real-pill validation, food/medication contact suitability, calibrated mounts or a final assembled CAD fit.
The candidate 5 V motor supply needs measured load/torque qualification. The DC-rated stop contact still needs exact part selection.
The tray has no sensor. It cannot prevent dose mixing if an earlier dose remains.
See docs/ACCEPTANCE_TESTS.md for the required physical campaign.

## Reproduce
From the extracted project folder, with appropriate tools installed:
```text
python -m unittest discover -s tests -v
g++ -std=c++17 -I tests tests/test_journal.cpp -o test_journal.exe
test_journal.exe
g++ -std=c++17 tests/test_beam.cpp -o test_beam.exe
test_beam.exe
g++ -std=c++17 -I tests/fakes -I tests -I PATH_TO_ArduinoJson/src tests/test_firmware.cpp -o test_firmware.exe
test_firmware.exe
arduino-cli compile --fqbn arduino:renesas_uno:minima firmware/SmartPillBudget
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi firmware/SmartPillBudget
arduino-cli compile --fqbn arduino:renesas_uno:minima firmware/ResetStorage
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi firmware/ResetStorage
python mechanical/build_budget_parts.py
```
The Arduino CLI commands above are reproduction instructions, not the commands that succeeded on this host.
Keep fake test headers out of Arduino sketch folders. The reset utility destroys prior settings and claims; it is for a deliberately inspected empty bench only.

