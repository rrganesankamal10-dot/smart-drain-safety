// config.h - edit these values to match YOUR wiring and calibration.
// Pin numbers below are defaults; change them if your build differs.
#pragma once

// ---- Wi-Fi (the ESP32 joins this network and serves the dashboard) ----
#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// ---- Pins (use ADC1 pins 32-39 for analog sensors; ADC2 conflicts with Wi-Fi) ----
#define PIN_LEVEL   34   // water-level sensor, analog output
#define PIN_GAS     35   // gas sensor, analog output
#define PIN_FLOW    27   // flow sensor, pulse output
#define PIN_BUZZER  25
#define PIN_LED     26

// ---- Sampling ----
#define SAMPLE_MS   1000

// ---- Water level calibration (raw ADC 0-4095 when dry / fully submerged) ----
#define LEVEL_RAW_EMPTY 0
#define LEVEL_RAW_FULL  3000

// ---- Flow sensor: pulses per second for 1 litre/minute (see your sensor datasheet) ----
#define FLOW_PULSES_PER_LPM 7.5f

// ---- Alert thresholds (calibrate on site) ----
#define LEVEL_WARN_PCT   60.0f   // water level where blockage check starts
#define LEVEL_ALERT_PCT  85.0f   // overflow risk
#define FLOW_LOW_LPM      1.0f   // flow at or below this with high level = suspected blockage
#define GAS_ALERT_RAW    2000    // raw ADC value treated as hazardous gas indicator
