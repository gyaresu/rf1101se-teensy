# rf1101se-teensy

Some working code for using the RF1101SE radio module based on a Texas Instruments CC1101 chip (that's the one with no MCU and no USB, just the radio).

I think the rf1101se-v3.1 is only good for 433MHz (due to the resistors/etc on the board, not the chip...)

Tested with a Teensy 3.1 but should work with 3.0/3.2

If you know how to move from pin 13 for CLK to pin 14 (alternate SPI pins for Teensy), let me know.

The Arduino SPI module has a method that lets you do that but it doesn't work with the minimal SPI in the library.

So ```SPI.setSCK(pin)``` doesn't work.



