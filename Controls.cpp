#include <stdarg.h>
#include <stdio.h>
#include "Arduino.h"

class Controls {

  private:

    int encoderCounter = 0;
    boolean lastEncoderA;

  public:

    int delay = 100;
    int length = 50;
    int mod = 64;
    int wave = 0;
    int envelope = 0;

    Controls() {
        pinMode(A0, INPUT);
        pinMode(A1, INPUT);
        pinMode(A2, INPUT);
        pinMode(A3, INPUT_PULLUP);
        pinMode(A4, INPUT_PULLUP);
        lastEncoderA = digitalRead(A3);
    }

    void read() {
      delay = map(analogRead(A0), 0, 1023, 200, 50);
      length = map(analogRead(A1), 0, 1023, 0, 127);
      mod = map(analogRead(A2), 0, 1023, 0, 127);

      boolean encoderA = digitalRead(A3);
      boolean encoderB = digitalRead(A4);

      if (encoderA != lastEncoderA) {
          lastEncoderA = encoderA;
          if (encoderA == LOW) {
            encoderCounter += (encoderB == HIGH ? +1 : -1);
            int counterMax = 6 * 4 - 1; // six waves, four envelopes
            if (encoderCounter < 0) encoderCounter = counterMax;
            if (encoderCounter > counterMax) encoderCounter = 0;
            wave = encoderCounter / 4;
            envelope = encoderCounter - (wave * 4);
        }
      }
    }

};