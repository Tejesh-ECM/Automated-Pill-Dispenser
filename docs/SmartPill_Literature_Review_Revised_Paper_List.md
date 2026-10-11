Literature Review of a Low Cost Smart Pill Dispenser

Ten recent papers explained at first-year college level

Publication period: 2022–2026  |  Literature checked: 10 October 2026

### Introduction

A smart pill dispenser is a device that makes selected medicine available at planned times. The idea sounds simple, but building one involves several different tasks: storing pills, keeping time, moving the mechanism, checking the output and giving the user clear instructions. Research in this area therefore includes both engineering tests and studies of how people use the devices.

This review looks at ten recent papers that are relevant to our project. It explains what each paper studied, what the researchers found, the limits of the evidence and what we can learn from it. The aim is to understand useful design choices, rather than copy a complicated system or assume that a published device is suitable for every user.

### Connection to our project

Our proposed build has four containers, an Arduino UNO R4, a real-time clock, a 16×2 LCD, a buzzer, optical outlet sensors and four refill LEDs. Settings are entered through simple USB-connected computer software. Saved schedules should continue when the computer is disconnected, provided the external supplies remain powered. During guided refill, software selects one container and only its LED lights.

The fixed receiving tray has no collection sensor. Therefore, our project can report dispensing activity but cannot confirm that a person collected or swallowed the medicine. The current design is a supervised prototype that still needs physical testing.

### How the papers were selected

The ten papers follow the supplied reading list, with titles, years and publication details checked against publisher records and research archives. The correct period is 2022–2026 because paper 10 was published in May 2026. Nine report original work and one is a systematic review. This is a focused narrative review, not an exhaustive systematic review.

Publisher text and open full text were used where available. Papers 4 and 5 are limited to accessible IEEE abstracts and verified metadata. Their full texts were not verified. The original Hackster build is project background and is not counted as one of the ten research papers.

### A few useful terms

Medication adherence means following the agreed medicine schedule. Usability means how easily people can use a device. An RTC is a clock circuit that keeps time. A prototype is an early working model. A feasibility study checks whether an idea is practical; it does not, by itself, prove long-term medical benefit.

## Review of the selected papers

### 1 Making the device easy to use

Gargioni, Fogli and Baroni (2024) — Systematic review of 56 papers.

This review examined 56 papers from the viewpoint of the people who use and manage dispensers. It covered hardware, software, interfaces, medicine loading and the roles of patients and caregivers. This is useful because a dispenser must be understandable during setup and refilling, not only when it releases a pill.

The reviewed systems differ, so their findings do not validate one particular design. For our project, the review supports paying attention to numbered containers, readable labels and a clear refill sequence. We should ask someone unfamiliar with the device to select and refill a container, then record any confusion or wrong selection.

[Source 1](https://doi.org/10.1007/s41666-024-00161-w)

### 2 An Arduino dispenser with printed rotary feeders

Krzysztoszek et al. (2025) — Engineering design and prototype testing.

The authors built two devices: a programmer that writes dosing information to an RFID card, and a dispenser that reads it. The dispenser uses an Arduino Mega, three stepper motors, printed rotary feeders, an RTC and an LCD. The programmer accepts settings through a USB serial interface. This makes the paper especially relevant to our mechanical and wired-software design.

The authors report several hundred operating attempts without failure, but also describe occasional false empty-container readings when pills sit at an angle. The test conditions therefore matter. Our UNO R4 project can learn from the modular feeders and separate setup process, while its four-container mechanism still needs its own repeated tests with different pill shapes.

[Source 2](https://doi.org/10.24136/jaeee.2025.008)

### 3 A printed dispenser with sliding pushers

Martinez Baquero et al. (2025) — CAD design and functional prototype.

This prototype has four printed storage columns, an ESP32 and servo-driven sliding pushers. A gear and straight toothed bar convert motor rotation into linear movement. An infrared sensor checks the pill drop, and a phone application selects the dispensing column. The paper describes up to seven pills per column and an intended maximum pill width of 25 mm and thickness of 5 mm.

The mechanism is useful for comparison, but it differs from our circular-disc design. The conclusion lists automatic routines at preset times and pill counting as future work, so we should not describe full scheduling as already demonstrated. For our build, the lesson is to confirm the drop before starting another container and test the chosen dimensions ourselves.

[Source 3](https://doi.org/10.18687/LACCEI2025.1.1.496)

## Scheduling sensing and system limitations

### 4 Standalone scheduling and laboratory failures

Patricia et al. (2025) — Laboratory prototype study reviewed from its abstract.

The accessible IEEE abstract describes an Arduino dispenser with an RTC, servo motors, buzzer, LCD and buttons. It operates without an internet connection. The authors report an initial 10-cycle test followed by a 30-cycle stress test, with 80% overall dispensing success. Reported timing deviations averaged +1.75 seconds and reached +2 seconds.

The hardware closely matches several parts of our project. However, accurate timing does not make an unsuccessful dispensing attempt acceptable. The abstract does not provide enough evidence to establish clinical safety, and the full test details were not available for this review. Our evaluation should record timing error, jams and correct pill counts separately, including operation after disconnecting USB.

[Source 4](https://doi.org/10.1109/ICORIS67789.2025.11295980)

### 5 Detecting a drop and a pickup

Seckin et al. (2024) — Conference prototype and short home feasibility study.

MedDock combines a 3D-printed dispensing attachment, servo motors and a textile force sensor. The accessible IEEE abstract describes detecting both pill drops and pickups. It reports a three-day home feasibility study with eight healthy participants aged 54–85. This is a small, short study, so it cannot establish long-term medication adherence.

The main lesson is that dispensing and pickup are separate events. Our optical sensors check outlet activity, while the fixed tray has no pickup detection. We must therefore keep our result limited to dispensing. This summary uses the accessible abstract and verified publication metadata; detailed full-text performance claims are not included.

[Source 5](https://doi.org/10.1109/BSN63547.2024.10780516)

### 6 Learning from connectivity and power problems

Dayananda and Upadhya (2024) — System development and functional testing.

Dayananda and Upadhya developed SPEC 2.0, a dispenser managed through an Android application with notifications and medicine records. The paper discusses eight functional test cases and reports about 90% overall accuracy and functionality. It also identifies problems with the physical structure, network delays and loss of operation during a power cut.

The combined percentage is not a clearly isolated measure of one-pill dispensing accuracy. The authors also acknowledge that their device cannot confirm swallowing. For our project, the useful lesson is to keep the saved schedule on the controller and test the structure carefully. The RTC battery keeps time; it does not supply power to the motors.

[Source 6](https://doi.org/10.1007/s40031-023-00956-2)

## User actions and home use

### 7 Separating dispensing from later user actions

Peddisetti et al. (2024) — Integrated prototype with controlled scenario testing.

This system combines an Arduino Nano RP2040 dispenser, a sensor-equipped cup and an Android application. Bluetooth links the devices. Cup movement and distance measurements are used to infer medicine-taking activity. The paper discusses motor performance and tests involving a reset after dispensing, as well as an attempt to request the same scheduled dose again.

These scenarios are useful for our software testing, particularly preventing a restart from triggering a duplicate dose. The evaluation used controlled conditions, and cup movement is indirect evidence of swallowing. Our budget design has a fixed tray without collection sensing, so it must report dispensing events only. The study does not require us to add a smart cup.

[Source 7](https://doi.org/10.1109/AIMHC59811.2024.00032)

### 8 Using a dispenser at home

Patel et al. (2022) — Six-month observational pilot study.

Patel and colleagues studied a home dispenser with 58 recruited participants over six months. The system used prepared medication pouches, reminders and remote monitoring. The reported average adherence was 98%, based on removing a dose within two hours of its scheduled time. Users also gave favourable ease-of-use ratings.

However, this was an observational study without a comparison group, and removing a pouch did not directly prove swallowing. For our project, the useful lesson is to test daily use as well as the mechanism. We should report “dispensed” accurately and avoid treating it as confirmation that medicine was taken.

[Source 8](https://doi.org/10.2196/34906)

## Visual guidance and behaviour prediction

### 9 Timing and visual guidance

Karagiannis et al. (2022) — Prototype and 14-participant comparison.

This team built a portable pillbox with a real-time clock, buzzer, display and individually controlled lights. Fourteen participants compared the working prototype with a dummy pillbox that received schedule changes through text messages. The prototype reduced intake delays, but this small study did not clearly show a reduction in wrong or missed pills.

The result supports studying clear timing and visual instructions, rather than assuming that more electronics always prevent mistakes. Our RTC and buzzer serve a similar timing purpose. We adapt the idea of selecting a location with light to refill guidance, although this paper did not test our refill procedure or our motor-driven dispenser.

[Source 9](https://doi.org/10.3390/s22155818)

### 10 Interpreting prediction accuracy carefully

Ridwan et al. (2026) — Laboratory data and machine-learning prototype.

Ridwan and colleagues combined an ESP32, RTC, weight sensor and button with Naive Bayes, a probability-based classification method. They generated 100 laboratory interaction records, trained the model on 80 and tested it on 20. It classified 16 test records correctly, giving 80% accuracy. This measures behaviour classification, not mechanical dispensing accuracy or confirmed medicine intake.

Sixteen of the 20 test records belonged to the same category. Our interpretation is that always choosing that category would also score 80%, so the headline figure alone is weak evidence of useful prediction. Real-patient validation is still needed. For our project, simple, accurate event records are more useful at this stage than adding a prediction algorithm.

[Source 10](https://doi.org/10.35314/ercsa238)

## Comparison of the ten papers

This table highlights the main lesson for our build. It is not a ranking: a product review, a short prototype test and a home study answer different questions.

| Paper | Evidence | Main lesson for our project |
|---|---|---|
| [1] Gargioni, 2024 | Systematic review | Evaluate setup and refill as well as dispensing. |
| [2] Krzysztoszek, 2025 | Rotary feeder prototype | Check false empty readings and pill orientation. |
| [3] Martinez Baquero, 2025 | Sliding pusher prototype | Verify each drop; test scheduled operation separately. |
| [4] Patricia, 2025 | Laboratory study* | Separate timing accuracy from dispensing success. |
| [5] Seckin, 2024 | Home feasibility study* | Distinguish a pill drop from its later pickup. |
| [6] Dayananda, 2024 | Functional system tests | Test power loss and remove scheduling dependencies. |
| [7] Peddisetti, 2024 | Controlled scenarios | Prevent repeat dispensing after a reset. |
| [8] Patel, 2022 | Home pilot study | Dose removal does not directly confirm swallowing. |
| [9] Karagiannis, 2022 | Pillbox comparison | Use clear timing and visual location cues. |
| [10] Ridwan, 2026 | Laboratory classifier | Do not confuse prediction accuracy with pill accuracy. |

*Papers 4 and 5 were reviewed from accessible abstracts; their detailed full-text methods were not verified.

### Why the results cannot be combined into one percentage

The papers measure different things. A correct mechanical output, removal of a medicine pouch, a user’s opinion and a correct software prediction are different outcomes. Adding these figures together would give a misleading result. For our demonstration, the first useful measurement is whether the device releases exactly the requested number of test pieces, followed by how easily someone can set it up and refill it.

## What the literature means for our design

### Keep the everyday interaction clear

The user-focused review shows why the interface deserves attention [1]. Our design response is a small number of clear actions: read settings, save settings, set the clock, select a refill container and inspect a fault. Container numbers should match on the software, LCD and physical labels. This is our design choice, not a finding that an LCD is always better than another interface.

### Treat refill guidance as a question to test

The portable pillbox uses visual guidance [9], but this does not establish the effectiveness of our exact refill LED arrangement. A practical question for our project is: can a new user select and refill the correct container without help? We can record wrong selections, time taken and requests for assistance. The light should mean “selected container,” not “refill confirmed,” because it does not sense added medicine.

### Test failure conditions as carefully as normal operation

The dispensing trials and restart scenarios provide useful examples for our evaluation [4, 7]. Our acceptance tests should include different piece shapes, low and full containers, blocked outlets, extra sensor events, restarts and USB disconnection. Each attempt should record the requested count, actual count, any fault and what happened after restarting. Software tests alone cannot establish the reliability of the assembled mechanism.

### Define the practical gap honestly

The opportunity for our team is to build and evaluate a simple, affordable four-container system with local scheduling and a guided manual refill process. The research does not establish that this combination has never been attempted. Our contribution should therefore be described as a focused implementation and evaluation, rather than a claim to have invented the first smart dispenser.

We also accept a clear trade-off: a simple wired interface reduces the setup we must develop, but it does not provide remote supervision. The tray must be emptied by the operator. The RTC battery preserves time during a power cut; it does not power the motors. These limits should remain visible in the project report.

### Conclusion

The ten papers show that a useful dispenser needs more than a timer and a motor. It also needs clear instructions, dependable handling and honest records of what happened. For our project, the best next step is to prove one dispensing mechanism, then integrate all four and test the complete refill and scheduling routine. We should claim reliable dispensing only after physical measurements support it, and avoid claiming improved patient adherence from a college demonstration alone.

## References

Numbered references correspond to the paper summaries and comparison table. DOI links lead to the publication records. Full titles are retained so the papers can be found easily. Author order follows the publisher’s citation record, except paper 10, which follows its PDF title page.

[1] L. Gargioni, D. Fogli and P. Baroni (2024). A Systematic Review on Pill and Medication Dispensers from a Human-Centered Perspective. Journal of Healthcare Informatics Research, 8, 244–285.

DOI: [https://doi.org/10.1007/s41666-024-00161-w](https://doi.org/10.1007/s41666-024-00161-w)

[2] K. Krzysztoszek, D. Kusak, M. Luft and D. Pietruszczak (2025). Automatic Pill Dispenser Supporting Correct Medication Dosing. Journal of Automation, Electronics and Electrical Engineering, 7(2), 27–40.

DOI: [https://doi.org/10.24136/jaeee.2025.008](https://doi.org/10.24136/jaeee.2025.008)

[3] J. E. Martinez Baquero, R. Jimenez Moreno and J. M. Bejarano Restrepo (2025). Automatic Medication Dispensing Prototype. 23rd LACCEI International Multi-Conference for Engineering, Education, and Technology, contribution 496.

DOI: [https://doi.org/10.18687/LACCEI2025.1.1.496](https://doi.org/10.18687/LACCEI2025.1.1.496)

[4] E. Patricia, E. R. Husen, S. Hermawan and M. E. Habiby (2025). Automatic Pill Dispenser to Improve Medication Adherence in Elderly and Chronic Disease Patients. 2025 7th International Conference on Cybernetics and Intelligent System (ICORIS), pp. 1–6.

DOI: [https://doi.org/10.1109/ICORIS67789.2025.11295980](https://doi.org/10.1109/ICORIS67789.2025.11295980)

[5] M. Seckin et al. (2024). MedDock: A 3D-Printed Smart Pill Dispenser with Sensitive Textile Sensor for Adherence Monitoring. 2024 IEEE 20th International Conference on Body Sensor Networks (BSN), pp. 1–4.

DOI: [https://doi.org/10.1109/BSN63547.2024.10780516](https://doi.org/10.1109/BSN63547.2024.10780516)

[6] P. Dayananda and A. G. Upadhya (2024). Development of Smart Pill Expert System Based on IoT. Journal of The Institution of Engineers (India): Series B, 105(3), 457–467.

DOI: [https://doi.org/10.1007/s40031-023-00956-2](https://doi.org/10.1007/s40031-023-00956-2)

[7] V. Peddisetti et al. (2024). Smart Medication Management: Enhancing Medication Adherence with an IoT-Based Pill Dispenser and Smart Cup. 2024 IEEE First International Conference on Artificial Intelligence for Medicine, Health and Care (AIMHC), pp. 137–144.

DOI: [https://doi.org/10.1109/AIMHC59811.2024.00032](https://doi.org/10.1109/AIMHC59811.2024.00032)

[8] T. Patel, J. Ivo, T. Pitre, S. Faisal, K. Antunes and K. Oda (2022). An In-Home Medication Dispensing System to Support Medication Adherence for Patients With Chronic Conditions in the Community Setting: Prospective Observational Pilot Study. JMIR Formative Research, 6(5), e34906.

DOI: [https://doi.org/10.2196/34906](https://doi.org/10.2196/34906)

[9] D. Karagiannis, K. Mitsis and K. S. Nikita (2022). Development of a Low-Power IoMT Portable Pillbox for Medication Adherence Improvement and Remote Treatment Adjustment. Sensors, 22(15), 5818.

DOI: [https://doi.org/10.3390/s22155818](https://doi.org/10.3390/s22155818)

[10] A. Ridwan, R. U. P. Pasaribu, S. J. Sembiring, S. L. Sihombing and D. Lei (2026). Smart Pill Dispenser with Naive Bayes Algorithm for Predicting Medication Compliance Based on Patient Behavior Patterns. INOVTEK Polbeng – Seri Informatika, 11(2), 639–648.

DOI: [https://doi.org/10.35314/ercsa238](https://doi.org/10.35314/ercsa238)
