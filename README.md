# Automated Pill Dispenser

The complete project is available as individual files in the folders below. Start with the build guide and wiring instructions before assembling the prototype.

## Project folders

| Folder | Contents |
|---|---|
| [firmware](firmware) | Arduino sketches and required library versions |
| [desktop](desktop) | Python software for USB configuration and control |
| [mechanical](mechanical) | STL, STEP and OpenSCAD files with design notes |
| [docs](docs) | Build instructions, wiring, costs, refill procedure and literature review |
| [tests](tests) | Software tests and simulated hardware helpers |
| [validation](validation) | Recorded build and test results |

## SmartPill USB UNO R4 Budget revision

Updated and audited 9 October 2026. This is the current budget implementation. Start with SmartPill_Complete_Audited_Guide.docx (15 pages), then docs/BUILD_GUIDE.md for editable instructions. Corrections and remaining physical issues are listed in docs/AUDIT.md.

**Status:** firmware/software supplied; physical dispenser, 5 V motor operation and final mechanical fit have not been verified. The factory sketch inhibits normal motor operation until commissioned. See VALIDATION.md for the checks actually run.

## Included
- firmware/SmartPillBudget: standalone scheduler, direct servo control, optical counting, internal storage and 16x2 LCD.
- desktop/app.py: one-window USB settings/control application, protocol 4.
- docs: feature changes, wiring, electronic cost list, build instructions and acceptance tests.
- mechanical/budget_candidates: eight editable STL/OpenSCAD templates for the changed structures.
- mechanical/reference_parts and pill_candidates: original/adapted references, not all required in the simplified assembly.
- tests: storage interruption, firmware state, optical counting and desktop checks.
- firmware/ResetStorage: deliberate empty-bench storage recovery utility.

## Quick start
1. Read docs/BUILD_GUIDE.md and docs/WIRING.md before purchasing or connecting.
2. Install Python 3 with Tcl/Tk; run Setup.bat then Start.bat on Windows.
3. Install Arduino Renesas UNO 1.6.0 and the pinned libraries in firmware/libraries.txt.
4. Select your actual UNO R4 variant. Compile/upload firmware/SmartPillBudget/SmartPillBudget.ino.
5. Calibrate empty mechanics, test inert pieces, then change HARDWARE_COMMISSIONED only after acceptance.
6. Match the desktop app to protocol 4. Earlier applications and wiring do not apply.

Normal scheduled operation does not require USB. Both external supplies must remain powered. The RTC battery powers the clock only.

The fixed receiving tray is not sensed. Empty it before every dose; a later dose will otherwise mix with remaining pills. The device reports DISPENSED, never collection or intake.

Retain ATTRIBUTION.md with the original/adapted mechanical material.


Refill LED update: four indicators controlled from the USB software; exactly one selected at a time. See docs/REFILL_LED_GUIDE.md. Updated wiring moves raw optical nodes off A0-A3, which now drive the four LEDs.

## Repository documents

- [Electronic component table](Electronic_Components_Table.txt)
- [Complete build guide](SmartPill_Complete_Audited_Guide.docx)
- [Literature review in Word](docs/SmartPill_Literature_Review_Revised_Paper_List.docx)
- [Literature review as text](docs/SmartPill_Literature_Review_Revised_Paper_List.md)

This repository contains the current budget revision. Runtime schedules and event logs in `data/` are local and excluded from version control. The computer application creates that directory when it starts.
