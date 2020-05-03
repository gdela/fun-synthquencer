#include <stdarg.h>
#include <stdio.h>
#include "Arduino.h"

/**
 * General controls of the sound, common for all items in the sequence: shape (wave form and envelope),
 * modulation, tempo (delay between notes), duration (length of one note).
 */
class Controls {

  private:

    int shapePin, modulationPin, tempoPin, durationPin;

  public:

    int wave = 0;
    int envelope = 0;
    int mod = 64;
    int delay = 100;
    int length = 50;

    Controls() {
      // noop
    }

    void setupPotPins(int shapePin, int modulationPin, int tempoPin, int durationPin) {
      this->shapePin = shapePin;
      this->modulationPin = modulationPin;
      this->tempoPin = tempoPin;
      this->durationPin = durationPin;
      pinMode(shapePin, INPUT);
      pinMode(modulationPin, INPUT);
      pinMode(tempoPin, INPUT);
      pinMode(durationPin, INPUT);
    }

    void read() {
      wave = map(analogRead(shapePin), 0, 1023, 0, 4);
      mod = map(analogRead(modulationPin), 0, 1023, 0, 127);
      delay = map(analogRead(tempoPin), 0, 1023, 200, 50);
      length = map(analogRead(durationPin), 0, 1023, 0, 127);
    }

};