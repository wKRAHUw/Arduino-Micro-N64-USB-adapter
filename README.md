# N64 Controller Adapter for Arduino

**Author:** KRAHU

This project contains the files needed to turn an **Arduino Micro** into a USB N64 controller adapter.

## Basic Information

The folder includes the complete, working Arduino code. All N64 buttons are supported, and certain values can be adjusted if, for example, the joystick mapping or calibration needs to be changed.

One thing to note is that, in my experience, the adapter did not work correctly with a second-hand N64 controller that I tested. It worked properly with an OEM N64 controller.

Some buttons may also appear under incorrect names in applications such as **sm64coopdx**. The buttons themselves work correctly; only their displayed names may be incorrect, so I did not change the mappings.

## Included Files

The project contains:

* Complete Arduino source code
* PCB layout file for use with a PCB milling machine
* PNG wiring diagram showing how to wire the adapter

## Required Library

You will need the **Joystick library by Matthew Heironimus**.

It can be installed through the Arduino IDE's Library Manager by searching for **"Joystick"**.

## Building the Adapter

You do not need a PCB milling machine to build the adapter. The circuit can also be wired directly on a breadboard.

I used PCB headers for the Arduino Micro so that I would not have to solder the Arduino directly onto the board.

## N64 Extension Cable

If you do not want to cut the cable from your N64 controller and solder it directly to the adapter, you will need an **N64 controller extension cable**.

## Questions and Issues

Have a question or found a problem? Please open an **Issue** on this repository.

For general questions, troubleshooting, or suggestions, feel free to ask there.
