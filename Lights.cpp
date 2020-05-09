#include <Adafruit_NeoPixel.h>

/**
 * Visualisation lights that show the pitch and currently played note.
 * Pixels 0 and 1 are under the speaker and represent current note only.
 * Pixels 2..9 are above the speaker, and represent whole sequence (colNr+2).
 */
class Lights {

  private:

    Adafruit_NeoPixel strip;
    long blinkStarted = 0;
    int blinkedPixel = -1;
    uint32_t beforeBlinkColor;

  public:

    Lights() {
      // noop
    }

    void setupStripPin(int stripPin) {
      strip = Adafruit_NeoPixel(10, stripPin, NEO_GRB + NEO_KHZ800);
      strip.begin();
      for (int i = 0; i < 10; i++) strip.setPixelColor(i, white(1));
      strip.show();
    }

    void show() {
      if (millis() - blinkStarted > 60) {
        blinkFinish();
        strip.show();
      }
    }

    void change(int colNr, boolean enabled, int pitch) {
      byte value = map(pitch, 0, 127, 0, 225); // pitch is 0..127, color value 0..255
      blinkFinish();
      if (enabled) {
        strip.setPixelColor(0, color(value, 12));
        strip.setPixelColor(1, color(value, 12));
        strip.setPixelColor(colNr+2, color(value, 15));
      } else {
        strip.setPixelColor(0, white(1));
        strip.setPixelColor(1, white(1));
        strip.setPixelColor(colNr+2, white(1));
      }
      blinkStart(colNr+2);
      strip.show();
    }

    void blinkStart(int pixel) {
      beforeBlinkColor = strip.getPixelColor(pixel);
      blinkStarted = millis();
      strip.setPixelColor(pixel, white(30));
      blinkedPixel = pixel;
    }

    void blinkFinish() {
      if (blinkedPixel != -1) {
        blinkStarted = 0;
        strip.setPixelColor(blinkedPixel, beforeBlinkColor);
        blinkedPixel = -1;
      }
    }

    uint32_t white(byte brightness) {
      return strip.Color(brightness, brightness, brightness);
    }

    uint32_t color(byte value, byte brightness) {
      uint16_t r, g, b;
      value = 255 - value;
      if (value < 85) {
        r = 255 - value*3;
        g = 0;
        b = value*3;
      }
      else if (value < 170) {
        value -= 85;
        r = 0;
        g = value*3;
        b = 255 - value*3;
      }
      else {
        value -= 170;
        r = value*3;
        g = 255 - value*3;
        b = 0;
      }
      r = r * brightness / 255;
      g = g * brightness / 255;
      b = b * brightness / 255;
      return strip.Color(r, g, b);
    }
};
