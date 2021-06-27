# BjSMatrix5x5
I designed a shift register LED display with 5 by 5 pixels. The shift registers are setup in serial to create a 32 bit register of which the first 25 bits are used for the display. User shift register A, B, C and D as naming convention and 0 through 7 as the first through last shifted in bit the display is connected as follows:

<pre>
A7 A6 A5 A4 A3
A2 A1 A0 B7 B6
B5 B4 B3 B2 B1
B0 C7 C6 C5 C4
C3 C2 C1 C0 D7
</pre>

This repository contains an example program using the included device driver library. The library enables the following features:

* Get/set single pixels via coordinates x, y in [0..4]
* Set bitmaps via 5 bytes, one for each line taking the 5 least significant bits as bitmap
* Set and scroll text using the 3x5 pixel build-in character set
* Rotate the display oriantation by 0°, 90°, 180° and 270°


## Using the library

Load the library via the provided ZIP file (in /Library/Download/) into your Arduino IDE libraries and include the library then listed in your libraries in your project. You then have to create an instance of the device driver:

```c
  BjSMatrix5x5 matrix = BjSMatrix5x5(SERIAL_PIN, CLOCK_PIN, LATCH_PIN);
```

Where SERIAL_PIN, CLOCK_PIN and LATCH_PIN are arbitrary free digital pins of your arduino to which you connect the corresponding pins of the first shift register.

The display driver instance has an internal buffer which is manipulated and then sent to the shift registers with the <code>show()</code> command.

### Commmands clear() and show() and off()

<code>clear()</code> overwrites the buffer with zeros thus turning every pixel off. <code>show()</code> sends the current buffer content to the display's shift registers. <code>off()</code> does both thus first clearing the buffer and then sending it to the display so every LED is turned off.

```c
  matrix.clear();
  matrix.show();
  // is the same as:
  matrix.off();
```


### Commands get(x, y) and set(x, y, on) and toggle(x, y)

Retrieves / sets the buffer state for a pixel where <code>x</code> and <code>y</code> musst be 0 or greater and 4 or lower. <code>get(x,y)</code> returns 0 for off or 1 for on. <code>set(x,y,on)</code> sets the pixel to off if <code>on</code> is 0 or to on if <code>on</code> is 1. <code>toggle(x,y)</code> toggles the pixel between on and off. Thus these two examples do the same:

```c
  if (matrix.get(2,2) == 1) matrix.set(2,2,0); else matrix.set(2,2,1);
  // is the same as:
  matrix.toggle(2,2);
```

The change is not sent to the display until <code>show()</code> is called.

### Commands text(str) and len() and textpos(i) and scroll()

<code>text(str)</code> sets <code>str</code> as the current text to scroll without writing to the display buffer. The text is just stored in the instance and the text length in pixels is calculated which can be retrieved by <code>len()</code>. Calling <code>textpos(i)</code> calculates the text area in view for the given horizontal scroll index <code>i</code> (which can be negative or beyond the text pixel length to scroll the text in or out) and draws the caracter or partial characters to the buffer. <code>scroll()</code> simple advances the internal last scroll position and updates the buffer and returns true when the scroll position was reset to the start. As only the buffer is set, <code>show()</code> must be called to display it on the display.
  
```c
  matrix.text("Hello world!");
  matrix.textpos(-4);
  // Scroll this text once
  while (!matrix.scroll()) {
    matrix.show();
    delay(150);
  }
```

Because of the self-set limitation of 3x5 pixels per character, some characters look a bit off. Currently only english alphabetic letters A-Z (all displayed as upper-case), digits and these special chars are supported: !.,:;-_+/\=?()[]{}<>%"|\. All unsupported characters will be displayed as blank.

### Command bmp(bitmap)

<code>bmp(bitmap)</code> converts the <code>bitmap</code> to shift register values. The bitmap consits of 5 bytes, each containing the 5 least significant bits as pixels for a line. As only the buffer is set, <code>show()</code> must be called to display it on the display.

```c
  byte heart[5] = [
    0vb0001010, // _ # _ # _
    0b00011111, // # # # # #
    0b00011111, // # # # # #
    0b00001110, // _ # # # _
    0b00000100  // _ _ # _ _
  ];
  matrix.bmp(heart);
  matrix.show();
```

### Command orientation(o)

<code>orientation(o)</code> sets the orientation in which text and bitmaps are written to the buffer where <code>o</code> can be:

* <code>0</code>: The original orientation
* <code>1</code>: Rotated 90° counter-clockwise
* <code>2</code>: Rotated 180°
* <code>3</code>: Rotated 90° clockwise

The change takes effect only on the next call to <code>bmp(bitmap)</code> or <code>textpos(i)</code> or <code>scroll()</code>.

```c
  byte heart[5] = [
    0vb0001010, // _ # _ # _
    0b00011111, // # # # # #
    0b00011111, // # # # # #
    0b00001110, // _ # # # _
    0b00000100  // _ _ # _ _
  ];
  // Rotate the heart counter-clockwise
  int o = -1;
  while (true) {
    o = (o + 1) % 4; // Advance o by 1 modulo 4
    matrix.orientation(o);
    matrix.bmp(heart);
    matrix.show();
    delay(500);
  }
```

This command only exists if <code>BjSMatrixRotate</code> is defiend as 999 in BjSMatrix5x5.h of the library which is default. You can set this to one of the <code>o</code> values to set a fixed orientation and safe some program space when compiled.
