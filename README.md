# Cable Tester 32×32 (Arduino + CD74HC4067 + I²C LCD)

A low-cost, Arduino-based cable tester that scans **32 lines** using **two 16-channel multiplexers (MUX)** and **two 16-channel demultiplexers (DEMUX)**. It detects **OK**, **OPEN**, **CROSS**, and **SHORT** conditions and shows live status on an I²C LCD while logging details over Serial.


---

## Features
- Tests up to **32 wires** (2× CD74HC4067 as sources, 2× as sinks)
- **1-based indexing**: Pins **1..16** map to MUX1/DEMUX1 C0..C15, **17..32** map to MUX2/DEMUX2 C0..C15
- **Live LCD** status + detailed **Serial** logs
- Robust logic: classifies **OK / OPEN / CROSS / SHORT**
- Breadboard friendly; easy to extend beyond 32 lines

---

## Dependencies (Arduino Libraries)

Install via **Arduino Library Manager** (Sketch → Include Library → Manage Libraries):

| Library               | Author             | Tested Version |
|-----------------------|--------------------|----------------|
| `LiquidCrystal_I2C`   | Frank de Brabander | 1.1.2          |

The firmware uses the LCD at I²C address `0x27` by default. If your module is
`0x3F`, uncomment the `lcd = &lcd2` line in `setup()` (see `firmware.ino:70`).

To find your LCD address, run an **I²C Scanner** sketch:
```
File → Examples → Wire → i2c_scanner
```

---

## Bill of Materials (BOM)
- 1× Arduino UNO (or compatible 5V board)
- 4× CD74HC4067 (16-ch MUX/DEMUX; using 2 as MUX, 2 as DEMUX)
- 1× 16×2 I²C LCD (PCF8574 backpack, address 0x27 or 0x3F)
- Resistors:
  - 1× **1 kΩ** (Arduino D12 → MUX SIG bus protection)
  - 1× **10 kΩ** (DEMUX SIG bus pull-down to GND)
  - 4× **10 kΩ** (EN pull-ups: MUX1/MUX2/DEMUX1/DEMUX2 → VCC)
- Capacitors:
  - 2× **10 µF** electrolytic (one per breadboard rails)
  - *(Recommended)* 4× **0.1 µF** ceramic (one per IC, close to VCC/GND)
- 2× breadboards + jumper wires
- The cable under test (up to 32 lines)

---

## Wiring Overview

> 📐 **Visual wiring diagrams** → see [WIRING.md](WIRING.md) for ASCII layout,
> signal bus details, and a one-page cheat-sheet.



### Signal Buses
- **MUX SIG bus**: `D12 (OUTPUT) → 1 kΩ → MUX1 SIG + MUX2 SIG`
- **DEMUX SIG bus**: `A0 (INPUT) ← DEMUX1 SIG + DEMUX2 SIG`, with `10 kΩ → GND`

### Shared Address Lines (parallel)
- **MUX S0..S3**: `D4, D5, D6, D7` → both MUX1 & MUX2
- **DEMUX S0..S3**: `D8, D9, D10, D11` → both DEMUX1 & DEMUX2

### Enable (active-LOW, each with 10 kΩ pull-up to its local VCC)
- **MUX1 EN** ← `A1`
- **MUX2 EN** ← `D13`
- **DEMUX1 EN** ← `D3`
- **DEMUX2 EN** ← `D2`

### LCD (I²C)
- SDA → `A4`, SCL → `A5`, VCC → `5V`, GND → `GND`  
- Address typically `0x27` (some are `0x3F`)

### Power
- Arduino 5V/GND → Board-1 rails → Board-2 rails (star distribution)  
- 10 µF across rails on each board; add 0.1 µF ceramics near each CD74HC4067 if available.

---

## Full Pin Mapping

| Arduino | Function      | Connected To                                  | Notes              |
|--------:|----------------|-----------------------------------------------|--------------------|
| D2      | EN_DEMUX2     | DEMUX2 EN (10 kΩ pull-up to VCC)              | Active-LOW         |
| D3      | EN_DEMUX1     | DEMUX1 EN (10 kΩ pull-up to VCC)              | Active-LOW         |
| D4      | MUX S0        | MUX1 S0, MUX2 S0                              | Shared line        |
| D5      | MUX S1        | MUX1 S1, MUX2 S1                              | Shared line        |
| D6      | MUX S2        | MUX1 S2, MUX2 S2                              | Shared line        |
| D7      | MUX S3        | MUX1 S3, MUX2 S3                              | Shared line        |
| D8      | DEMUX S0      | DEMUX1 S0, DEMUX2 S0                          | Shared line        |
| D9      | DEMUX S1      | DEMUX1 S1, DEMUX2 S1                          | Shared line        |
| D10     | DEMUX S2      | DEMUX1 S2, DEMUX2 S2                          | Shared line        |
| D11     | DEMUX S3      | DEMUX1 S3, DEMUX2 S3                          | Shared line        |
| D12     | SIG_SRC       | MUX SIG bus (through 1 kΩ)                    | Drives HIGH        |
| D13     | EN_MUX2       | MUX2 EN (10 kΩ pull-up to VCC)                | Active-LOW         |
| A0      | SIG_SNK       | DEMUX SIG bus (10 kΩ pull-down to GND)        | Reads continuity   |
| A1      | EN_MUX1       | MUX1 EN (10 kΩ pull-up to VCC)                | Active-LOW         |
| A4      | LCD SDA       | PCF8574 SDA                                   | I²C address 0x27/0x3F |
| A5      | LCD SCL       | PCF8574 SCL                                   |                    |
| 5V/GND  | Power         | Breadboard rails → all ICs                    | Decoupling on rails|

---
