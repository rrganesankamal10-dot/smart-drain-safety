# Smart Drain Safety & Early-Warning System

An ESP32-based IoT prototype that monitors underground drains for water level, flow rate and a gas indicator, warns people nearby with a buzzer and LED, and shows live data on a Wi-Fi dashboard for municipal staff.

Built by **KAMALRAJ G** (individual participant), PSNA College of Engineering and Technology, Dindigul, Tamil Nadu, for *Hack for Social Cause – Youth Tech Challenge 2027* (Theme: Governance & Civic Technology).

## Why

Drains are often inspected manually and only after a blockage or flood. This node gives early warning of overflow, suspected blockage and gas build-up, which protects residents and sanitation workers. See [DESIGN.md](DESIGN.md) for the full design and architecture diagram.

## Features

- Water-level, flow-rate and gas readings once per second
- Alert rules: `GAS_ALERT`, `OVERFLOW_ALERT`, `BLOCKAGE_WARNING`, `NORMAL`
- Local buzzer and LED alert
- Built-in web dashboard (`/`) and JSON endpoint (`/data`)

## Files

| File | Purpose |
|---|---|
| `smart_drain.ino` | ESP32 Arduino firmware |
| `config.h` | Pins, Wi-Fi, calibration and alert thresholds |
| `DESIGN.md` | Technical design document with architecture diagram |
| `sample_readings.csv` | Sample sensor readings with the expected status |
| `threshold_logic.py` | Reference implementation of the alert rules |
| `test_thresholds.py` | Test cases for the alert rules |
| `LICENSE` | MIT license |

## Hardware

- ESP32 development board
- Water-level sensor with analog output
- Flow sensor with pulse output
- Gas sensor with analog output
- Buzzer and LED (with resistor)

Default pins are in `config.h` (level 34, gas 35, flow 27, buzzer 25, LED 26). **Edit them to match your wiring and sensors.**

## Setup

1. Install the Arduino IDE and add ESP32 board support (Boards Manager, "esp32 by Espressif Systems").
2. Put `smart_drain.ino` and `config.h` together in a folder named `smart_drain` (the Arduino IDE requires the sketch folder to match the sketch name) and open `smart_drain.ino`.
3. Edit `config.h`: set `WIFI_SSID` and `WIFI_PASSWORD`, confirm the pins, and calibrate `LEVEL_RAW_FULL`, `FLOW_PULSES_PER_LPM` and the alert thresholds.
4. Select your ESP32 board and port, then upload.
5. Open the Serial Monitor at 115200 baud. It prints the dashboard address, for example `http://192.168.1.50`.
6. Open that address in a browser on the same Wi-Fi network.

### Environment variables

This project has no environment variables. All settings live in `config.h`. Do not commit your real Wi-Fi password; keep the placeholder values in the repository.

## Running the tests

The alert rules are tested in Python (no extra packages needed):

```
python -m unittest test_thresholds -v
```

The tests check every rule, the boundary values, and each row of `sample_readings.csv`.

## Sample data

`sample_readings.csv` contains synthetic readings used for testing the alert rules. It is not field data.

## Limitations

Thresholds need on-site calibration. The gas sensor gives a raw indicator, not a certified measurement, so it is a warning aid and not a replacement for approved safety equipment.

## License

MIT, see [LICENSE](LICENSE).
