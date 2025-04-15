#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "arduinoFFT.h"
#include "Icons.h"
#ifndef PSTR
  #define PSTR // Make Arduino Due happy
#endif


// Color definitions
// http://www.rinkydinkelectronics.com/calc_rgb565.php to calculate your own
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

//change colors or add more for more expressions x3
#define PCOLOR          0x6012
#define HAPPYCOLOR      0x0660
#define SURPRISEDCOLOR  0x006c
#define ANGRYCOLOR      RED
#define VWVCOLOR        0x601F

// array to store colors for blinkstate. When adding expressions, colors, also increase the colors[number]
int colors[4] = {HAPPYCOLOR, SURPRISEDCOLOR, ANGRYCOLOR, VWVCOLOR};

//#define VisorSensor
#define BoopSensor
//#define Microphone

const int DIN = 6;
const int matrixWidth = 8;
const int matrixHeight = 8;
const int tilesX = 1;
const int tilesY = 14;


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
  matrixWidth, matrixHeight, tilesX, tilesY, DIN,
  NEO_TILE_TOP + NEO_TILE_RIGHT +
  NEO_TILE_ROWS + NEO_TILE_PROGRESSIVE +
  NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB + NEO_KHZ800);


int expressions = 4; // Here you can change the amount of expressions you want to use

/////////////////////////// I/O Pins \\\\\\\\\\\\\\\\\\\\\\\\\\\

const int ButtonPin = 2;
const int VisorPin = 3;
const int irPin = A7; // Analog input for infrared or 'boop' sensor
const int micPin = A0;  // Analog input for microphone

//////////////////////////////// Icon Posistions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

signed int EyeLeft = 96; // Horizontal positions off the icons on the screens
signed int MawLeft = 64;
signed int NoseLeft = 56;
signed int NoseRight = 48;
signed int MawRight = 16;
signed int EyeRight = 0;

//////////////////////////////// Variables \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

volatile unsigned long glitchDelay = 0;
volatile unsigned long glitchSpeed = 0;
volatile unsigned long blinkTime = 0;
volatile unsigned long blinkSpeed = 0;
volatile unsigned long boopTime = 0;
volatile unsigned long speakingDelay = 0;
volatile unsigned long debounceTime = 0;
volatile unsigned long debounceTime2 = 0;
volatile unsigned long debounceTime3 = 0;
volatile unsigned long previousMillis = 0;


bool visorState = 0;
bool micState = 0;
bool boopState = 0;
bool boopTimer = 0;
bool boopActive = 0;
bool speakingActive = 0;
bool boopDisabled = 0;

bool rising = 1;
bool ledstate = 0;
byte state = 0;
int timer = 1;
int y;
int x;
int Step = 0;
int counter = 0;
const unsigned long ledTime = 1000;

//Blink variables
unsigned long blinkDelay = 9000;
bool blinkState = 0;


//Smoothing Microphone
int sensitivity = 65; // Change this value if your microphone is too sensitive, or not sensitive enough
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
float filterDCComponent = 510;

//FFT variables
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
#define CHANNEL micPin
const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 4000; //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[samples];
double vImag[samples];
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

///////////////////////////Setup runs once\\\\\\\\\\\\\\\\\\\\\\\\\\\

void setup() {
  matrix.begin();
  matrix.setRotation(3);

  expressions--;

  pinMode(ButtonPin, OUTPUT);
  pinMode(VisorPin, INPUT);
  pinMode(irPin, INPUT);
  pinMode(micPin, INPUT);
  digitalWrite(irPin, LOW);
  digitalWrite(micPin, LOW);
  digitalWrite(ButtonPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(ButtonPin), ISR_button, FALLING);

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));

  #ifdef VisorSensor
  visorState = 1;
  #endif
  #ifdef Microphone
  micState = 1;
  #endif
  #ifndef BoopSensor
  boopDisabled = 1;
  #endif
}

///////////////////////////Actual code begins here\\\\\\\\\\\\\\\\\\\\\\\\\\\

void loop() {
  if ((digitalRead(VisorPin) == 0) || (visorState == 0)) {
    Serial.println(analogRead(irPin));
    if ((analogRead(irPin) > 500)||(boopDisabled == 1)) { //Change the '>' to '<' if the boop detection works the wrong way round
      boopState = 0;
      boopTimer = 1;
    }
    else {
      boopState = 1;
    }
    if ((boopState * boopTimer) != 1) {
      boopActive = 0;
      boopTime = millis();
      if (rising == 1) {
        matrix.fillScreen(0);
        matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, PCOLOR);
        matrix.drawBitmap(NoseLeft, 0, noseL, 8, 8, PCOLOR);
        matrix.drawBitmap(NoseRight, 0, nose, 8, 8, PCOLOR);
        matrix.drawBitmap(MawRight, 0, maw, 32, 8, PCOLOR);
        switch (state) {
          case 0:                             //First button press: Happy expression
            matrix.drawBitmap(EyeLeft, 0, EyeL, 16, 8, HAPPYCOLOR);
            matrix.drawBitmap(EyeRight, 0, Eye, 16, 8, HAPPYCOLOR);
            matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, HAPPYCOLOR);
            matrix.drawBitmap(NoseLeft, 0, noseL, 8, 8, HAPPYCOLOR);
            matrix.drawBitmap(NoseRight, 0, nose, 8, 8, HAPPYCOLOR);
            matrix.drawBitmap(MawRight, 0, maw, 32, 8, HAPPYCOLOR);
            rising = 0;
            previousMillis = millis();
            break;

          case 1:                             //Second button press: Surprised
            matrix.drawBitmap(EyeLeft, 0, SpookedL, 16, 8, SURPRISEDCOLOR);
            matrix.drawBitmap(EyeRight, 0, Spooked, 16, 8, SURPRISEDCOLOR);
            matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, SURPRISEDCOLOR);
            matrix.drawBitmap(NoseLeft, 0, noseL, 8, 8, SURPRISEDCOLOR);
            matrix.drawBitmap(NoseRight, 0, nose, 8, 8, SURPRISEDCOLOR);
            matrix.drawBitmap(MawRight, 0, maw, 32, 8, SURPRISEDCOLOR);
            rising = 0;
            previousMillis = millis();
            break;

          case 2:                             //Third button press: Angry expression
            matrix.drawBitmap(EyeLeft, 0, AngryL, 16, 8, ANGRYCOLOR);
            matrix.drawBitmap(EyeRight, 0, Angry, 16, 8, ANGRYCOLOR);
            matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, ANGRYCOLOR);
            matrix.drawBitmap(NoseLeft, 0, noseL, 8, 8, ANGRYCOLOR);
            matrix.drawBitmap(NoseRight, 0, nose, 8, 8, ANGRYCOLOR);
            matrix.drawBitmap(MawRight, 0, maw, 32, 8, ANGRYCOLOR);
            rising = 0;
            previousMillis = millis();
            break;

          case 3:
            matrix.drawBitmap(EyeLeft, 0, vwvL, 16, 8, VWVCOLOR);
            matrix.drawBitmap(EyeRight, 0, vwv, 16, 8, VWVCOLOR);
            matrix.drawBitmap(MawLeft, 0, mawL, 32, 8, VWVCOLOR);
            matrix.drawBitmap(NoseLeft, 0, noseL, 8, 8, VWVCOLOR);
            matrix.drawBitmap(NoseRight, 0, nose, 8, 8, VWVCOLOR);
            matrix.drawBitmap(MawRight, 0, maw, 32, 8, VWVCOLOR);
            rising = 0;
            previousMillis = millis();
            break;
        }
        blinkState = 0;
      }
      glitch();
      Blink();
      microphone();
      matrix.show();
      if (millis() - previousMillis >= ledTime) {
        ledstate = 0;
      }
    }
    else {
      if ((millis() - boopTime) < 6000) {
        blinkTime = millis();
        if (boopActive == 0) {
          #define color colors[state]
          matrix.fillRect(EyeRight, 0, 16, 8, 0);
          matrix.fillRect(EyeLeft, 0, 16, 8, 0);
          matrix.drawBitmap(EyeLeft, 0, vwvL, 16, 8, color);
          matrix.drawBitmap(EyeRight, 0, vwv, 16, 8, color);
          matrix.Color(16, 4, 0);
          boopActive = 1;
          rising = 1;
        }
      }
      else {
        boopTimer = 0;
      }
    }
    matrix.show();
  }
  else {
    matrix.fillScreen(0);
    matrix.show();
  }
}

///////////////////////////Button interrupt\\\\\\\\\\\\\\\\\\\\\\\\\\\

void ISR_button() {                               //Stuff you shouldn't touch :P
  if ((millis() - debounceTime) > 150) {
    if (state < expressions) {
      state++;
    }
    else {
      state = 0;
    }
    rising = 1;
    debounceTime = millis();
  }
}