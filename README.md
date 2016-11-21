# rf1101se-teensy

Hi! This is my repo for learning about microcontrollers and packetised radio with the [CC1101 by Texas Instruments](cc1101) chip driven by 'Arduino-compatible' devices.

You're going to want to have at least one (or a dozen) of these really really cheap RF1101SE radio module based on a Texas Instruments CC1101 chip (that's the one with no MCU and no USB, just the radio).

![rf1101 with atmega328 from boldport and FTDI serial adapter](/files/unit.jpg)
_I'm driving *this* 3.3v chip directly from the Atmega328 with 5v... but other chips have acted in super weird ways. 
So now you've been warned(!), it really shouldn't work. YMMV_  ¯\\_(ツ)_/¯

 * cc1101 — Just the radio
 * cc1110 — radio and [MCU] (Radica IM-ME used in [OpenSesame])
 * cc1111 — radio, mcu, usb (YARD Stick One)

You're also going to want to have a [Teensy](teensy), [Arduino](arduino) or basically any microcontroller that can speak [SPI].

Now all of this works with the Arduino 'whatever' but if you have a 'whatever' then you'll need Logic Converters because "whatever's" do 5V on the digital pins and the Teensy can deal with either 3.3V or 5V because...

### Teensy don't care.

Teensy is your basic honey badger of microcontrollers.

Now the rf1101se-v3.1 is probably only good for 433MHz because the one I have is missing resistors on the circuit board that would enable it to be useful at any other frequencies. So it's not the chip that's at fault, just the cheap board design.

### Encode all the things!

The example presented currently enables:
 * 16 bit preamble
 * 2 byte Sync Word (0xEEEE)
 * CRC Checking
 * [Gaussian frequency-shift keying](GFSK)
 * [Manchester encoding](manchester)
 * Address filtering (byte after sync word for specific device selection)
 
### Read The Fine Manual
I've tried to document what the registers do in the Arduino code.
You really need to read the manual though to understand what's going on.
 * http://www.ti.com/lit/ds/symlink/cc1101.pdf

![cc1101 reference manual](/files/spec.png)

![gfsk manchester](/files/gfsk-manchester.png)
![gfsk manchester detailed](/files/gfsk-manchester-preamble.png)
![gfsk manchester decoding](/files/gfsk-manchester-decoding.png)

***TODO***
Move to the default Arduino SPI library.
I would like to drive two slave devices (cc1101 chips) with a single microcontroller but the included panstamp library has it's own `spi.c` and `spi.h` which don't allow ```SPI.setSCK(pin)```

![ASK/OOK encoding](/files/rf1101-teensy.png)

I include Yardstick One (YS1) code in this repo because I have one. If you don't have one then get one because they're awesome.

### Links to things
 * [Jared Boone's manchester decoding function](https://github.com/jboone/tpms/blob/master/src/bit_coding.py)
 * [YARD Stick One](ys1)
 * [gareth__](twitter) on the twitters
 * Freenode IRC — gareth__ on #HackRF #RFCat etc.

[SPI]: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
[twitter]: https://twitter.com/gareth__
[manchester]: https://en.wikipedia.org/wiki/Manchester_code
[GFSK]: https://en.wikipedia.org/wiki/Frequency-shift_keying#Gaussian_frequency-shift_keying
[ys1]: http://greatscottgadgets.com/yardstickone/
[cc1101]: http://www.ti.com/product/cc1101
[arduino]: https://www.arduino.cc
[teensy]: https://www.pjrc.com/teensy/index.html
[OpenSesame]: http://samy.pl/opensesame/
[MCU]: https://en.wikipedia.org/wiki/Microcontroller
