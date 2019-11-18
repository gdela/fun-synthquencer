#include "Arduino.h"
#include "MedianFilter.h"

/**
 * Column of controls of a single item from the sequence: a pitch potentiometer, an on/off button and a status led.
 */
struct Column {
  int selectPin;
  int potValue = 0; // current potentiometer value (smoothed from last ten)
  MedianFilter potValues = MedianFilter(10, 0); // last ten potentiometer values
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
  
    const int numOfColumns;
    const int potPin;
    const int buttonPin;
    Column *columns;

  public:

    Columns(int numOfColumns, int potPin, int buttonPin) : numOfColumns(numOfColumns), potPin(potPin), buttonPin(buttonPin) {
      columns = new Column[numOfColumns];
      pinMode(potPin, INPUT);
      pinMode(buttonPin, INPUT);
    };

    Column &operator[](int colNr) {
      return columns[colNr];
    };

    void read() {
      for (int i = 0; i < numOfColumns; i++) {
        read(columns[i]);
      }
    }

    void read(Column &column) {
      digitalWrite(column.selectPin, HIGH);

      int readPotValue = analogRead(potPin);
      column.potValues.in(readPotValue); // smooth using median from last n-values
      column.potValue = column.potValues.out();

      int readButtonState = digitalRead(buttonPin);
      if (column.buttonState != readButtonState && (column.buttonChanged < (millis() - 50))) { // debounce
        column.buttonState = readButtonState;
        column.buttonChanged = millis();
        if (readButtonState == HIGH) {
          column.enabled = !column.enabled;
        }
      }

      digitalWrite(column.selectPin, LOW);
    }

};
