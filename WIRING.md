# Wiring Guide — 32×32 Cable Tester

This guide lays out every connection visually so you can wire the entire
circuit on two breadboards without hunting through the pin table.

---

## Breadboard Layout

```
 ┌──────────────────────────────────────────────────────────┐
 │                     BREADBOARD 1                          │
 │                                                           │
 │  ┌──────────────────┐    ┌──────────────────┐             │
 │  │   MUX1            │    │   DEMUX1          │             │
 │  │   CD74HC4067      │    │   CD74HC4067       │             │
 │  │                   │    │                    │             │
 │  │ C0..C15 = pins    │    │ C0..C15 = pins     │             │
 │  │   1..16 on test   │    │   1..16 on test    │             │
 │  └────┬──────────────┘    └────┬───────────────┘             │
 │       │                        │                              │
 │       │  S0..S3 ───────────────│── S0..S3 (shared, D8..D11)  │
 │       │  SIG ────┐            │  SIG ────┐                   │
 │       │  EN ──── A1           │  EN ──── D3                  │
 │       │          │            │          │                    │
 │  ─────┼──────────┼────────────┼──────────┼───── VCC (+5V)    │
 │  ─────┼──────────┼────────────┼──────────┼───── GND          │
 │       │          │            │          │                    │
 │       │     10µF ║            │     0.1µF (opt)              │
 └───────┼──────────┼────────────┼──────────┼────────────────────┘
         │          │            │          │
 ────────┼──────────┼────────────┼──────────┼── to Breadboard 2 ──
         │          │            │          │
 ┌───────┼──────────┼────────────┼──────────┼────────────────────┐
 │       │          │            │          │     BREADBOARD 2    │
 │  ┌────┴──────────┴──┐    ┌───┴──────────┴───┐                 │
 │  │   MUX2            │    │   DEMUX2          │                 │
 │  │   CD74HC4067      │    │   CD74HC4067       │                 │
 │  │                   │    │                    │                 │
 │  │ C0..C15 = pins    │    │ C0..C15 = pins     │                 │
 │  │  17..32 on test   │    │  17..32 on test    │                 │
 │  └────┬──────────────┘    └────┬───────────────┘                 │
 │       │                        │                                  │
 │       │  S0..S3 ───────────────│── S0..S3 (shared, D8..D11)      │
 │       │  SIG ────┐            │  SIG ────┐                       │
 │       │  EN ──── D13          │  EN ──── D2                      │
 │       │          │            │          │                        │
 │  ─────┼──────────┼────────────┼──────────┼───── VCC (+5V)        │
 │  ─────┼──────────┼────────────┼──────────┼───── GND              │
 │       │          │            │          │                        │
 │       │     10µF ║            │     0.1µF (opt)                  │
 └───────┼──────────┼────────────┼──────────┼────────────────────────┘
         │          │            │          │
         ▼          ▼            ▼          ▼
      To Arduino   To SIG      To SIG     To Arduino
      D4..D7       bus         bus        D8..D11
```

---

## Signal Bus Detail

```
  Arduino D12 ───[ 1kΩ ]───┬─── MUX1 SIG (pin 1)
                            ├─── MUX2 SIG (pin 1)
                            │
                            │  D12 drives HIGH during test
                            │  Resistor limits current on shorts

  Arduino A0  ─────────────┬─── DEMUX1 SIG (pin 1)
                            ├─── DEMUX2 SIG (pin 1)
                            │
                            ├───[ 10kΩ ]─── GND
                            │
                            │  A0 reads back signal
                            │  Pull-down keeps bus LOW when open
```

---

## Address & Enable Routing

```
         Address Lines (shared in parallel)
  ─────────────────────────────────────────────

       MUX S0..S3              DEMUX S0..S3
       (Arduino D4..D7)        (Arduino D8..D11)
            │                        │
    ┌───────┼────────┐       ┌───────┼────────┐
    ▼       ▼        ▼       ▼       ▼        ▼
  MUX1   MUX2     (float   DEMUX1  DEMUX2   (float
  S0..S3 S0..S3   when off) S0..S3  S0..S3   when off)


         Enable Lines (one per IC, active LOW)
  ─────────────────────────────────────────────

  Arduino A1  ──── MUX1 EN   ────[ 10kΩ ]─── VCC
  Arduino D13 ──── MUX2 EN   ────[ 10kΩ ]─── VCC
  Arduino D3  ──── DEMUX1 EN ────[ 10kΩ ]─── VCC
  Arduino D2  ──── DEMUX2 EN ────[ 10kΩ ]─── VCC

  Each EN pin is pulled HIGH (disabled) by default.
  Arduino pulls LOW to enable the selected IC.
```

---

## LCD (I²C)

```
  LCD PCF8574          Arduino
  ──────────           ───────
     SDA  ────────────  A4
     SCL  ────────────  A5
     VCC  ────────────  5V
     GND  ────────────  GND

  Default address: 0x27
  If blank screen, try: 0x3F
```

---

## Arduino Pin Summary (cheat-sheet)

```
         ┌──────────────────────────┐
         │      Arduino UNO          │
         │                           │
  D2  ───│→ DEMUX2 EN               │
  D3  ───│→ DEMUX1 EN               │
  D4  ───│→ MUX S0 (both)           │
  D5  ───│→ MUX S1 (both)           │
  D6  ───│→ MUX S2 (both)           │
  D7  ───│→ MUX S3 (both)           │
  D8  ───│→ DEMUX S0 (both)         │
  D9  ───│→ DEMUX S1 (both)         │
  D10 ───│→ DEMUX S2 (both)         │
  D11 ───│→ DEMUX S3 (both)         │
  D12 ───│→ SIG bus (via 1kΩ)       │
  D13 ───│→ MUX2 EN                 │
  A0  ───│← DEMUX SIG bus           │
  A1  ───│→ MUX1 EN                 │
  A4  ───│↔ LCD SDA (I²C)           │
  A5  ───│↔ LCD SCL (I²C)           │
  5V ────│→ breadboard rails        │
  GND ───│→ breadboard rails        │
         └──────────────────────────┘
```

---

## Test Cable Connection

The cable under test connects **MUX C0..C31 ↔ DEMUX C0..C31**:

```
  Cable pin 1  ─── MUX1 C0   ────[ wire ]─── DEMUX1 C0
  Cable pin 2  ─── MUX1 C1   ────[ wire ]─── DEMUX1 C1
  ...
  Cable pin 16 ─── MUX1 C15  ────[ wire ]─── DEMUX1 C15
  Cable pin 17 ─── MUX2 C0   ────[ wire ]─── DEMUX2 C0
  ...
  Cable pin 32 ─── MUX2 C15  ────[ wire ]─── DEMUX2 C15
```

---

## Power Distribution

```
  Arduino 5V ──┬── Breadboard 1 VCC rail ──┬── MUX1 VCC
               │                           ├── DEMUX1 VCC
               │                           └── LCD VCC
               │
               └── Breadboard 2 VCC rail ──┬── MUX2 VCC
                                           └── DEMUX2 VCC

  Arduino GND ──┬── Breadboard 1 GND rail ──┬── MUX1 GND
               │                            ├── DEMUX1 GND
               │                            └── LCD GND
               │
               └── Breadboard 2 GND rail ──┬── MUX2 GND
                                           └── DEMUX2 GND

  Decoupling:
    - 10µF electrolytic across VCC/GND on each breadboard
    - 0.1µF ceramic near each CD74HC4067 VCC pin (optional but helps)
```

---

## Quick Verification (before connecting cable)

1. Power on → LCD shows `CableTester 32x32` then `1-based index`
2. Serial monitor (115200 baud) → all 32 lines should show `OPEN no continuity`
3. Test: jumper MUX1 C0 → DEMUX1 C0 → line 1 should show `OK correct`
4. Test: jumper MUX1 C0 → DEMUX1 C5 → line 1 should show `CROSS to 6`
