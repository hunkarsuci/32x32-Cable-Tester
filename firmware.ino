#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- LCD ----
LiquidCrystal_I2C lcd1(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x3F, 16, 2);
LiquidCrystal_I2C* lcd = &lcd1;

// ---- Pins (same as docs) ----
const int MUX_S[4]   = {4,5,6,7};      // MUX S0..S3 (shared)
const int DEMUX_S[4] = {8,9,10,11};    // DEMUX S0..S3 (shared)
const int EN_MUX1    = A1;             // active LOW (10k pull-up)
const int EN_MUX2    = 13;             // active LOW (10k pull-up)
const int EN_DEMUX1  = 3;              // active LOW (10k pull-up)
const int EN_DEMUX2  = 2;              // active LOW (10k pull-up)
const int SIG_SRC    = 12;             // OUTPUT -> 1k -> MUX SIG bus
const int SIG_SNK    = A0;             // INPUT  <- DEMUX SIG bus (10k to GND)

static inline void setSel(const int p[4], uint8_t ch){
  digitalWrite(p[0], ch & 0x01);
  digitalWrite(p[1], ch & 0x02);
  digitalWrite(p[2], ch & 0x04);
  digitalWrite(p[3], ch & 0x08);
}
static inline void enableMuxBank(uint8_t b){ digitalWrite(EN_MUX1, (b==0)?LOW:HIGH); digitalWrite(EN_MUX2, (b==1)?LOW:HIGH); }
static inline void disableMux(){ digitalWrite(EN_MUX1,HIGH); digitalWrite(EN_MUX2,HIGH); }
static inline void enableDemuxBank(uint8_t b){ digitalWrite(EN_DEMUX1, (b==0)?LOW:HIGH); digitalWrite(EN_DEMUX2, (b==1)?LOW:HIGH); }
static inline void disableDemux(){ digitalWrite(EN_DEMUX1,HIGH); digitalWrite(EN_DEMUX2,HIGH); }
static inline int oneBased(int bank, int ch){ return bank*16 + ch + 1; } // 1..32

void lcdShow(int src1b, int hits, int hit1b){
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(F("SRC:")); lcd->print(src1b);
  lcd->print(F("  H:"));
  if(hits==1) lcd->print(hit1b);
  else lcd->print('-');

  lcd->setCursor(0,1);
  if(hits==0) {
    lcd->print(F("OPEN no cont.   "));
  } else if(hits==1) {
    if(hit1b==src1b) lcd->print(F("OK correct      "));
    else {
      lcd->print(F("CROSS->")); lcd->print(hit1b);
      int cur = 7 + (hit1b >= 10 ? 2 : 1);
      for(int i=cur;i<16;i++) lcd->print(' ');
    }
  } else {
    lcd->print(F("SHORT multiple  "));
  }
}

void setup(){
  for(int i=0;i<4;i++){ pinMode(MUX_S[i],OUTPUT); pinMode(DEMUX_S[i],OUTPUT); }
  pinMode(EN_MUX1,OUTPUT); pinMode(EN_MUX2,OUTPUT);
  pinMode(EN_DEMUX1,OUTPUT); pinMode(EN_DEMUX2,OUTPUT);
  pinMode(SIG_SRC,OUTPUT); pinMode(SIG_SNK,INPUT);

  disableMux(); disableDemux();
  for(int i=0;i<4;i++){ digitalWrite(MUX_S[i],LOW); digitalWrite(DEMUX_S[i],LOW); }
  digitalWrite(SIG_SRC,LOW);

  Serial.begin(115200);
  Serial.println(F("SRC(1..32)\tDETECT\tNOTE"));

  Wire.begin();
  lcd1.init(); lcd1.backlight();
  // If your module is 0x3F, switch to:
  // lcd = &lcd2; lcd->init(); lcd->backlight();

  lcd->clear();
  lcd->setCursor(0,0); lcd->print(F("CableTester 32x32"));
  lcd->setCursor(0,1); lcd->print(F("1-based index"));
  delay(500);
}

void loop(){
  for(int srcIdx=0; srcIdx<32; srcIdx++){
    int sBank = srcIdx / 16;
    int sCh   = srcIdx % 16;

    enableMuxBank(sBank);
    setSel(MUX_S, sCh);
    delayMicroseconds(80);
    digitalWrite(SIG_SRC, HIGH);
    delayMicroseconds(80);

    int hits=0, hitBank=-1, hitCh=-1;

    for(int dBank=0; dBank<2; dBank++){
      enableDemuxBank(dBank);
      for(int dst=0; dst<16; dst++){
        setSel(DEMUX_S, dst);
        delayMicroseconds(50);
        if(digitalRead(SIG_SNK)==HIGH){
          hits++; hitBank=dBank; hitCh=dst;
        }
      }
      disableDemux();
    }

    digitalWrite(SIG_SRC, LOW);
    disableMux();

    int src1b = oneBased(sBank, sCh);
    Serial.print(src1b); Serial.print('\t');

    if(hits==0){
      Serial.println(F("OPEN\tno continuity"));
      lcdShow(src1b, 0, -1);
    } else if(hits==1){
      int hit1b = oneBased(hitBank, hitCh);
      if(hit1b==src1b){
        Serial.println(F("OK\tcorrect mapping"));
      } else {
        Serial.print(F("CROSS\tto ")); Serial.println(hit1b);
      }
      lcdShow(src1b, 1, hit1b);
    } else {
      Serial.println(F("SHORT\tmultiple lines responded"));
      lcdShow(src1b, 2, -1);
    }

    delay(150);
  }

  lcd->setCursor(0,1); lcd->print(F("Scan done       "));
  Serial.println();
  delay(600);
}
