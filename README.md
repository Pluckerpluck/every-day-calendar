# The Every Day Calendar

![edc](/images/edc.gif)

## My Addition (Night/Sleep Timer)

This fork adds an additional **Night Time** feature to the Every Day Calendar.

Having mounted this in my bedroom,
I found that once I'd lit up a number of the days it became very bright.
This was regardless of the brightness of the calendar.

I resorted to turning the calendar on and off (via brightness)
each night, but found myself not turning it back on in the morning.
As the calendar was to serve as a visual reminder,
this was not acceptable.

My solution was the night timer!
It allows you to turn off the display for X number of hours,
after which it will automatically turn back on.

The timer only allows a resolution of hours,
and officially has a +-10% accuracy,
though I have in practice found my device more accurate than this.

## Night Timer Usage

Hold **Dec 31st** for 5 seconds to activate the timer selection mode.
Using the January column,
select the number of hours you would like your display to sleep for.
Press **Dec 31st** once more to confirm your selection.

Waiting for your desired time to see the display turn back on,
or tap anywhere to re-enable the display.


# Original README

## Overview

This repository contains firmware and schematics of the [Every Day Calendar](https://www.kickstarter.com/projects/simonegiertz/the-every-day-calendar) project.

Please note that this is a finished project. The files are provided as is and we will not be maintaining this repository.


## Installation

To download the firmware and schematic files, run the following command on your terminal:


    git clone https://gitlab.com/simonegiertz/the-every-day-calendar.git
## Firmware

### Connecting the Every Day Calendar to the Arduino IDE

1. Power on the Every Day Calendar with a 5V adapter and connect the calendar to your computer using a USB Type-B to USB Typ-A cable.

2. Open the Arduino IDE

   - Configure the board type to **Arduino Pro or Pro Mini**

     ![board_config](/images/board_config.png)

   - Configure the processor and clock speed to **Atmega328P (3.3V, 8MHz)**

     ![clockconfig](/images/clockconfig.png)

### Installing Arduino Libraries

1. Copy the contents of this repository's *firmware > libraries* directory into your computer's Arduino libraries folder.

   - Typically that's located in **Documents/Arduino/libraries**

   **Note:** If more instruction is needed, follow Arduino's guide: https://www.arduino.cc/en/guide/libraries

To test and see if the custom libraries are working, you can run our sample code provided in the *firmware > sketches* directory of this Github repository. Simply upload one of the sample code files onto the calendar using the Arduino IDE, open the **Serial Monitor** and set the baud rate to **9600**.

![serialmonitor](/images/serialmonitor.png)

Once everything is working, you're ready to play with the Every Day Calendar! Have fun!

## License

The contents of this repository are released under the following licenses for hardware and software:

- Hardware: CC BY-SA 4.0: https://creativecommons.org/licenses/by-sa/4.0/
- Software: MIT License: https://opensource.org/licenses/MIT
