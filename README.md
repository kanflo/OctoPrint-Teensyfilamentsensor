# OctoPrint Teensy Filament Sensor

Work In Progress!

I wanted a filement sensor for my Duplicator 6 prompted by an "abnormal filament despooling incident". I found the "Smart Filament Sensor" by Bigtreetech, reasonably priced around 25 Euros, but it cannot be connected to the D6. I did have an old [Teensy 2.0](https://www.pjrc.com/teensy/) which could interface the filent sensor. An OctoPring plugin could then monitor the printing process, query the Teensy and act if the filament seemed to have run out or gotten jammed. Obviosly, this "Teensy Smart Filament Sensor" will work for any 3D printer.


## Setup

Install via the bundled [Plugin Manager](https://docs.octoprint.org/en/master/bundledplugins/pluginmanager.html) or manually using this URL:

    https://github.com/kanflo/OctoPrint-Teensyfilamentsensor/archive/master.zip


## Teensy 2.0 firmware

The firmware for the Teensy implements a USB device talking RAW HID, meaning there is no need for a driver. Build and flash the Teensy and connect G to Teensy GND, V to Teensy VCC and S to Teensy F6.

[![Smart Filament Sensor pinout](smart-filement-sensor-pinut.png)]


## Mounting

I designed a housing the the filament sensor and Teensy that mounts nicely on the Duplicator 6.


## Using the filament sensor

The plugin adds a status icon to the top bar of the OctoPrint UI:

[![Filament status in OctoPrint](octoprint-status.png)]

When the printer is idle, the icon will flash red/gray if the Teensy is disconnected and start flashing green/gray once the Teensy is connected.

When printing, the icon will be bright green when the filament status is ok. If the sensor detects a jam, it will light bright red and the print will be stopped. If there is an error talking to the Teensy, or if the Teensy is disconnected when printing, the icon will flash bright red/gray.


## Configuration

Currently none.
