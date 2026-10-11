# Budget mechanical structure

Run python mechanical/build_budget_parts.py to regenerate budget_candidates/*.stl and *.scad. All units are mm.
These eight files are dimensional TEMPLATES with closed, consistently oriented meshes; physical fit has not been verified.

| Candidate | Intended quantity | Required adjustment |
|---|---:|---|
| manual_refill_plate | 1 | Replace example 190x150 plate and inlet centres with measured enclosure/inlet coordinates |
| refill_cap_print_four | 4 | Match each port; starting plug clearance 0.4 mm per side; add external labels 1–4 |
| shared_refill_guard | 1 | Cover all caps; add captive fastener/strap retention and switch actuator after physical measurement |
| lcd_16x2_plate | 1 | Confirm actual LCD window, holes, PCB depth and original panel coverage |
| fixed_receiving_tray | 1 | Confirm chute alignment and fasten to base; add a separate cleanable liner |
| controller_plate | 1 | Confirm board holes, standoffs, cable clearances and electronics attachment |
| refill_led_tab_print_four | 4 | 5.4 mm square aperture; adapt retention/bezel to actual LED; attach beside the matching refill port |
| cover_switch_bracket | 1 | Match purchased microswitch/actuator; closure only with cover fitted |

The refill plate example inlet positions (50,45), (140,45), (50,105), (140,105) are NOT assembly-derived coordinates. Do not print this as a guaranteed replacement.
A direct refill passage must reach only its labelled bowl, with no gap that spills into a neighbour. Modify/line the passage as required by the measured assembly.
The original drum is not part of the budget assembly. Do not reuse its home-position assumptions.
The tray is fixed during operation. Cleaning uses a separate removable liner. No part detects its presence, emptiness, contents or collection.
Keep the cover switch inaccessible to ordinary pill movement. Opening the guard must release its contact. Do not bypass it to save a bracket.

reference_parts/ preserves original extracted CAD and its manifest for container/rotor/housing/base reference. Parts are in local coordinates; the manifest is not a budget purchase/print list. Display holder, drum and original top arrangement are reference only and require the new manual-access/panel design.
Original housings model TGY-9025MG; SG90 dimensions, shaft position and horn need fit checks. The original base may exceed a 220 mm printer bed; use a suitable printer or engineer proper joints instead of scaling.

pill_candidates/ retains the four inert test profiles and experimental rotors:
C1 round 6x6x3; C2 round 10x10x4; C3 oblong 16x7x5; C4 capsule-shaped 21.7x7.6x7.6.
These are selected test envelopes, not measured medicines or a proven list of the most commonly used pill sizes.
The original 2 mm rotor thickness with a profile-shaped opening is not a proven single-pill pocket. Test double feeding, stack height, wiper geometry, closure and actual pill tolerances.
Do not confuse printed dummy pieces with medicines. No print is qualified for medication contact by mesh checks alone.

Original/adapted reference CAD retains MakersUPV attribution and CC BY-NC 4.0 restrictions. See ../ATTRIBUTION.md.


Mount each LED on a fixed tab beside its port and label it 1-4. LEDs become visible when the shared cover is lifted; the software/LCD confirms the selection beforehand. Keep tabs and insulated leads outside the pill passage and clear of cap seating. The new tab is a mounting template, not a validated snap-fit for every 5 mm LED.
