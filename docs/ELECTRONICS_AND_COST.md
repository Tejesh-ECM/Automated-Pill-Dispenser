# Electronics and Chennai budget

9 October 2026. Rounded purchasing allowances, not a supplier quotation. Current web spot-checks showed LCD-only INR 126, I2C backpack INR 45 and a 5 V 5 A adapter INR 849; allowances below include selection/pack-price margin. Other rows remain estimates. GST, freight, exact variants and stop ratings need checkout confirmation.

| Item | Qty | Unit INR | Total INR | Use / condition |
|---|---:|---:|---:|---|
| UNO R4 Minima | 1 | 1000 | 1000 | Controller and internal schedule storage |
| 16x2 LCD with I2C backpack | 1 | 250 | 250 | Clock, next time, status and faults |
| DS3231 module, non-charging battery circuit | 1 | 300 | 300 | Independent clock |
| CR2032 | 1 | 40 | 40 | Retains time during power loss |
| 5 V-compatible SG90 positional servo | 4 | 150 | 600 | One per dispensing mechanism; verify torque |
| Matched IR emitter and phototransistor pair | 4 | 30 | 120 | Individual outlet counters |
| SN74HC14N DIP14 | 1 | 60 | 60 | Conditions optical signals |
| Passive piezo buzzer | 1 | 30 | 30 | Audible reminders and faults |
| PN2222A transistor | 1 | 10 | 10 | Buzzer driver |
| 220 ohm resistors | 4 | 2 | 8 | Initial emitter current limiting |
| 10 kohm resistors | 9 | 2 | 18 | 4 optical pull-ups, 4 servo signal pull-downs, 1 buzzer pull-down |
| 1 kohm resistors | 6 | 2 | 12 | Buzzer base and piezo discharge resistors, plus four individual refill LED resistors |
| Diffused 5 mm green indicator LEDs | 4 | 5 | 20 | One beside each refill port; selected through USB software |
| 100 nF capacitors | 7 | 3 | 21 | Decoupling allowance |
| 1000 uF >=16 V electrolytic | 1 | 35 | 35 | Motor supply transient support, upstream of stop |
| Two-NC latching stop with adequate DC breaking capacity | 1 | 1000 | 1000 | Provisional allowance; exact rating/quote unresolved |
| Cover interlock microswitch and leads | 1 | 100 | 100 | Manual refill inhibit |
| Enclosed regulated 5 V 5 A adapter | 1 | 900 | 900 | Conditional on measured servo load test |
| 9 V 1 A centre-positive 5.5x2.1 mm adapter | 1 | 250 | 250 | Independent UNO supply |
| Main fuse and holder | 1 | 80 | 80 | Initial 5 A time-delay; final rating by measurements |
| Branch fuse and holder | 4 | 60 | 240 | Initial 2 A time-delay per servo; verify ratings |
| USB-C data cable | 1 | 100 | 100 | Computer setup; reuse if available |
| Copper wire and DC input socket assortment | 1 | 300 | 300 | Rated soldered harness; include a socket matching the motor adapter plug |
| Perfboard and heat-shrink assortment | 1 | 100 | 100 | Insulated sensor/driver assembly and joints |

**Parts subtotal: INR 5594.** Allow INR 500–1,000 delivery and INR 1,000 for spares/pack quantities/price variation: INR 7094–7594; plan approximately INR 7,500–8,000.

Conditional: the motor adapter must pass measured 5 V torque/current tests. An adequately enclosed supply and the stop's DC switching rating remain procurement checks. If the 5 V supply fails qualification, revisit power sizing and the budget before purchase; do not silently accept weak dispensing.

Reuse an owned compatible UNO R4 to save INR 1,000 and a suitable USB data cable to save INR 100. Buzzer retained because its circuit costs only about INR 46. Capacitor quantity is an allowance; retain required local decoupling. No detachable harness connector set is required; provide insulated strain-relieved soldered joints.

Excludes printing, mechanical hardware, tools, cleanable liner, laptop and labour. Small components may only be sold in packs.

Price references used for allowances: [UNO R4](https://robu.in/product-category/classic-family/), [LCD](https://robocraze.com/products/16x2-lcd-module-green), [I2C backpack](https://robocraze.com/products/16x2-lcd-i2c-interface-adapter), [motor adapter candidate](https://robu.in/product/pro-range-5v-5a-power-adapter/), [cover microswitch example](https://robu.in/product/kw1-103-z6a100-spdt-roller-lever-micro-switch1n/). The microswitch is a low-current sensing contact, not a substitute for the motor-power stop.


LED upgrade allowance: INR 28 (four LEDs at INR 5 and four extra resistors at INR 2). This is an estimate, not a newly verified retailer quotation; mounting/printing excluded.
