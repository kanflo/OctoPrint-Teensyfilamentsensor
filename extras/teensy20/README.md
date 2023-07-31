# Teensy 2.0 firmware for the filament sensor

For the impatient, flash `filementsensor.hex` to your Teensy 2.0 using the [https://www.pjrc.com/teensy/loader.html](Teensy loader). Any other Teensy will of course do, I used this 13 year little gem as I had one lying around. If you are patient, install `sudo apt install gcc-avr binutils-avr avr-libc` and run `make`.

Once flashed, you will see a RAW HID device with PID 0x04c9 and VID 0x1234 (I stole the PID from AT&T Microelectronics). Using RAW HID instead of a regular serial port makes you not connect OctoPrint to the filament sensor thinking it is your 3D printer ;)
