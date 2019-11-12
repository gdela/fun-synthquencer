#include "Arduino.h"
#include "MedianFilter.h"

struct Column {
  int selectPin;
  int potValue = 0; // current potentiometer value (smoothed from last ten)
  MedianFilter potValues = MedianFilter(10, 0); // last ten potentiometer values
  boolean buttonState = LOW; // current button state
  long buttonChanged = 0; // timestamp of last state change
  boolean enabled = true; // flip-flop controlled by button
};

class Columns {

  private:
  
    const int numOfColumns;
    const int potPin;
    const int buttonPin;
    Column *columns;

  public:

    Columns(int numOfColumns, int potPin, int buttonPin) : numOfColumns(numOfColumns), potPin(potPin), buttonPin(buttonPin) {
      columns = new Column[numOfColumns];
    };

    private:

    Column &operator[](int colNr) {
      return columns[colNr];
    };

};
