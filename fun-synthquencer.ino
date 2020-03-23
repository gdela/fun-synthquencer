#include "synth.h"
#include "Columns.cpp"
#include "Controls.cpp"

// synthesizer library from https://github.com/dzlonline/the_synth
synth edgar;

const int NUM_OF_COLUMNS = 8;
const int COLUMNS_POT_PIN = A7;
const int COLUMNS_BUTTON_PIN = 7;
const int MUX_INHIBIT_PIN = 2;
const int MUX_PIN_A = 4;
const int MUX_PIN_B = 5;
const int MUX_PIN_C = 6;
Columns columns(NUM_OF_COLUMNS, COLUMNS_POT_PIN, COLUMNS_BUTTON_PIN);
Controls controls;

const int DEBUGPIN = 13;

void setup() {
  edgar.begin(CHA); // voice, wave, pitch, env, length, mod
  for (int i=0; i<4; i++) edgar.setupVoice(i, SAW, 0, ENVELOPE0, 64, 64);
  columns.setupMuxPins(MUX_INHIBIT_PIN, MUX_PIN_A, MUX_PIN_B, MUX_PIN_C);
  for (int i=0; i<3; i++) debugFlick(100);
}

int colNr = -1;
long lastTime = 0;

void loop() {
  // read controls and columns values
  controls.read();
  columns.read();

  // play next column from he sequence
  long currTime = millis();
  if (currTime - lastTime > controls.delay) {
    if (++colNr == NUM_OF_COLUMNS) colNr = 0;
    Column column = columns[colNr];
    if (column.enabled) {
      edgar.setWave(colNr%4, controls.wave);
      edgar.setEnvelope(colNr%4, controls.envelope);
      edgar.setLength(colNr%4, controls.length);
      edgar.setMod(colNr%4, controls.mod);
      edgar.setPitch(colNr%4, column.pitch);
      edgar.trigger(colNr%4);
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
