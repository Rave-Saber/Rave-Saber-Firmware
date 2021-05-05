# CHANGELOG

## master

* Bump clock speed to 20Mhz, raise button polling interval from 1ms to 10ms, &
  drop max button hold time from 67s to 2.5s.

## v2.0.0

Patterns! Support for rendering things other than single colors has been added,
with the ability to define your own custom patterns. Since the hardware for
this release is now on battery power, we've added some powersaving
optimizations as well.

* ROYGBIV color sequences are now ROYGBV, since indigo was too similar to
  violet.
* Use all 144 LEDs in a strip with a maximum current draw of 1.25A.
* Add custom WideBand & FlashBand patterns with rainbow color sequences.
* Modify code for use with 16Mhz CPU clock speeds. Add make commands to
  set/reset fuses for external crystals.
* Replace the colors array with a variety of patterns and color sequences.
* Switch from the simple effects apa102 library module to the patterns module.
  This allows extension/retraction with any pattern type so we can support all
  patterns instead of just solid colors.

## v1.0.0

Super basic lightsaber firmware. It waits for long button presses to toggle
blade illumination and short presses to cycle the shown color. Only solid
colors are supported and the sequence of colors to cycle through are hardcoded
into the firmware.
