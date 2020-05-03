#include <stdarg.h>
#include <stdio.h>
#include "Arduino.h"
#include <RunningMedian.h>

/**
 * Column of controls of a single item from the sequence: a pitch potentiometer, an on/off button
 * and a status led.
 */
struct Column {

  int pitch; // pitch as set on potentiometer (pot value scaled to 0-127 range)
  int potValue = 0; // current potentiometer value (smoothed from last couple of values)
  RunningMedian potValues = RunningMedian(21); // last couple of potentiometer values

  boolean enabled = true; // flip-flop controlled by button
  boolean buttonState = LOW; // current button state
  long buttonChanged = 0; // timestamp of last state change

};

/**
 * Whole set of columns, connected to arduino with a set of wires:
 * - three digital output pins to multiplexer, to select one of the columns
 * - common analog input pin from potentiometers
 * - common digital input pin from buttons
 */
class Columns {

  private:
  
    Column *columns;
    const int numOfColumns;

    int muxInhibitPin;
    int muxPinA, muxPinB, muxPinC;

    int highlightedColNr;
    long highlightStarted = 0;

    int pitchPin;
    int togglePin;

    const int MAX_POT_VALUE = 550; // due to voltage drop on mux, it won't be the usual 1024
    const int MAX_PITCH = 127; // max possible pitch in our synthesizer

    long lastRefresh = 0;

  public:

    Columns(int numOfColumns) : numOfColumns(numOfColumns) {
      columns = new Column[numOfColumns];
    }
    
    void setupMuxPins(int inhibitPin, int pinA, int pinB, int pinC) {
      this->muxInhibitPin = inhibitPin;
      this->muxPinA = pinA;
      this->muxPinB = pinB;
      this->muxPinC = pinC;
      pinMode(inhibitPin, OUTPUT);
      pinMode(pinA, OUTPUT);
      pinMode(pinB, OUTPUT);
      pinMode(pinC, OUTPUT);
    }

    void setupInputPins(int pitchPin, int togglePin) {
      this->pitchPin = pitchPin;
      this->togglePin = togglePin;
      pinMode(pitchPin, INPUT);
      pinMode(togglePin, INPUT);
    }
    
    Column &operator[](int colNr) {
      highlight(colNr);
      return columns[colNr];
    }

    void select(int colNr) {
      digitalWrite(muxInhibitPin, HIGH);
      digitalWrite(muxPinA, bitRead(colNr, 0));
      digitalWrite(muxPinB, bitRead(colNr, 1));
      digitalWrite(muxPinC, bitRead(colNr, 2));
      digitalWrite(muxInhibitPin, LOW);
    }

    void deselect() {
      digitalWrite(muxInhibitPin, HIGH);
    }

    void highlight(int colNr) {
      highlightStarted = millis();
      highlightedColNr = colNr;
    }

    void read() {
      // do not refresh too often as this highlights all leds temporarily
      boolean needToRefresh = millis() - lastRefresh > 25;
      if (needToRefresh) {
        lastRefresh = millis();
        for (int colNr = 0; colNr < numOfColumns; colNr++) {
          select(colNr);
          read(columns[colNr]);
        }
      }
      // update led highlighting to show what has been recently played
      boolean highlightStillOn = millis() - highlightStarted <= 75;
      if (columns[highlightedColNr].enabled && highlightStillOn) {
        select(highlightedColNr);
      } else {
        deselect();
      }
    }

    void read(Column &column) {
      // smooth pot value to cancel noise, then calculate pitch
      column.potValues.add(analogRead(pitchPin));
      column.potValues.add(analogRead(pitchPin));
      column.potValues.add(analogRead(pitchPin));
      int readPotValue = column.potValues.getMedian();
      int minimumMeaningfulDifference = MAX_POT_VALUE / MAX_PITCH;
      if (abs(column.potValue - readPotValue) >= minimumMeaningfulDifference) {
        column.potValue = readPotValue;
        column.pitch = map(readPotValue, 0, MAX_POT_VALUE, 0, MAX_PITCH);
        column.pitch = MAX_PITCH - column.pitch; // inverse
      }
      // debounce button, then establish column enabled state
      int readButtonState = digitalRead(togglePin);
      boolean changedRecently = millis() - column.buttonChanged <= 50;
      if (column.buttonState != readButtonState && !changedRecently) {
        column.buttonState = readButtonState;
        column.buttonChanged = millis();
        if (readButtonState == HIGH) {
          column.enabled = !column.enabled;
        }
      }
    }

};
