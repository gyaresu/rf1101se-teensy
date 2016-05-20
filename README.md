# rf1101se-teensy

Hi! This is working code for a specific couple of devices.

You're going to want to have at least one (or a dozen) of these really really cheap RF1101SE radio module based on a Texas Instruments CC1101 chip (that's the one with no MCU and no USB, just the radio).

You're also going to want to have a [Teensy](https://www.pjrc.com/teensy/teensy31.html) microcontroller.

Now all of this works with the Arduino 'whatever' but if you have a 'whatever' then you'll need Logic Converters because "whatever's" do 5V on the digital pins and the Teensy can deal with either 3.3V or 5V because...

### Teensy don't care.

Teensy is your basic honey badger of microcontrollers.

Now the rf1101se-v3.1 is probably only good for 433MHz because the one I have is missing resistors on the circuit board that would enable it ot be useful at any other frequencies. So it's not the chip that's at fault, just the cheap board design.

Ok, so this has been tested with a Teensy 3.1 but it shoul work with with a Teensy 3.0 or 3.2 because that's how they're built.

p.s. If you know how to move from pin 13 for CLK to pin 14 (alternate SPI pins for Teensy), let me know.

The Arduino SPI module has a method that lets you do that but it doesn't work with the minimal SPI in the library.

So ```SPI.setSCK(pin)``` doesn't work.

![https://github.com/gyaresu/rf1101se-teensy/blob/master/rf1101-teensy.png](https://github.com/gyaresu/rf1101se-teensy/blob/master/rf1101-teensy.png)

I include Yardstick One (YS1) code in this repo because I have one. If you don't have one then get one because they're awesome.

 - Gareth

