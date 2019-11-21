#include <stdarg.h>
#include <stdio.h>
#include "Arduino.h"
#include "MedianFilter.h"

const int MAX_POT_VALUE = 650; // due to voltage drop on mux, it won't be the usual 1024
const int MAX_PITCH = 127; // max possible pitch in our synthesizer

/**
 * Column of controls of a single item from the sequence: a pitch potentiometer, an on/off button and a status led.
 */
struct Column {
  int selectPin;
  
  int pitch; // pitch as set on potentiometer (pot value scaled to 0-127 range)
  int potValue = 0; // current potentiometer value (smoothed from last couple of values)
  MedianFilter potValues = MedianFilter(21, 0); // last couple of potentiometer values

  boolean enabled = true; // flip-flop controlled by button
  boolean buttonState = LOW; // current button state
  long buttonChanged = 0; // timestamp of last state change
};

/**
 * Whole set of columns, connected to arduino with a set of wires:
 * - three digital output pins to multiplexer, to select one of the columns
 * - common potentiometer analog input pin
 * - common button digital input pin
 */
class Columns {

  private:
  
    Column *columns;
    const int numOfColumns;
    const int potPin;
    const int buttonPin;
    int highlightedNr;

  public:

    Columns(int numOfColumns, int potPin, int buttonPin) : numOfColumns(numOfColumns), potPin(potPin), buttonPin(buttonPin) {
      columns = new Column[numOfColumns];
      pinMode(potPin, INPUT);
      pinMode(buttonPin, INPUT);
    }

    void setupSelectPins(...) {
      va_list pins;
      va_start(pins, numOfColumns);
      for (int colNr = 0; colNr < numOfColumns; colNr++) {
        columns[colNr].selectPin = va_arg(pins, int);
        pinMode(columns[colNr].selectPin, OUTPUT);
      }
      va_end(pins);
    }

    Column &operator[](int colNr) {
      return columns[colNr];
    }

    void highlight(int colNr) {
      digitalWrite(columns[highlightedNr].selectPin, LOW);
      highlightedNr = colNr;
      //digitalWrite(columns[highlightedNr].selectPin, HIGH); todo: if we want short flash to denote current column
    }

    void read() {
      for (int i = 0; i < numOfColumns; i++) {
        read(columns[i]);
      }
    }

    void read(Column &column) {
      // select the column which pot value and button state will be read
      digitalWrite(columns[highlightedNr].selectPin, LOW);
      digitalWrite(column.selectPin, HIGH);

      // smooth pot value to cancel noise, then calculate pitch
      column.potValues.in(analogRead(potPin));
      column.potValues.in(analogRead(potPin));
      column.potValues.in(analogRead(potPin));
      int readPotValue = column.potValues.getMean();
      int minimumMeaningfulDifference = MAX_POT_VALUE / MAX_PITCH;
      if (abs(column.potValue - readPotValue) >= minimumMeaningfulDifference) {
        column.potValue = readPotValue;
        column.pitch = map(readPotValue, 0, MAX_POT_VALUE, 0, MAX_PITCH);
      }

      // debounce button, then establish column enabled state
      int readButtonState = digitalRead(buttonPin);
      boolean changedRecently = millis() - column.buttonChanged <= 50;
      if (column.buttonState != readButtonState && !changedRecently) {
        column.buttonState = readButtonState;
        column.buttonChanged = millis();
        if (readButtonState == HIGH) {
          column.enabled = !column.enabled;
        }
      }

      // select the column which is highlighed, to turn led back on
      digitalWrite(column.selectPin, LOW);
      digitalWrite(columns[highlightedNr].selectPin, columns[highlightedNr].enabled);
    }

};
