#include "Pins.cpp"
#include "synth-core.h"
#include "Columns.cpp"
#include "Controls.cpp"
#include "Lights.cpp"

Synth synth;
Controls controls;
Columns columns(NUM_OF_COLUMNS);
Lights lights;

void setup() {
  synth.begin(CHB); // pin D3 for audio output
  controls.setupPotPins(CONTROLS_SHAPE_PIN, CONTROLS_MODULATION_PIN, CONTROLS_TEMPO_PIN, CONTROLS_DURATION_PIN);
  columns.setupMuxPins(MUX_INHIBIT_PIN, MUX_PIN_A, MUX_PIN_B, MUX_PIN_C);
  columns.setupInputPins(COLUMNS_PITCH_PIN, COLUMNS_TOGGLE_PIN);
  lights.setupStripPin(LIGHTS_STRIP_PIN);
}

int colNr = -1;
long lastTime = 0;

void loop() {
  // read controls and columns values
  controls.read();
  columns.read();
  lights.show();
  
  // play next column from the sequence
  long currTime = millis();
  if (currTime - lastTime > controls.delay) {
    if (++colNr == NUM_OF_COLUMNS) colNr = 0;
    Column column = columns[colNr];
    if (column.enabled) {
      synth.setupVoice(colNr%4, controls.wave, column.pitch, controls.envelope, controls.length, controls.mod);
      synth.trigger(colNr%4);
    }
    lights.change(colNr, column.enabled, column.pitch);
    lastTime = currTime;
  }
}
