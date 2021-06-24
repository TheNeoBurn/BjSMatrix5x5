#include <Arduino.h>

// Set the orientation of the display: 0=>original, 1=>90°counter-clockwise, 2=>180°, 3=>90°clockwise, 999=>variable (needs more RAM)
#define BjSMatrixRotate 999

/*!
 * BjSMatrix5x5 LED display
 */
class BjSMatrix5x5 {
  public:
    /*!
     * @brief Create a new instance of BjSMatrix5x5 LED display
     * A driver for the BjSMatrix5x5 LED display using shift registers to show a 5*5 bitmap using 3 pins
     * @param dataPin Data pin
     * @param clockPin Clock pin
     * @param latchPin Latch pin
     */
    BjSMatrix5x5(int dataPin, int clockPin, int latchPin);
	/*!
	 * @brief Reset the buffer to all off
	 */
	void clear();
	/*!
	 * @brief Send the buffer to the display
	 */
	void show();
    /*!
     * @brief Turns the display off
     * Turns all LEDs off
     */	
    void off();
    /*!
     * @brief Returns a pixel state
     * Returns the buffer value for a pixel from the buffer
     * @param x x-coordinate (0-4)
     * @param y y-coordinate (0-4)
     */
    bool get(byte x, byte y);
    /*!
     * @brief Sets a pixel state
     * Sets the buffer value for a pixel in the buffer
     * @param x x-coordinate (0-4)
     * @param y y-coordinate (0-4)
	 * @param on The state of the LED (true|false)
     */
    void set(byte x, byte y, byte on);
    /*!
     * @brief Toggles a pixel state
     * Toggles the buffer value for a pixel in the buffer
     * @param x x-coordinate (0-4)
     * @param y y-coordinate (0-4)
     */
    void toggle(byte x, byte y);
    /*!
     * @brief Set a bitmap
     * Sets all buffer values from a given bitmap
     * @param lines An 5-element array of bytes, each byte containing one line in the least-significant bits (where 0b10000 is left and 0b00001 is right)
     */
    void bmp(byte* lines);
	/*!
     * @brief Sets the text for scrolling
     * Sets the text for scrolling using an internal 3x5 charset and sets len() to the width in pixels
     * @param text The text to display
     */
    void text(char* text);
	/*!
     * @brief Returns the text width in pixels for scrolling
     * Returns the text width in pixels for scrolling
     */	
    int len();
	/*!
     * @brief Sets the text scroll position and re-draws
     * Sets the text scroll position, calculates the buffer image and re-draws the display
     * @param pos The left most pixel of the text (can be negative of larger then the text width)
     */	
    void textpos(int pos);
    /*!
     * @brief Scrolls the text by one pixel and resets if the end is reached
     * Scrolls the text by one pixel and resets if the end is reached
     */
    bool scroll();
#if BjSMatrixRotate == 999
	/*!
	 * @brief Set the display orientation
	 * Sets the display orientation
	 * @param o The orientation to set (0=>original, 1=>90°counter-clockwise, 2=>180°, 3=>90°clockwise)
	 */
	void orientation(byte o);
#endif
  private:
    /*!
     * @brief The data pin to send serial data
     */
    int _dataPin;
    /*!
     * @brief The clock pin to clock the serial data
     */
    int _clockPin;
    /*!
     * @brief The latch pin to commit the new data
     */
    int _latchPin;
    /*!
     * @brief The internal buffer containing the shift register bits (differs from the bitmap!!!)
     */
    byte _buffer[4] = { 0, 0, 0, 0 };
    /*!
     * @brief The text to scroll
     */
    char* _text;
    /*!
     * @brief The width in pixels of the current text to scroll
     */
    int _len = 0;
    /*!
     * @brief The last scroll position
     */
    int _pos = -4;
#if BjSMatrixRotate == 999
	/*!
	 * @brief The current display orientation (0=>original, 1=>90°counter-clockwise, 2=>180°, 3=>90°clockwise)
	 */
	byte _orientation = 0;
#endif
    /*!
     * @brief Get the bits for a char, format: 15 lest-significnat bits, left-right 3 bis times 5 lines
     */
    static int getChar(char c);
};
