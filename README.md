# 32-line cable continuity and mapping tester

An Arduino Uno sketch that scans a 32 × 32 source-to-sink connection matrix through four CD74HC4067 analog multiplexers. For each driven source, it checks all 32 sink positions and reports the observed mapping on a 16 × 2 I²C LCD and at 115200 baud over Serial.

The repository contains [firmware](firmware.ino) and a [wiring guide](WIRING.md). It does not contain a recorded hardware test report, photographs, or an automated test harness. The status labels below describe the firmware's classification logic, not measured fault-detection performance.

| Sink positions detected for one source | Firmware result | Interpretation |
| --- | --- | --- |
| None | `OPEN` | No continuity at any scanned sink |
| Exactly one, same index | `OK` | Expected one-to-one connection |
| Exactly one, different index | `CROSS` | Connection reaches another sink |
| More than one | `SHORT` | Multiple sink positions respond |

A `SHORT` result identifies multiple responding paths, but this scan alone cannot locate a physical short or distinguish every possible cable fault topology. It tests continuity and mapping; it does not measure resistance, insulation, intermittent contact, or performance under load.

## Hardware and wiring

- Arduino Uno or compatible 5 V board
- Four CD74HC4067 devices: two 16-channel source banks and two 16-channel sink banks
- 16 × 2 LCD with a PCF8574 I²C backpack
- 1 kΩ resistor between Uno D12 and the shared source signal bus
- 10 kΩ pull-down on the shared sink bus at A0
- Four 10 kΩ enable pull-ups, one per multiplexer
- Power-rail capacitors and local 0.1 µF decoupling close to each multiplexer
- Cable fixture and common-ground wiring appropriate to the board

The source address lines S0–S3 use D4–D7; sink address lines use D8–D11. Source bank enables use A1 and D13; sink bank enables use D3 and D2. The LCD uses A4/A5 for SDA/SCL. Inputs 1–16 map to bank 0 channels 0–15; 17–32 map to bank 1 channels 0–15. The complete connection table and signal-bus layout are in [WIRING.md](WIRING.md). Check the CD74HC4067 voltage ratings and wiring before powering the circuit.

## Build and run

The sketch is stored at the repository root as `firmware.ino`. Arduino IDE expects a sketch file inside a folder of the same base name: copy it to `firmware/firmware.ino`, or place the cloned contents in a folder named `firmware` before opening it.

Install `LiquidCrystal_I2C` (Frank de Brabander, version 1.1.2 as documented for this sketch) through the Arduino Library Manager. The sketch defaults to LCD address `0x27`. For a `0x3F` backpack, select `lcd2` and initialize/backlight that instance in `setup()`; the commented example is next to the current `lcd1.init()` call. Use an I²C scanner if the address is unknown.

Upload to the Uno, open Serial Monitor at **115200 baud**, and connect a known-good cable before introducing fault examples. The scan advances through all 32 sources; each Serial row contains the source index, classification, and a note. The LCD shows the current source and detected sink or fault class. The sketch pauses 150 ms after each source and 600 ms at the end of a scan.

A `platformio.ini` is present, but the repository does not use PlatformIO's standard `src/main.cpp` layout. Arduino IDE is the documented build route; a PlatformIO build needs its source layout configured separately.

## What to verify on a physical build

Record the board, LCD library version/address, supply voltage, fixture wiring, and a Serial log for at least these cases: 1→1 and 32→32 continuity, an open source, a single crossed connection, and one source connected to two sinks. Compare each displayed class with the fixture's known wiring. Repeat scans to investigate contact bounce or intermittent behavior. No such measurements are committed here, so hardware reliability and detection coverage remain unverified.

The firmware energizes one source bank/channel at a time and scans both sink banks, disabling the sink bank between passes. It is a prototype for controlled low-voltage continuity tests, not a certified cable tester. See [LICENSE](LICENSE).
