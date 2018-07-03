#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <apa102.h>
#include <apa102_effects.h>

// Calculate the size of an array. Must be declared as an array, not a pointer.
#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// The delay time for the extend & retract animations - in milliseconds.
#define EXTEND_RETRACT_DELAY 10

/* Button Press Timings */
// Each count is ~1ms
#define POWER_ON_COUNT 250
#define POWER_OFF_COUNT 750
#define SWITCH_COLOR_COUNT 100

// TODO: move colors, sequences, & patterns to a "patterns.h" file
// Some colors
static const RGBColor_t RED = { 0xff, 0x00, 0x00 };
static const RGBColor_t GREEN = { 0x00, 0xff, 0x00 };
static const RGBColor_t BLUE = { 0x00, 0x00, 0xff };
static const RGBColor_t MAGENTA = { 0xff, 0x00, 0x2c };
static const RGBColor_t CYAN = { 0x00, 0x79, 0x38 };
static const RGBColor_t YELLOW = { 0xe6, 0xe1, 0x00 };
static const RGBColor_t ORANGE = { 0xff, 0x1b, 0x00 };
static const RGBColor_t VIOLET = { 0x3d, 0x00, 0xff };


// Selectable Patterns
static const RGBColor_t COLORS[] = {
    RED,
    GREEN,
    BLUE,
    MAGENTA,
    CYAN,
    YELLOW,
    ORANGE,
    VIOLET
};
#define COLOR_COUNT (ARR_SIZE(COLORS))

static uint8_t current_color = 0;


/* Button Handling */
// TODO: Move to button.h file

// The hold count of the last button press.
// TODO: When moving button code to separate file, hide this variable and
//      exposing a function for comparing hold down times to it, reseting the
//      value when returning true. This prevents users from forgetting to set
//      this to 0 after handling a button press.
static volatile uint16_t last_hold_count = 0;

/* Check the button status when Timer 0 reaches the CTC mode TOP.
 *
 * With a clock of 8Mhz/1024 & a TOP of 8, this interrupt is called every ~1ms:
 *
 *      8 / ( 8000000 / 1024 ) = 0.001024
 *
 * When the timer reaches TOP, we sample the button input & compare it to
 * previous runs. When pushing the button down, the timer checks for 4
 * consecutive pushed-down states before accepting the button as pressed-down.
 *
 * When the button is released, the pushed-down time is moved into the
 * `last_hold_count` global variable and the tracking state is reset.
 *
 * By storing pushed-down time count in 2 bytes, we can track button presses of
 * up to 67s.
 *
 * Any application code that consumes a button press should set
 * `last_hold_count` to `0`.
 *
 * TODO: Also test for 4 samples when deciding the button is released
 */
ISR(TIMER0_COMPA_vect) {
    static volatile uint16_t current_hold_count = 0;
    static volatile bool tracking_press = false;
    static volatile uint8_t sample_count = 0;

    const bool is_pressed = (PIND & (1 << PD2)) == 0;

    if (is_pressed) {
        if (!tracking_press) {
            sample_count++;
            if (sample_count > 3) {
                tracking_press = true;
                current_hold_count = sample_count;
                sample_count = 0;
            }
        } else {
            current_hold_count++;
        }
    } else {
        if (tracking_press) {
            tracking_press = false;
            last_hold_count = current_hold_count;
            current_hold_count = 0;
        } else {
            sample_count = 0;
        }
    }
}

/* Initialize the Button Input, Debouncing Timer(Timer0), & Interrupts.
 *
 * Sets the enable interrupts bit.
 */
static inline void init_button(void) {
    // Set INT0 to input & enable pull up resistor
    DDRD &= ~(1 << PD2);
    PORTD |= 1 << PD2;

    // Set to CTC mode with TOP of 8
    TCCR0A |= 1 << WGM01;
    OCR0A = 8;
    // Call interrupt on compare match
    TIMSK0 |= 1 << OCIE0A;
    sei();
    // Set clock prescaler to 1/1024
    TCCR0B |= (1 << CS02) | (1 << CS00);
}


int main(void) {
    clock_prescale_set(clock_div_1);

    init_button();
    apa102_init_spi();
    apa102_set_all(rgb(0x000000));

    bool powered_on = false;
    while (1) {
        while (last_hold_count < POWER_ON_COUNT) {}
        last_hold_count = 0;
        powered_on = true;
        extend(COLORS[current_color], EXTEND_RETRACT_DELAY);
        while (powered_on) {
            if (last_hold_count > POWER_OFF_COUNT) {
                last_hold_count = 0;
                powered_on = false;
            } else if (last_hold_count > SWITCH_COLOR_COUNT) {
                last_hold_count = 0;
                current_color = (current_color + 1) % COLOR_COUNT;
                apa102_set_all(COLORS[current_color]);
            } else {
                _delay_ms(50);
            }
        }
        retract(COLORS[current_color], EXTEND_RETRACT_DELAY);
        apa102_set_all(rgb(0x000000));
    }
    return 0;
}
