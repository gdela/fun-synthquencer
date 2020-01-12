#include "synth.h"
#include "MedianFilter.h"
#include "Columns.cpp"

synth edgar;

const int MUX_ENABLE_PIN = 5;
const int MUX_PIN_A = 4;
const int MUX_PIN_B = 3;
const int MUX_PIN_C = 2;

const int COLUMNS_POT_PIN = A7;
const int COLUMNS_BUTTON_PIN = 13;

const int DEBUGPIN = 12;

int pots[10];
boolean buts[10];

long lastTime = 0;
boolean myFlag = false;

MedianFilter filterObject(10, 0);

Columns columns(8, COLUMNS_POT_PIN, COLUMNS_BUTTON_PIN);

void setup() {
  edgar.begin(CHA); // voice, wave, pitch, env, length, mod
  edgar.setupVoice(0,SAW,49,ENVELOPE3,75,64);
  pinMode(DEBUGPIN, OUTPUT);
  columns.setupMuxPins(MUX_ENABLE_PIN, MUX_PIN_A, MUX_PIN_B, MUX_PIN_C);
}

int colNr = -1;

void loop() {
  uint32_t t = millis();
  columns.read();
  if (t - lastTime > 200) {
    colNr++;
    if (colNr > 7) colNr = 0;
    columns.highlight(colNr);
    edgar.setPitch(0, columns[colNr].pitch);
    if (columns[colNr].enabled) {
      edgar.trigger(0);
    }
    lastTime = t;
  }
  delay(20); // how often columns and controls are read
}




void flick(long delayTime) {
  digitalWrite(DEBUGPIN, HIGH);
  delay(delayTime / 2);
  digitalWrite(DEBUGPIN, LOW);
  delay(delayTime / 2);
}
