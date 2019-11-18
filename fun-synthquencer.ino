#include "synth.h"
#include "MedianFilter.h"
#include "Columns.cpp"

synth edgar;

const int COLUMN1_SELECT_PIN = 7;
const int COLUMN2_SELECT_PIN = 5;

const int COLUMNS_POT_PIN = A7;
const int COLUMNS_BUTTON_PIN = 10;

const int DEBUGPIN = 3;

int pots[10];
boolean buts[10];

long lastTime = 0;
boolean myFlag = false;

MedianFilter filterObject(10, 0);

Columns newColumns(2, COLUMNS_POT_PIN, COLUMNS_BUTTON_PIN);

void setup() {
  edgar.begin(CHA);// voice, wave, pitch, env, length, mod
  edgar.setupVoice(0,SAW,49,ENVELOPE3,75,64);
  pinMode(DEBUGPIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("c1,c2");

  newColumns[0].selectPin = COLUMN1_SELECT_PIN;
  newColumns[1].selectPin = COLUMN2_SELECT_PIN;
  pinMode(COLUMN1_SELECT_PIN, OUTPUT);
  pinMode(COLUMN2_SELECT_PIN, OUTPUT);
}

int colNr = -1;

void loop() {
  uint32_t t = millis();
  digitalWrite(newColumns[0].selectPin, LOW);
  digitalWrite(newColumns[1].selectPin, LOW);
  newColumns.read();
  digitalWrite(DEBUGPIN, newColumns[1].enabled);
  if (newColumns[colNr].enabled) {
    digitalWrite(newColumns[colNr].selectPin, HIGH);
  }
  if (t - lastTime > 180) {
    colNr++;
    if (colNr > 1) colNr = 0;
    edgar.setPitch(0, map(newColumns[colNr].potValue, 0, 850, 0, 127));
    if (newColumns[colNr].enabled) {
      edgar.trigger(0);
    }
    Serial.print(newColumns[0].potValue);
    Serial.print(",");
    Serial.println(newColumns[1].potValue);
    lastTime = t;
  }
  delay(10);
}




void flick(long delayTime) {
  digitalWrite(DEBUGPIN, HIGH);
  delay(delayTime / 2);
  digitalWrite(DEBUGPIN, LOW);
  delay(delayTime / 2);
}
