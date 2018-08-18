# RaveSaber - AVR Firmware

[![Build Status](https://travis-ci.org/Rave-Saber/Rave-Saber-Firmware.svg?branch=master)](https://travis-ci.org/Rave-Saber/Rave-Saber-Firmware)

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

The firmware assumes you are using a 16Mhz crystal oscillator. If you are
running a different clock frequency, change `F_CPU` in the `Makefile`. You can
enable the external oscillator fuses by running `make set_fuses` and set them
back to the defualt with `make reset_fuses`. You will need to change the fuse
definitions in the Makefile if you are using something other than an
ATmega168a.

Once your microcontroller is hooked up to the rest of [the hardware][hardware],
you can long-press the button to toggle power & short-press the button to cycle
colors.


## TODO

* Setup, Usage, & API docs.
* Power saving
    * Take measurements of LED & MCU current draws to establish baseline.
    * Use avr power saving modes(when off or single step pattern) w/ button
      press wake-ups(only stay woken up if we pass a button threshold).
    * Use timers for delays so we can sleep during steps?
    * Disable unused peripherals.
* Save/Load Patterns to/from EEPROM or SD Card.
* Bluetooth module + desktop or tablet app to control saber & modify patterns
  interactively.


## License

GPL-3.0


[avr-apa102-lib]: https://github.com/Rave-Saber/AVR-APA102-library
[hardware]: https://github.com/Rave-Saber/Rave-Saber-Hardware
