# Automated Pill Dispenser

Complete four-container Arduino UNO R4 budget project with a 16x2 LCD, buzzer, optical outlet counters, standalone scheduling and USB-selected refill LEDs.

## Download the complete project

[Automated-Pill-Dispenser-Complete.zip](Automated-Pill-Dispenser-Complete.zip) contains all 130 project files in their folder structure. Download and extract it before opening the firmware or running the computer application. The repository currently distributes the source files inside this archive.

The archive includes:

- `firmware/` — operating firmware, storage recovery sketch and library versions.
- `desktop/` — the Python USB application.
- `mechanical/` — STL, STEP and OpenSCAD files, original references and experimental adaptations.
- `docs/` — build instructions, wiring, material costs, refill guidance, protocol and acceptance tests.
- `tests/` and `validation/` — software tests and recorded verification results.
- `Setup.bat`, `Start.bat`, `requirements.txt` and the complete Word guide.
- The revised ten-paper literature review and component lists.
- `SHA256SUMS.txt` — checksums for the extracted files.

## Read first

- [Complete build guide](SmartPill_Complete_Audited_Guide.docx)
- [Electronics table](Electronic_Components_Table.txt)
- [Literature review in Word](SmartPill_Literature_Review_Revised_Paper_List.docx)
- [Literature review as text](SmartPill_Literature_Review_Revised_Paper_List.md)
- [Validation record](VALIDATION.md)
- [Source attribution](ATTRIBUTION.md)

## Project status

This is a supervised college prototype. The physical dispenser, final CAD fit and servo torque/current requirements remain unverified. Factory firmware disables normal motor operation until the documented commissioning checks are completed. Saved schedules run without the computer while both external power supplies remain powered. The RTC battery preserves time only.

Local schedules, event logs, Python caches and temporary files are excluded. The application creates its local data folder when started. Use the wiring and instructions included in the current archive together.

Original and adapted mechanical materials retain their stated attribution and noncommercial terms; see ATTRIBUTION.md. No broad new licence is applied to third-party material.
