# OctoPrint Teensy Filament Sensor

Work In Progress!

I wanted a filement sensor for my Duplicator 6 prompted by an "abnormal filament despooling incident". I found the "Smart Filament Sensor" by Bigtreetech, reasonably priced around 25 Euros, but it cannot be connected to the D6. I did have an old [Teensy 2.0](https://www.pjrc.com/teensy/) which could interface the filent sensor. An OctoPring plugin could then monitor the printing process, query the Teensy and act if the filament seemed to have run out or gotten jammed. Obviosly, this "Teensy Smart Filament Sensor" will work for any 3D printer.

![3D Model](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/3d-model-2.png)

## Setup

Install via the bundled [Plugin Manager](https://docs.octoprint.org/en/master/bundledplugins/pluginmanager.html) or manually using this URL:

    https://github.com/kanflo/OctoPrint-Teensyfilamentsensor/archive/master.zip


## Teensy 2.0 firmware

The firmware for the Teensy implements a USB device talking RAW HID, meaning there is no need for a driver. Flash the Teensy with `filamentsensor.hex` or roll your own firmware.


## Mounting

I designed a housing the the filament sensor and Teensy that mounts nicely on the Duplicator 6. I printed them in PLA with 80% infill for sturdiness, probably overkill. Make sure you can insert the "lock peg" into the slot of the "back shell". Also "pre thread" all holes, especially the M4 holes in the lock peg and the corresponding holes in the back shell.

Material needed is:

 - 1x strip board, 9x11 holes
 - 2x Female headers 1x8 (or atleast 1x6)
 - 3x Male headers 1x1
 - 5x M3x8 screws
 - 2x M3 nuts
 - 2x M4x15 screws
 - 2x M4 nuts

1. First solder the headers to the strip board leaving one row of pins on the outside of the Teensy.
2. Solder three male single pin headers for VCC, GND and PF6.
3. Depending on you strip board, make sure the traces are cut, isolating the pins of the Teensy
4. The strip board is fastened by a pair of M3 screws and you need to drill holes in the strip board. Place in inside the 3D printed housing and mark the centers with a pen. Drill both holes.
5. Fasten the two M3 screws with a blob of hot glue or inserting the screw will be difficult. It should now look something like this:

![Strip Board](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/strip-board.jpg)

6. Cut the wires fo the filament sensor to about 6cm in length and solder to the three headers. The wires on mine were marked with dense dots (VCC, red), stripes (GND, black) and sparse dots (S, white):

![Filament Sensor Wire](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/wire.jpg)

7. Connect the filament sensor to the Teensy, place everything in the housing:

![Mounting Complete!](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/mounting-complete.jpg)

8. Depending on your filament sensor, you might need to crack it open and cut the spring inside. Mine was so stiff the extruder failed to pull the filament through the sensor. I took two pair of pliers and pulled the spring and then cut away pieces or it until it was looes enough. I am not sure if it helps, but this is the amount of material I removed:

![Spring Break](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/spring-break.jpg)

9. Now it's time to mount the sensor. I removed the top holder for the extruder cable to gain access to the mounting hole:

![D6 Mounting Hole](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/d6-mounting-hole.jpg)

10. Press the "lock peg" through the hole of the D6, press the housing with the sensor and everything against the lock peg and realize you need one more hand for fastening the M4 screws. I honestly have no idea how I did ut, but succeeded with only two hands ;)

11. Fasten the M4 screws. I had a bit of wiggle so I added some double sided tape between the D6 back wall and the housing.

12. Insert the filement tube into the top of the filement sensor and feed your filament through it:

![Everything Mounted](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/d6-mounted.jpg)


## Using the filament sensor

Before using the sensor you need to copy a file to your OctoPrint installation. This requires that you know how to connect to your OctoPrint server using SSH:

```
scp extras/00-filament-sensor.rules octopi:
ssh octopi
sudo mv 00-filament-sensor.rules /etc/udev/rules.d
```

If you already connected the Teensy, disconnect and reconnect it.

The plugin adds a status icon to the top bar of the OctoPrint UI:

![Filament status in OctoPrint](https://raw.githubusercontent.com/kanflo/OctoPrint-Teensyfilamentsensor/main/extras/images/octoprint-status.png)

When the printer is idle, the icon will flash red/gray if the Teensy is disconnected and start flashing green/gray once the Teensy is connected. It should flash green/gray now. If not, ssh to your OctoPrint server and run the command `tail -f logs/octoprint.log`. The directory `logs` is located in a directory called `.octoprint` om by OctoPi.

When printing, the icon will be bright green when the filament status is ok. If the sensor detects a jam, it will light bright red and the print will be stopped (well, that's a TODO for now). If there is an error talking to the Teensy, or if the Teensy is disconnected when printing, the icon will flash bright red/gray.

When the counter ticks, the new counter will be written to the OctoPrint log.


## Configuration

Currently none.
