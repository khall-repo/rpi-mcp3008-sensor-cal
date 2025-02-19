# rpi-mcp3008-sensor-cal

A program for Raspberry Pi using the BC Robtoics 16-channel analog hat to
monitor and calibrate sensors.

# Install and build on a fresh RPi OS
## Libraries needed to run ./bin/sensor-cal are WiringPi and GTK4.

1. Get and install WiringPi
https://github.com/WiringPi/WiringPi
WiringPi can either be built from source or installed from binary.
Instructions for both methods are listed on the WiringPi Github page.

2. Install the GTK4 libary
$ sudo apt install libgtk-4-1

# Additional libraries needed to build this project:
In addition to 1 and 2 above, you need to..

1. Install the GTK4 Dev library
$ sudo apt install libgtk-4-dev

## Building the project:
$ make


# Usage
## Config
Set your desired options in the config.ini file:
num_sensors - Number of sensors <br />
sensor_min_scale - Bottom of sensor scale <br />
sensor_full_scale - Top of sensor scale <br />
cal1_pv - Zero calibration point <br />
cal2_pv - Span calibration point <br />

The rest of the values shouldn't be messed with unless you know what you're
doing. Feel free to break things, though.. that's why I left them in there.

Connect your BC Robotics 16-channel analog hat to your Raspberry Pi.
Wire up your sensors to the analog hat.

## Run
Run ./bin/sensor-cal

## Sensor zero calibration
Click the "Zero" button in the row of the analog channel to set the zero for
that channel.

## Sensor span calibration
Click the "Span" button in the row of the analog channel to set the span for
that channel.

# Disclaimer 1 of 2
This software is provided "as is", without warranty of any kind, express or
implied, including but not limited to the warranties of merchantability,
fitness for a particular purpose, and noninfringement. In no event shall the
authors or copyright holders be liable for any claim, damages, or other
liability, whether in an action of contract, tort, or otherwise, arising from,
out of, or in connection with the software or the use or other dealings in the
software.
## Use at your own risk.

# Disclaimer 2 of 2
This software is not intended for public use, but feel free to clone this
project and use it as you please. Do not expect any support from the author,
please do not ask. I am writing this for a personal friend to use, and once
it works well enough for him to use, that is where my effort will end with
this project.

# WiringPi License
This project uses the WiringPi library under the GNU LESSER GENERAL PUBLIC
LICENSE.
License document and listing of authors are located in \wiringPi-license

(C)2025 Keith Hall
All rights reserved
