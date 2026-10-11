# Project audit — 9 October 2026

Scope: the current four-container UNO R4 budget revision with wired computer control, LCD, buzzer and four selected-container refill LEDs. Protocol remains 4. This is a source/build/host-test audit, not physical validation or a claim of zero remaining defects.

## Defects corrected
1. Startup could claim a due schedule as missed before automatic enabling settled. Scheduling and manual start now wait for startup checks.
2. A schedule due while another motor job was running could start later in the same minute. Busy periods are remembered without moving-time flash writes and claimed as missed afterwards. Completion and refill-exit boundaries also capture due periods.
3. Old idle/refill beam pulses could cause a false unexpected-outlet fault in a new job. New jobs establish a fresh pulse boundary; pulses during an active job remain faults.
4. Stop/cover checks before a storage write could be stale at actual servo attachment. Attachment now rechecks both inputs.
5. An unrelated missed period could replace interrupted-dose evidence in the saved result. Fault/active results are preserved while missed day claims advance.
6. A settings acknowledgement cleared pending state before full readback, leaving manual/refill actions able to use stale data. Upload now invalidates cached configuration and blocks these actions until readback.
7. A board boot on an already-open port retained prior identity/settings. Boot now triggers reidentification and configuration retrieval.
8. Disconnect retained the prior journal sequence in desktop logging. It now resets, so an equal sequence from a later connection is not suppressed.
9. The transistor-driven passive piezo circuit omitted its discharge path. Added a separate 1 kohm resistor across the piezo terminals, following Murata's external-drive circuit guidance. This is a documentation/circuit correction; sound output still requires a physical test. Added allowance: INR 2.

## Documentation and procurement corrections
- Cost arithmetic is INR 5,594 electronics; INR 7,094–7,594 with delivery and reserve. Printing, mechanics, laptop, tools and labour are excluded.
- Wire allowance explicitly includes a matching motor-supply input socket. A 5.5 x 2.5 mm motor plug must not be confused with the UNO's 5.5 x 2.1 mm barrel plug.
- A0–A3 are refill LED outputs only; raw phototransistor nodes belong at test pads. One 1 kohm resistor is required per LED.
- Verify purchased LCD backpack address; 0x27 is only the default in config.h. The example supplier also sells 0x3F backpacks.
- Readback, rather than acknowledgement alone, is now required before calling settings verified.

## Evidence
The actual sketch passes host state tests; journal tests model all truncated completion-write prefixes; beam tests cover filtering/rollover; ten desktop/model tests pass with seven real Tk widget tests. Operating/reset builds link for both UNO R4 variants. Separate commissioned copies compile, while the shipped factory flag remains false. Eight candidate meshes pass geometric closure/orientation/volume checks. See VALIDATION.md for sizes and limits.

## Unresolved physical issues
SG90 fit and torque are unproven against the original TGY-9025MG-labelled housing. Original/adapted rotors are not proven to singulate pills or remain closed without power. New cover/port positions and mounts are example geometry. All four optics require measured alignment, pulse timing and double-feed testing. The motor supply, fuses and exact DC-rated stop require measured qualification. No tray sensor means accumulated doses can mix. RTC battery does not supply controller or motors. No real-pill, contact-material, lifetime or assembled-fit validation has been performed.

Use inert objects for commissioning. Do not treat this software audit as authorization to use the prototype for unsupervised medication delivery.
