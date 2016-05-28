// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Battery.h>
#include <Timer.h>
#include <SerialCommand.h>
#include "BlinkTimerControl.h"

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

//-----------------------------------------------------------------------------

void toggleLed(int ledPin)
{
  bool isLedOn = digitalRead(ledPin);
  digitalWrite(ledPin, !isLedOn);
}

// global variable definition
BlinkTimerControl* blinkTimerControl = 0;

// global constant definition
const unsigned int BLINK_TIME_MILLIS = 200;

class BlinkTimerAdapter : public TimerAdapter
{
public:
  void timeExpired()
  {
    toggleLed(LED_BUILTIN);

    if (0 != blinkTimerControl)
    {
      blinkTimerControl->timeExpired();
    }
  }
};

//-----------------------------------------------------------------------------
// Arduino Cmd I/F
//-----------------------------------------------------------------------------
SerialCommand* sCmd = 0;

void decr()
{
  char *arg;
  if (0 != sCmd)
  {
    arg = sCmd->next();    // Get the next argument from the SerialCommand object buffer
  }
  else
  {
    arg = const_cast<char*>("");
  }
  if (0 != blinkTimerControl)
  {
    if (arg != NULL)         // As long as it exists, take it
    {
      int n = atoi(arg);
      while (n > 0)
      {
        n--;
        blinkTimerControl->decrementBlinkTime();
      }
    }
    else
    {
      blinkTimerControl->decrementBlinkTime();
    }
  }
}

void incr()
{
  char *arg;
  if (0 != sCmd)
  {
    arg = sCmd->next();    // Get the next argument from the SerialCommand object buffer
  }
  else
  {
    arg = const_cast<char*>("");
  }
  if (0 != blinkTimerControl)
  {
    if (arg != NULL)         // As long as it exists, take it
    {
      int n = atoi(arg);
      while (n > 0)
      {
        n--;
        blinkTimerControl->incrementBlinkTime();
      }
    }
    else
    {
      blinkTimerControl->incrementBlinkTime();
    }
  }
}

// This is the default handler, and gets called when no other command matches.
void unrecognized(const char *command)
{
  Serial.println("What?");
}

//-----------------------------------------------------------------------------
// Battery Voltage Surveillance
//-----------------------------------------------------------------------------
class MyBatteryAdapter : public BatteryAdapter
{
public:
  float readBattVoltageSenseFactor()
  {
    return 9.239;
  }
  unsigned int readRawBattSenseValue()
  {
    unsigned int rawBattSenseValue = analogRead(0);
    return rawBattSenseValue;
  }
};

Battery* battery = 0;
MyBatteryAdapter* batteryAdapter = 0;

//-----------------------------------------------------------------------------
// Neo Matrix
//-----------------------------------------------------------------------------
#define PIN 12

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN,
NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
NEO_GRB + NEO_KHZ800);

const uint16_t colors[] =
{ matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

int x = matrix.width();
int pass = 0;
int counter = 1;

class LoopTimerAdapter : public TimerAdapter
{
public:
  void timeExpired()
  {
    matrix.fillScreen(0);

    matrix.fillCircle(6, 6, 6, matrix.Color(190, 190, 190));
    matrix.setTextColor(colors[0]);
    matrix.setCursor(0, 1);
    matrix.print(F("158"));

    if (counter % 2 == 0)
    {
      matrix.fillScreen(0);
      matrix.fillCircle(6, 5, 5, matrix.Color(20, 20, 20));

      matrix.setTextColor(colors[2]);
      matrix.setCursor(0, 1);
      matrix.print(String(counter));
    }

    matrix.show();
    counter++;
  }
};

//-----------------------------------------------------------------------------

void setup()
{
  //---------------------------------------------------------------------------
  // configure LED pin
  //---------------------------------------------------------------------------
  pinMode(LED_BUILTIN, OUTPUT);

  //---------------------------------------------------------------------------
  // setup blink timer control
  //---------------------------------------------------------------------------
  BlinkTimerAdapter* blinkTimerAdapter = new BlinkTimerAdapter();
  blinkTimerControl = new BlinkTimerControl(blinkTimerAdapter, BLINK_TIME_MILLIS);

  //-----------------------------------------------------------------------------
  // Serial Command Object for Debug CLI
  //-----------------------------------------------------------------------------
  Serial.begin(115200);
  sCmd = new SerialCommand();

  // Setup callbacks for SerialCommand commands
  if (0 != sCmd)
  {
    sCmd->addCommand("i", incr);
    sCmd->addCommand("d", decr);
    sCmd->setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  }
  Serial.println("Hello from Neo Matrix Test!\n");

  //-----------------------------------------------------------------------------
  // Battery Voltage Surveillance
  //-----------------------------------------------------------------------------
  batteryAdapter = new MyBatteryAdapter();
  battery = new Battery(batteryAdapter);

  //-----------------------------------------------------------------------------
  // Neo Matrix
  //-----------------------------------------------------------------------------
  new Timer(new LoopTimerAdapter(), Timer::IS_RECURRING, 1000);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(10);
  matrix.setTextColor(colors[0]);
  matrix.setTextSize(1);
}

void loop()
{
  if (0 != sCmd)
  {
    sCmd->readSerial();                       // process serial commands
  }
  yield();
}
