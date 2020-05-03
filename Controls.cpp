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

    const int MIN_POT_VAL = 0;
    const int MAX_POT_VAL = 1023;

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
      int numOfWaves = 6;
      int numOfEnvelopes = 4;
      int shape = readPot(shapePin, 0, numOfWaves * numOfEnvelopes - 1);
      wave = shape / numOfEnvelopes; // range 0-5
      envelope = shape - (wave * numOfEnvelopes); // range 0-4

      mod = readPot(modulationPin, 16, 112); // mod range is 0-127, no mod 64
      if (mod >= 62 && mod <= 66) mod = 64; // make it easier to select no modulation
      
      delay = readPot(tempoPin, 300, 30); // delay range is 0 up to hundrets milliseconds
      length = readPot(durationPin, 30, 90); // synth's length range is 0-127
    }

    int readPot(int pin, int minOutVal, int maxOutVal) {
      return map(analogRead(pin), MAX_POT_VAL, MIN_POT_VAL, minOutVal, maxOutVal);
    }
};