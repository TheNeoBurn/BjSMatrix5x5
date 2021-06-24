// Set the serial pin
#define SERIAL_PIN 5
// Set the clock pin
#define CLOCK_PIN 6
// Set the latch pin
#define LATCH_PIN 7
// Set the push-button pin
#define BUTTON_PIN 3
// Set the time in ms over that a button press is long
#define BUTTON_LONG_DURATION 1000

// This Library must have been added to the Arduino IDE!!!
#include <BjSMatrix5x5.h>

// An instance of an shift register 5x5 pixel display
BjSMatrix5x5 matrix = BjSMatrix5x5(SERIAL_PIN, CLOCK_PIN, LATCH_PIN);

// Current display mode (0=>text, 1=>heart, 2=>random)
// Thin will switch on start because we use a pullup-resistor button detection
byte mode = 2;
// Current display orientation (0=>original, 1=>90°counter-clockwise, 2=>180°, 3=>90°clockwise)
byte orientation = 0;
// Last button down time (used only by onButtonChange interrupt handler)
unsigned long btnStart = 0;
// Flag if the button was pressed shortly (set by onButtonChange interrupt handler and reset in loop())
volatile bool btnPressedShort = false;
// Flag if the button was pressed longer (set by onButtonChange interrupt handler and reset in loop())
volatile bool btnPressedLong = false;

// Bitmaps for a pulsing heart
const byte BMP_HEART1[5] = { 0b01010, 0b11111, 0b11111, 0b01110, 0b00100 };
const byte BMP_HEART2[5] = { 0b00000, 0b01010, 0b01110, 0b00100, 0b00000 };
const byte BMP_HEART3[5] = { 0b00000, 0b00000, 0b00100, 0b00000, 0b00000 };
byte heart = 3;

/************************************
 * Interrupt handler for the button *
 ************************************/
void onButtonChange() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Safe time of button press
    btnStart = millis();
  } else {
    // Set button flag short or long via duration on button release
    int duration = millis() - btnStart;
    if (duration > BUTTON_LONG_DURATION) {
      btnPressedLong = true;
    } else {
      btnPressedShort = true;
    }
  }
}

void setup() {
  // Get a random seed by reading floating analog pin 0
  randomSeed(analogRead(0));

  // Set the button pin as input with a pullup resistor (button press will be LOW)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Set the text to display on matrix.scroll() (is not displayed yet)
  matrix.text("Hello there!");
  // Clear the display LEDs
  matrix.off();

  // Register interrupt to handle button presses
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonChange, CHANGE); 
}


void loop() {
  /*********************************************************
   * Switch to next mode if the button was pressed shortly *
   *********************************************************/
  if (btnPressedShort) {
    // Reset the flag
    btnPressedShort = false;
    // Switch to the next mode ('% 3' means that the value gets back to 0 when 3 is reached) 
    mode = (mode + 1) % 3;
  }

  /***********************************************************************
   * Switch to next display orientation if the button was pressed longer *
   ***********************************************************************/
  if (btnPressedLong) {
    // Reset the flag
    btnPressedLong = false;
    // Switch to the next display orientation ('% 4' see above)
    orientation = (orientation + 1) % 4;
    // Set the orientation in the display instance
    matrix.orientation(orientation); 
  }

  /****************************************************
   * Show the next step depending on the current mode *
   ****************************************************/
  switch (mode) {
    /****************************************
     * mode 0: Scroll text (set in setup()) *
     ****************************************/
    case 0:
      // Clear the display shortly to avoid wrong pixels for unstable cable connection to the display
      // Can be removed if the display connection is soldered
      matrix.off();
      // Shift the text position (effects only the buffer), will automatically start again when the end is reached
      matrix.scroll();
      // Send the buffer to the display
      matrix.show();
      // Wait 200ms until the next text shift
      delay(200);
      break;

    /********************************
     * mode 1: Show a pulsing heart *
     ********************************/
    case 1:
      // Clear the display shortly to avoid wrong pixels for unstable cable connection to the display
      // Can be removed if the display connection is soldered
      matrix.off();
      // Set the heart bitmaps for the current step to the display buffer
      switch(heart) {
        case 0:
        case 1:
        case 2:
        case 8:
          matrix.bmp(BMP_HEART1);
          break;
        case 3:
        case 4:
        case 6:
        case 7:
          matrix.bmp(BMP_HEART2);
          break;
        case 5:
          matrix.bmp(BMP_HEART3);
          break;
      }
      // Send the buffer to the display
      matrix.show();
      // Switch to the next heart step (wrap around at heart = 9 to 0)
      heart = (heart + 1) % 9;
      // Wait 100ms until the next step
      delay(100);
      break;

    /********************************
     * mode 2: Toggle random pixels *
     ********************************/
    case 2:
      // Toggle a single random pixel of the display
      matrix.toggle(random(5), random(5));
      // Send the buffer to the display
      matrix.show();
      // Wait 50ms until the next step
      delay(50);
      break;
  }
}
