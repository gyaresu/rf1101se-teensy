# rf1101se-teensy

Hi! This is working code for a specific couple of devices.

You're going to want to have at least one (or a dozen) of these really really cheap RF1101SE radio module based on a Texas Instruments CC1101 chip (that's the one with no MCU and no USB, just the radio).

You're also going to want to have a [Teensy](https://www.pjrc.com/teensy/teensy31.html) or basically any microcontroller that can speak [SPI].

Now all of this works with the Arduino 'whatever' but if you have a 'whatever' then you'll need Logic Converters because "whatever's" do 5V on the digital pins and the Teensy can deal with either 3.3V or 5V because...

### Teensy don't care.

Teensy is your basic honey badger of microcontrollers.

Now the rf1101se-v3.1 is probably only good for 433MHz because the one I have is missing resistors on the circuit board that would enable it to be useful at any other frequencies. So it's not the chip that's at fault, just the cheap board design.


***TODO***
Move to the defeault Arduino SPI library.
I would like to drive two slave devices (cc1101 chips) with a single microcontroller but the included panstamp library has it's own `spi.c` and `spi.h` which don't allow ```SPI.setSCK(pin)```

![ASK/OOK encoding](/files/rf1101-teensy.png)

I include Yardstick One (YS1) code in this repo because I have one. If you don't have one then get one because they're awesome.

 - [Gareth]

[SPI]: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
[Gareth]: https://twitter.com/gareth__
