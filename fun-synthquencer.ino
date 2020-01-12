#include "synth.h"
#include "Columns.cpp"

// synthesizer library from https://github.com/dzlonline/the_synth
synth edgar;

const int NUM_OF_COLUMNS = 8;
const int COLUMNS_POT_PIN = A7;
const int COLUMNS_BUTTON_PIN = 13;
const int MUX_ENABLE_PIN = 5;
const int MUX_PIN_A = 4;
const int MUX_PIN_B = 3;
const int MUX_PIN_C = 2;
Columns columns(NUM_OF_COLUMNS, COLUMNS_POT_PIN, COLUMNS_BUTTON_PIN);

const int DEBUGPIN = 12;

void setup() {
  edgar.begin(CHA); // voice, wave, pitch, env, length, mod
  edgar.setupVoice(0, SAW, 49, ENVELOPE3, 75, 64);
  columns.setupMuxPins(MUX_ENABLE_PIN, MUX_PIN_A, MUX_PIN_B, MUX_PIN_C);
}

int colNr = -1;
long lastTime = 0;

void loop() {
  // read controls and columns values
  columns.read();
  delay(11);   

  // play next column from he sequence
  long currTime = millis();
  if (currTime - lastTime > 200) {
    if (++colNr == NUM_OF_COLUMNS) colNr = 0;
    Column column = columns[colNr];
    if (column.enabled) {
      edgar.setPitch(0, column.pitch);
      edgar.trigger(0);
    }
    lastTime = currTime;
  }
}





// useful for debuging, blink a led
void debugFlick(long delayTime) {
  pinMode(DEBUGPIN, OUTPUT);
  digitalWrite(DEBUGPIN, HIGH);
  delay(delayTime / 2);
  digitalWrite(DEBUGPIN, LOW);
  delay(delayTime / 2);
}
