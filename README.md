N64 Controller Adapter for Arduino

Author: KRAHU



This folder contains the files needed to turn an Arduino micro into an N64 controller adapter.



Basic:

The folder includes the complete, working Arduino code. All buttons are supported, and you can adjust certain values if, for example, the joystick mapping is incorrect.



One thing to note: in my experience, the adapter did not work correctly with a second-hand controller I tested, but it worked properly with an OEM N64 controller.

Some buttons may also appear with incorrect names in applications such as sm64coopdx. The buttons themselves work correctly; only their displayed names may be incorrect, so I did not bother to change the mappings.



Included Files



The folder contains:

\-The complete Arduino source code.
-A Pads layout file for use with a PCB milling machine.
-A PNG wiring diagram showing how to wire the adapter.



You do not need a PCB milling machine to build the adapter. You can wire the circuit directly on a breadboard.

I also used PCB headers for the Arduino Micro so that I wouldn’t have to solder the Arduino directly onto the board.





N64 Extension Cable:



If you do not want to cut the cable from your N64 controller and solder it directly to the adapter, you will need to purchase an N64 controller extension cable.





Questions and Issues:



Have a question or found a problem? Please open an Issue on this repository.

For general questions, troubleshooting, or suggestions, feel free to ask there.

