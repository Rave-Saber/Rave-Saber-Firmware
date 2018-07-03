# RaveSaber - AVR Firmware

Firmware for AVR microcontrollers that drives a Dotstar LED strip in the
RaveSaber lightsaber.

This code is responsible for defining available patterns and handling button
presses(toggling power & switching patterns). The pattern rendering code lives
in our [APA102 Library][avr-apa102-lib].


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
