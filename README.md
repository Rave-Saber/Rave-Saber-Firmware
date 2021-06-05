# RaveSaber - AVR Firmware

[![Build Status](https://github.com/Rave-Saber/Rave-Saber-Firmware/actions/workflows/main.yml/badge.svg?branch=master)](https://github.com/Rave-Saber/Rave-Saber-Firmware)

Firmware for AVR microcontrollers that drives a Dotstar LED strip in the
RaveSaber lightsaber.

This code is responsible for defining available patterns and handling button
presses(toggling power & switching patterns). The pattern rendering code lives
in our [APA102 Library][avr-apa102-lib].


## Usage

Connect your programmer to your ATmega168a chip and run `make flash`. If you
want to change or re-order the colors, modify `src/main.c` and re-flash your
chip. We use the USBtinyISP programmer along with all 144 LEDs, but you can
change this by modifying the variables in the `Makefile`.

The firmware assumes you are using a 16Mhz crystal oscillator. If you are
running a different clock frequency, change `F_CPU` in the `Makefile`. You can
enable the external oscillator fuses by running `make set_fuses` and set them
back to the default with `make reset_fuses`. You may need to change the fuse
definitions in the `Makefile` if you want to use those commands with different
ATmega chips.

Once your microcontroller is hooked up to the rest of [the hardware][hardware],
you can long-press the button to toggle power & short-press the button to cycle
patterns.


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
