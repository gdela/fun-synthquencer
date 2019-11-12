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
Column columns[2];

void setup() {
  edgar.begin(CHA);// voice, wave, pitch, env, length, mod
  edgar.setupVoice(0,SAW,49,ENVELOPE3,75,64);
  pinMode(COLUMN1_SELECT_PIN, OUTPUT);
  pinMode(COLUMN2_SELECT_PIN, OUTPUT);
  digitalWrite(COLUMN1_SELECT_PIN, LOW);
  digitalWrite(COLUMN2_SELECT_PIN, LOW);
  pinMode(COLUMNS_POT_PIN, INPUT);
  pinMode(COLUMNS_BUTTON_PIN, INPUT);
  pinMode(DEBUGPIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("start,hopsa");

  columns[0].selectPin = COLUMN1_SELECT_PIN;
  columns[1].selectPin = COLUMN2_SELECT_PIN;

  newColumns[0].selectPin = COLUMN1_SELECT_PIN;
  newColumns[1].selectPin = COLUMN2_SELECT_PIN;
}

int colNr = -1;

void loop() {
  uint32_t t = millis();
  digitalWrite(columns[0].selectPin, LOW);
  digitalWrite(columns[1].selectPin, LOW);
  readColumns();
  digitalWrite(DEBUGPIN, columns[1].enabled);
  if (columns[colNr].enabled) {
    digitalWrite(columns[colNr].selectPin, HIGH);
  }
  if (t - lastTime > 180) {
    colNr++;
    if (colNr > 1) colNr = 0;
    Serial.print(colNr);
    Serial.print(": ");
    edgar.setPitch(0, map(columns[colNr].potValue, 0, 850, 0, 127));
    if (columns[colNr].enabled) {
      edgar.trigger(0);
    }
    Serial.print(map(columns[0].potValue, 0, 850, 0, 127));
    Serial.print(",");
    Serial.println(map(columns[1].potValue, 0, 850, 0, 127));
    lastTime = t;
  }
  delay(10);
}

void readColumns() {
  for (int i = 0; i <= 1; i++) {
    readColumn(columns[i]);
  }
}

void readColumn(Column &column) {
  digitalWrite(column.selectPin, HIGH);

  int readPotValue = analogRead(COLUMNS_POT_PIN); // todo: trzy odczyty i mediana?
  column.potValues.in(readPotValue);
  column.potValue = column.potValues.out();

  int readButtonState = digitalRead(COLUMNS_BUTTON_PIN);
  if (column.buttonState != readButtonState && (column.buttonChanged < (millis() - 50))) { // debounce
    column.buttonState = readButtonState;
    column.buttonChanged = millis();
    if (readButtonState == HIGH) {
      column.enabled = !column.enabled;
    }
  }

  digitalWrite(column.selectPin, LOW);
}

void flick(long delayTime) {
  digitalWrite(DEBUGPIN, HIGH);
  delay(delayTime / 2);
  digitalWrite(DEBUGPIN, LOW);
  delay(delayTime / 2);
}
