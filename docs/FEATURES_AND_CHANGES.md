# Budget revision — 9 October 2026

This revision implements the requested simplifications. Its firmware and desktop app are a new matched protocol-4 pair.

| Feature | Final behaviour |
|---|---|
| Four medicine containers | Retained; one labelled medicine per container |
| Three daily times | Retained; different whole-pill counts 0–10 per container and period |
| USB-independent scheduling | Retained, with both external power supplies on |
| Optical outlet counting | Four IR barriers and conditioned digital edges retained |
| Jam/missing-pill/excess-event handling | Retained; stops and requires inspection |
| Restart protection | Internal page journal records daily claims before motion and unfinished jobs |
| Cup presence/removal | Removed completely; no sensor, prompts, wait states or collection record |
| Output handling | Fixed receiving tray; operator empties it before each dose |
| Refill | Manual labelled ports; shared cover switch inhibits dosing during refill |
| Refill indicator LEDs | USB software selects container 1-4; only its LED lights; others off |
| Guided refill motor | Removed; no automatic alignment or homing |
| LCD | 16x2; clock/next time and short status/progress/error messages |
| Buzzer | Retained; three start beeps, two completion beeps, five fault beeps |
| Servo driver board | Removed; four servos controlled directly by UNO R4 |
| External memory | Removed; eight rotating internal 1 KB pages store complete snapshots |
| History | Latest outcome only survives offline; the app logs observed results while connected |
| Computer interface | One window: labels, quantities, three times, upload/read, clock, enable, stop, clear, refill instructions and confirmed manual dose |
| Patient profiles, strength fields, CSV history UI, graphical progress | Removed; not needed for the budget objective |
| Wiring connections | Soldered, insulated and strain-relieved harness; detachable terminal assortment not required |
| Motor supply | Candidate 5 V / 5 A enclosed supply, conditional on measured 5 V servo performance |
| Controller supply | Separate 9 V / 1 A adapter retained |
| Electrical stop and fuses | Retained |

The fixed tray removes the need for cup sensing but does not detect an uncollected dose. The machine will dispense another scheduled dose into whatever remains there. Empty it before each scheduled time. No record means "collected" or "taken". This is a supervised prototype, not an unattended medication-adherence system.

Detailed history is deliberately reduced. An offline sequence of events may be overwritten by the latest result. Essential daily claims and interrupted-operation state remain separate from the displayed result within every snapshot.

Mechanical design changes are supplied as editable STL/OpenSCAD templates. They are not verified replacements for the assembled original enclosure. Do not print four complete mechanisms before the single-mechanism trial.


## LED-guided refill
Connect the matching protocol-4 app, read the saved medicine labels, choose a container and click Light selected container. Wait for device confirmation; open the cover and fill only the illuminated labelled port. Select another container to move the light. Closing the cover for one second or Finish refill (cover closed) turns every LED off. A fault or USB heartbeat loss also turns them off. No selection is remembered after power loss. Unguided manual refill remains possible offline.
