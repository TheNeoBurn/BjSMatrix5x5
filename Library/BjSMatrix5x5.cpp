#include <Arduino.h>
#include <BjSMatrix5x5.h>

BjSMatrix5x5::BjSMatrix5x5(int dataPin, int clockPin, int latchPin) {
	_dataPin = dataPin;
	_clockPin = clockPin;
	_latchPin = latchPin;
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	digitalWrite(dataPin, LOW);
	digitalWrite(clockPin, LOW);
	digitalWrite(latchPin, LOW);
	_buffer[0] = _buffer[1] = _buffer[2] = _buffer[3] = _buffer[4] = 0;
#if BjSMatrixRotate == 999
	_orientation = 0;
#endif
}

void BjSMatrix5x5::clear() {
	_buffer[0] = _buffer[1] = _buffer[2] = _buffer[3] = 0;
}

void BjSMatrix5x5::show() {
	digitalWrite(_latchPin, LOW);
	shiftOut(_dataPin, _clockPin, LSBFIRST, _buffer[3]);
	shiftOut(_dataPin, _clockPin, LSBFIRST, _buffer[2]);
	shiftOut(_dataPin, _clockPin, LSBFIRST, _buffer[1]);
	shiftOut(_dataPin, _clockPin, LSBFIRST, _buffer[0]);
	digitalWrite(_latchPin, HIGH);
}

void BjSMatrix5x5::off() {
	clear();
	show();
}

bool BjSMatrix5x5::get(byte x, byte y) {
	byte i = 5 * y + x;
	return (_buffer[i >> 3] & (0b10000000 >> (i & 7))) != 0;     
}

void BjSMatrix5x5::set(byte x, byte y, byte on) {
	byte i = 5 * y + x;
	byte s = 0b10000000 >> (i & 7);
	if  (on) {
	  _buffer[i >> 3] |= s;
	} else {
	  _buffer[i >> 3] &= ~s;
	}
}

void BjSMatrix5x5::toggle(byte x, byte y) {
	byte i = 5 * y + x;
	byte n = i >> 3;
	byte s = 0b10000000 >> (i & 7);
	bool on = (_buffer[n] & s) != 0;
	if  (on) {
		_buffer[n] &= ~s;
	} else {
		_buffer[n] |= s;
	}
}

#if BjSMatrixRotate == 999
void BjSMatrix5x5::orientation(byte o) {
	_orientation = o % 4;
}
#endif

void BjSMatrix5x5::bmp(byte* lines) {
#if BjSMatrixRotate == 999
	switch (_orientation) {
		case 1: // 90° counter-clockwise (ROW|COL: 40,30,20,10,00, 41,31,21;11,01, 42,32,22,12,02, 43;33,23,13,03, 44,34,24,14;04)
			_buffer[0] = ((lines[4] & 0b10000) << 3) | ((lines[3] & 0b10000) << 2) | ((lines[2] & 0b10000) << 1) | ((lines[1] & 0b10000) << 0) | ((lines[0] & 0b10000) >> 1) | ((lines[4] & 0b01000) >> 1) | ((lines[3] & 0b01000) >> 2) | ((lines[2] & 0b01000) >> 3);
			_buffer[1] = ((lines[1] & 0b01000) << 4) | ((lines[0] & 0b01000) << 3) | ((lines[4] & 0b00100) << 3) | ((lines[3] & 0b00100) << 2) | ((lines[2] & 0b00100) << 1) | ((lines[1] & 0b00100) << 0) | ((lines[0] & 0b00100) >> 1) | ((lines[4] & 0b00010) >> 1);
			_buffer[2] = ((lines[3] & 0b00010) << 6) | ((lines[2] & 0b00010) << 5) | ((lines[1] & 0b00010) << 4) | ((lines[0] & 0b00010) << 3) | ((lines[4] & 0b00001) << 3) | ((lines[3] & 0b00001) << 2) | ((lines[2] & 0b00001) << 1) | ((lines[1] & 0b00001) << 0);
			_buffer[3] = ((lines[0] & 0b00001) << 7);
			break;
		case 2: // 180°               (ROW|COL: 44,43,42,41,40, 34,33,32;31,30, 24,23,22,21,20, 14;13,12,11,10, 04,03,02,01;00)
			_buffer[0] = ((lines[4] & 0b00001) << 7) | ((lines[4] & 0b00010) << 5) | ((lines[4] & 0b00100) << 3) | ((lines[4] & 0b01000) << 1) | ((lines[4] & 0b10000) >> 1) | ((lines[3] & 0b00001) << 2) | ((lines[3] & 0b00010) << 0) | ((lines[3] & 0b00100) >> 2);
			_buffer[1] = ((lines[3] & 0b01000) << 4) | ((lines[3] & 0b10000) << 2) | ((lines[2] & 0b00001) << 5) | ((lines[2] & 0b00010) << 3) | ((lines[2] & 0b00100) << 1) | ((lines[2] & 0b01000) >> 1) | ((lines[2] & 0b10000) >> 3) | ((lines[1] & 0b00001) << 0);
			_buffer[2] = ((lines[1] & 0b00010) << 6) | ((lines[1] & 0b00100) << 4) | ((lines[1] & 0b01000) << 2) | ((lines[1] & 0b10000) << 0) | ((lines[0] & 0b00001) << 3) | ((lines[0] & 0b00010) << 1) | ((lines[0] & 0b00100) >> 1) | ((lines[0] & 0b01000) >> 3);
			_buffer[3] = ((lines[0] & 0b10000) << 3);
			break;
		case 3: // 90° clockwise      (ROW|COL: 04,14,24,34,44, 03,13,23;33,43, 02,12,22,32,42, 01;11,21,31,41, 00,10,20,30;40)
			_buffer[0] = ((lines[0] & 0b00001) << 7) | ((lines[1] & 0b00001) << 6) | ((lines[2] & 0b00001) << 5) | ((lines[3] & 0b00001) << 4) | ((lines[4] & 0b00001) << 3) | ((lines[0] & 0b00010) << 1) | ((lines[1] & 0b00010) << 0) | ((lines[2] & 0b00010) >> 1);
			_buffer[1] = ((lines[3] & 0b00010) << 6) | ((lines[4] & 0b00010) << 5) | ((lines[0] & 0b00100) << 3) | ((lines[1] & 0b00100) << 2) | ((lines[2] & 0b00100) << 1) | ((lines[3] & 0b00100) << 0) | ((lines[4] & 0b00100) >> 1) | ((lines[0] & 0b01000) >> 3);
			_buffer[2] = ((lines[1] & 0b01000) << 4) | ((lines[2] & 0b01000) << 3) | ((lines[3] & 0b01000) << 2) | ((lines[4] & 0b01000) << 1) | ((lines[0] & 0b10000) >> 1) | ((lines[1] & 0b10000) >> 2) | ((lines[2] & 0b10000) >> 3) | ((lines[3] & 0b10000) >> 4);
			_buffer[3] = ((lines[4] & 0b10000) << 3);
			break;
		default: // 0° original       (ROW|COL: 00,01,02,03,04, 10,11,12;13,14, 20,21,22,23,24, 30;31,32,33,34, 40,41,42,43;44)
			_buffer[0] = ((lines[0] & 0b00011111) << 3) | ((lines[1] & 0b00011100) >> 2);
			_buffer[1] = ((lines[1] & 0b00000011) << 6) | ((lines[2] & 0b00011111) << 1) | ((lines[3] & 0b00010000) >> 4);
			_buffer[2] = ((lines[3] & 0b00001111) << 4) | ((lines[4] & 0b00011110) >> 1);
			_buffer[3] = ((lines[4] & 0b00000001) << 7);
			break;
	}
#elif BjSMatrixRotate == 90
	// 90° anti-clockwise (ROW|COL: 40,30,20,10,00, 41,31,21;11,01, 42,32,22,12,02, 43;33,23,13,03, 44,34,24,14;04)
	_buffer[0] = ((lines[4] & 0b10000) << 3) | ((lines[3] & 0b10000) << 2) | ((lines[2] & 0b10000) << 1) | ((lines[1] & 0b10000) << 0) | ((lines[0] & 0b10000) >> 1) | ((lines[4] & 0b01000) >> 1) | ((lines[3] & 0b01000) >> 2) | ((lines[2] & 0b01000) >> 3);
	_buffer[1] = ((lines[1] & 0b01000) << 4) | ((lines[0] & 0b01000) << 3) | ((lines[4] & 0b00100) << 3) | ((lines[3] & 0b00100) << 2) | ((lines[2] & 0b00100) << 1) | ((lines[1] & 0b00100) << 0) | ((lines[0] & 0b00100) >> 1) | ((lines[4] & 0b00010) >> 1);
	_buffer[2] = ((lines[3] & 0b00010) << 6) | ((lines[2] & 0b00010) << 5) | ((lines[1] & 0b00010) << 4) | ((lines[0] & 0b00010) << 3) | ((lines[4] & 0b00001) << 3) | ((lines[3] & 0b00001) << 2) | ((lines[2] & 0b00001) << 1) | ((lines[1] & 0b00001) << 0);
	_buffer[3] = ((lines[0] & 0b00001) << 7);
#elif BjSMatrixRotate == 180
	// 180°               (ROW|COL: 44,43,42,41,40, 34,33,32;31,30, 24,23,22,21,20, 14;13,12,11,10, 04,03,02,01;00)
	_buffer[0] = ((lines[4] & 0b00001) << 7) | ((lines[4] & 0b00010) << 5) | ((lines[4] & 0b00100) << 3) | ((lines[4] & 0b01000) << 1) | ((lines[4] & 0b10000) >> 1) | ((lines[3] & 0b00001) << 2) | ((lines[3] & 0b00010) << 0) | ((lines[4] & 0b00100) >> 2);
	_buffer[1] = ((lines[3] & 0b01000) << 4) | ((lines[3] & 0b10000) << 2) | ((lines[2] & 0b00001) << 5) | ((lines[2] & 0b00010) << 3) | ((lines[2] & 0b00100) << 1) | ((lines[2] & 0b01000) >> 1) | ((lines[2] & 0b10000) >> 3) | ((lines[1] & 0b00001) << 0);
	_buffer[2] = ((lines[1] & 0b00010) << 6) | ((lines[1] & 0b00100) << 4) | ((lines[1] & 0b01000) << 2) | ((lines[1] & 0b10000) << 0) | ((lines[0] & 0b00001) << 3) | ((lines[0] & 0b00010) << 1) | ((lines[0] & 0b00100) >> 1) | ((lines[0] & 0b01000) >> 3);
	_buffer[3] = ((lines[0] & 0b10000) << 3);
#elif BjSMatrixRotate == 270
	// 90° clockwise      (ROW|COL: 04,14,24,34,44, 03,13,23;33,43, 02,12,22,32,42, 01;11,21,31,41, 00,10,20,30;40)
	_buffer[0] = ((lines[0] & 0b00001) << 7) | ((lines[1] & 0b00001) << 6) | ((lines[2] & 0b00001) << 5) | ((lines[3] & 0b00001) << 4) | ((lines[4] & 0b00001) << 3) | ((lines[0] & 0b00010) << 1) | ((lines[1] & 0b00010) << 0) | ((lines[2] & 0b00010) >> 1);
	_buffer[1] = ((lines[3] & 0b00010) << 6) | ((lines[4] & 0b00010) << 5) | ((lines[0] & 0b00100) << 3) | ((lines[1] & 0b00100) << 2) | ((lines[2] & 0b00100) << 1) | ((lines[3] & 0b00100) << 0) | ((lines[4] & 0b00100) >> 1) | ((lines[0] & 0b01000) >> 3);
	_buffer[2] = ((lines[1] & 0b01000) << 4) | ((lines[2] & 0b01000) << 3) | ((lines[3] & 0b01000) << 2) | ((lines[4] & 0b01000) << 1) | ((lines[0] & 0b10000) >> 1) | ((lines[1] & 0b10000) >> 2) | ((lines[2] & 0b10000) >> 3) | ((lines[3] & 0b10000) >> 4);
	_buffer[3] = ((lines[4] & 0b10000) << 3);
#else
	// 0° original        (ROW|COL: 00,01,02,03,04, 10,11,12;13,14, 20,21,22,23,24, 30;31,32,33,34, 40,41,42,43;44)
	_buffer[0] = ((lines[0] & 0b00011111) << 3) | ((lines[1] & 0b00011100) >> 2);
	_buffer[1] = ((lines[1] & 0b00000011) << 6) | ((lines[2] & 0b00011111) << 1) | ((lines[3] & 0b00010000) >> 4);
	_buffer[2] = ((lines[3] & 0b00001111) << 4) | ((lines[4] & 0b00011110) >> 1);
	_buffer[3] = ((lines[4] & 0b00000001) << 7);
#endif
}

void BjSMatrix5x5::text(char* text) {
	_text = text;
	// The length is 3 times the char count plus 1 gap in between each
	_len = 4 * strlen(text) - 1;
}

int BjSMatrix5x5::len() {
	return _len;
}

void BjSMatrix5x5::textpos(int pos) {
	int i, m;
	if (pos < -4) {
		// The pos is so far left that no chars are shown
		i = -2;
		m = 0;
	} else if (pos < 0) {
		// The pos is so far left that only the first char is shown
		i = -1;
		m = 4 + pos;;
	} else {
		// The position may be that the current and the next character are partially displayed at the same time
		i = pos / 4; // The current char at pos
		m = pos % 4; // pos modulo char width
	}
	int l = strlen(_text); // needed for upper bound check
	int c1 = (i >= 0 && i < l) ? BjSMatrix5x5::getChar((i >= 0 && i < l) ? _text[i] : 0) : 0;
	int c2 = (i > -2 && i + 1 < l) ? BjSMatrix5x5::getChar(_text[i + 1]) : 0;
	// Create a bitmap from the char values moving them m to the left
	byte d[5] = {
		(((c1 & 0b111000000000000) >> 10) << m) | ((((c2 & 0b111000000000000) >> 10) << m) >> 4),
		(((c1 & 0b000111000000000) >>  7) << m) | ((((c2 & 0b000111000000000) >>  7) << m) >> 4),
		(((c1 & 0b000000111000000) >>  4) << m) | ((((c2 & 0b000000111000000) >>  4) << m) >> 4),
		(((c1 & 0b000000000111000) >>  1) << m) | ((((c2 & 0b000000000111000) >>  1) << m) >> 4),
		(((c1 & 0b000000000000111) <<  2) << m) | ((((c2 & 0b000000000000111) <<  2) << m) >> 4)
	};
	bmp(d);
	_pos = pos;
}

bool BjSMatrix5x5::scroll() {
	int p = _pos;
	bool result = false;
	p++;
	if (p > _len + 4) {
		p = -4;
		result = true;
	}
	textpos(p);
	show();
	return result;
}

int BjSMatrix5x5::getChar(char c) {
  // Bit format: {3x1st Line}{3x2nd Line}{3x3rd Line}{3x4th Line}{3x5th Line}
  switch (c) {
    case 'o':
    case 'O':
    case '0': return 0b010101101101010;
    case '1': return 0b010110010010111;
    case '2': return 0b110001010100111;
    case '3': return 0b110001010001110;
    case '4': return 0b101101111001001;
    case '5': return 0b111100110001110;
    case '6': return 0b011100110101010;
    case '7': return 0b111001010010010;
    case '8': return 0b010101010101010;
    case '9': return 0b010101011001110;
    case 'a':
    case 'A': return 0b010101111101101;
    case 'b':
    case 'B': return 0b110101110101110;
    case 'c':
    case 'C': return 0b011100100100011;
    case 'd':
    case 'D': return 0b110101101101110;
    case 'e':
    case 'E': return 0b111100110100111;
    case 'f':
    case 'F': return 0b111100110100100;
    case 'g':
    case 'G': return 0b011100111101011;
    case 'h':
    case 'H': return 0b101101111101101;
    case 'i':
    case 'I': return 0b111010010010111;
    case 'j':
    case 'J': return 0b111001001101010;
    case 'k':
    case 'K': return 0b101101110101101;
    case 'l':
    case 'L': return 0b100100100100111;
    case 'm':
    case 'M': return 0b101111101101101;
    case 'n':
    case 'N': return 0b101111111111101;
    case 'p':
    case 'P': return 0b110101110100100;
    case 'q':
    case 'Q': return 0b010101101111011;
    case 'r':
    case 'R': return 0b110101110101101;
    case 's':
    case 'S': return 0b110001010100011;
    case 't':
    case 'T': return 0b111010010010010;
    case 'u':
    case 'U': return 0b101101101101111;
    case 'v':
    case 'V': return 0b101101101101010;
    case 'w':
    case 'W': return 0b101101101111101;
    case 'x':
    case 'X': return 0b101101010101101;
    case 'y':
    case 'Y': return 0b101101010010010;
    case 'z':
    case 'Z': return 0b111001010100111;
    case '!': return 0b010010010000010;
    case '.': return 0b000000000000010;
    case ',': return 0b000000000010100;
    case ':': return 0b000010000010000;
    case ';': return 0b000010000010100;
    case '-': return 0b000000111000000;
    case '_': return 0b000000000000111;
    case '+': return 0b000010111010000;
    case '/': return 0b001001010100100;
    case '\\':return 0b100100010001001;
    case '=': return 0b000111000111000;
    case '?': return 0b110001010000010;
    case '(': return 0b001010010010001;
    case ')': return 0b100010010010100;
    case '[': return 0b011001001001011;
    case ']': return 0b110010010010110;
    case '{': return 0b011010100010011;
    case '}': return 0b110010001010110;
    case '<': return 0b001010100010001;
    case '>': return 0b100010001010100;
    case '%': return 0b101001010100101;
    case '"': return 0b101101000000000;
    case '|': return 0b010010010010010;
    case '\'':return 0b010000000000000;
    default:  return 0b000000000000000;
  }
}