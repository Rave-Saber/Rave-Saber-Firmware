# RaveSaber - AVR Firmware

Firmware for AVR microcontrollers that drives a Dotstar LED strip in the
RaveSaber lightsaber.

This code is responsible for defining available patterns and handling button
presses(toggling power & switching patterns). The pattern rendering code lives
in our [APA102 Library][avr-apa102-lib].


## Usage

Connect your programmer to your ATmega168a chip and run `make flash`. If you
want to change or re-order the colors, modify `src/main.c` and re-flash your
chip. By default, we use the USBtinyISP programmer along with the last 72 of
the 144 available LEDs, but you can change this by modifying the variables in
the `Makefile`.

Once your microcontroller is hooked up to the rest of [the hardware][hardware],
you can long-press the button to toggle power & short-press the button to cycle
colors.


## TODO

* Setup, Usage, & API docs.
* Use avr power saving modes(when off or single step pattern) w/ button press
  wake-ups.
* Save/Load Patterns to/from EEPROM or SD Card.
* Bluetooth module + desktop or tablet app to control saber & modify patterns
  interactively.


## License

GPL-3.0


[avr-apa102-lib]: https://github.com/Rave-Saber/AVR-APA102-library
[hardware]: https://github.com/Rave-Saber/Rave-Saber-Hardware
