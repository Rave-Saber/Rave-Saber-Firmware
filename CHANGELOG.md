# CHANGELOG

## master

* For current-draw purposes, reduce the number of lit LEDs & shift them to the
  end of the strip. This makes it easier to test out patterns by swinging the
  strip around while it's attached to a breadboard.
* Replace the colors array with a variety of patterns and color sequences.
* Switch from the simple effects apa102 library module to the patterns module.
  This allows extension/retraction with any pattern type so we can support all
  patterns instead of just solid colors.

## v1.0.0

Super basic lightsaber firmware. It waits for long button presses to toggle
blade illumination and short presses to cycle the shown color. Only solid
colors are supported and the sequence of colors to cycle through are hardcoded
into the firmware.
