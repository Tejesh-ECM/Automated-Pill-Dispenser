#pragma once
#if !defined(ARDUINO_UNOR4_MINIMA) && !defined(ARDUINO_UNOR4_WIFI)
#error "Select UNO R4 Minima or UNO R4 WiFi"
#endif
// Change only after completing docs/ACCEPTANCE_TESTS.md with inert objects.
constexpr bool HARDWARE_COMMISSIONED=false;
constexpr uint8_t LCD_ADDRESS=0x27;
constexpr uint8_t SERVO_PINS[4]={2,3,4,5};
constexpr uint8_t BEAM_PINS[4]={0,1,8,12};
// These pins no longer connect to raw optical nodes. Measure those at test pads.
constexpr uint8_t REFILL_LED_PINS[4]={A0,A1,A2,A3};
constexpr uint8_t STOP_PIN=9, LID_PIN=7, BUZZER_PIN=10;
constexpr uint16_t CLOSED_US[4]={2000,2000,2000,2000};
constexpr uint16_t OPEN_US[4]={1000,1000,1000,1000};
constexpr uint16_t MIN_BEAM_PULSE_US=300;
constexpr uint32_t PILL_TIMEOUT_MS=8000, DOSE_TIMEOUT_MS=120000;
constexpr uint32_t HOST_TIMEOUT_MS=5000;

