#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <apa102.h>
#include <apa102_patterns.h>

// Calculate the size of an array. Must be declared as an array, not a pointer.
#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// The delay time for the extend & retract animations - in milliseconds.
#define EXTEND_RETRACT_DELAY 0

/* Button Press Timings */
// Each count is ~1ms
#define POWER_ON_COUNT 250
#define POWER_OFF_COUNT 750
#define SWITCH_PATTERN_COUNT 100

// TODO: move colors, sequences, & patterns to a "patterns.h" file
// Some colors
static const RGBColor_t RED = { 0xff, 0x00, 0x00 };
static const RGBColor_t GREEN = { 0x00, 0xff, 0x00 };
static const RGBColor_t BLUE = { 0x00, 0x00, 0xff };
//static const RGBColor_t MAGENTA = { 0xff, 0x00, 0x2c };   // current draw is too high
static const RGBColor_t MAGENTA = { 0x88, 0x00, 0x6b };
static const RGBColor_t CYAN = { 0x00, 0x79, 0x38 };
static const RGBColor_t YELLOW = { 0xe6, 0xe1, 0x00 };
static const RGBColor_t ORANGE = { 0xff, 0x1b, 0x00 };
static const RGBColor_t LIME_GREEN = { 0x2d, 0x6b, 0x01 };
static const RGBColor_t VIOLET = { 0x3d, 0x00, 0xff };
// Specific colors for the rainbow sequence
static const RGBColor_t RB_VIOLET = { 0x95, 0x00, 0xff };
static const RGBColor_t RB_INDIGO = { 0x37, 0x00, 0xeb };
static const RGBColor_t RB_YELLOW = { 0xff, 0xff, 0x00 };
static const RGBColor_t RB_ORANGE = { 0xff, 0x20, 0x00 };

// Some color sequences
static const RGBColor_t RGB_SEQUENCE[] = { RED, GREEN, BLUE };
static const uint8_t RGB_SEQUENCE_LENGTH = (ARR_SIZE(RGB_SEQUENCE));

static const RGBColor_t RAINBOW_SEQUENCE[] = {
    RED, RB_ORANGE, RB_YELLOW, GREEN, BLUE, RB_INDIGO, RB_VIOLET
};
static const uint8_t RAINBOW_SEQUENCE_LENGTH = (ARR_SIZE(RAINBOW_SEQUENCE));

static const RGBColor_t TEST_SEQUENCE[] = {
    MAGENTA, LIME_GREEN, MAGENTA, CYAN, ORANGE, ORANGE
};
static const uint8_t TEST_SEQUENCE_LENGTH = (ARR_SIZE(TEST_SEQUENCE));


/* Pattern Choices */
#define SIMPLE_SOLID(c1) static const SolidArgs_t SOLID_ ## c1 = \
    { .color = c1, .delay = 0 }
SIMPLE_SOLID(RED);
SIMPLE_SOLID(BLUE);
SIMPLE_SOLID(GREEN);
SIMPLE_SOLID(MAGENTA);
SIMPLE_SOLID(CYAN);
SIMPLE_SOLID(ORANGE);
SIMPLE_SOLID(LIME_GREEN);
SIMPLE_SOLID(VIOLET);
SIMPLE_SOLID(YELLOW);
static const FlashArgs_t FLASH_RGB = {
    .sequence = RGB_SEQUENCE, .length = RGB_SEQUENCE_LENGTH,
    .color_delay = 70, .blank_delay = 20
};

static const FlashArgs_t FLASH_TEST = {
    .sequence = TEST_SEQUENCE, .length = TEST_SEQUENCE_LENGTH,
    .color_delay = 55, .blank_delay = 15
};

static const RibbonArgs_t RIBBON_RGB = {
    .sequence = RGB_SEQUENCE, .length = RGB_SEQUENCE_LENGTH, .delay = 35
};

static const ScrollArgs_t SCROLL_RAINBOW = {
    .sequence = RAINBOW_SEQUENCE, .length = RAINBOW_SEQUENCE_LENGTH,
    .reverse = false, .delay = 50,
};

static const ScrollArgs_t SCROLL_RGB = {
    .sequence = RGB_SEQUENCE, .length = RGB_SEQUENCE_LENGTH,
    .reverse = false, .delay = 100,
};

static const WideScrollArgs_t WIDE_SCROLL_RGB = {
    .sequence = RGB_SEQUENCE, .length = RGB_SEQUENCE_LENGTH,
    .delay = 10,
};

static const WideScrollArgs_t WIDE_SCROLL_RAINBOW = {
    .sequence = RAINBOW_SEQUENCE, .length = RAINBOW_SEQUENCE_LENGTH,
    .delay = 3,
};


// Selectable Patterns
static const GenericPattern_t PATTERNS[] = {
    SOLID_PATTERN(SOLID_RED),
    SOLID_PATTERN(SOLID_GREEN),
    SOLID_PATTERN(SOLID_BLUE),
    FLASH_PATTERN(FLASH_RGB),
    WIDE_SCROLL_PATTERN(WIDE_SCROLL_RGB),
    SOLID_PATTERN(SOLID_MAGENTA),
    SOLID_PATTERN(SOLID_CYAN),
    SOLID_PATTERN(SOLID_ORANGE),
    FLASH_PATTERN(FLASH_TEST),
    RIBBON_PATTERN(RIBBON_RGB),
    SCROLL_PATTERN(SCROLL_RAINBOW),
    WIDE_SCROLL_PATTERN(WIDE_SCROLL_RAINBOW),
    SOLID_PATTERN(SOLID_LIME_GREEN),
    SOLID_PATTERN(SOLID_VIOLET),
    SOLID_PATTERN(SOLID_YELLOW),
    SCROLL_PATTERN(SCROLL_RGB),
};
static const uint8_t PATTERN_COUNT = (ARR_SIZE(PATTERNS));

static uint8_t current_pattern = 0;


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
 * With a clock of 16Mhz/1024 & a TOP of 16, this interrupt is called every
 * ~1ms:
 *
 *      16 / ( 16000000 / 1024 ) = 0.001024
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

    // Set to CTC mode with TOP of 16
    TCCR0A |= 1 << WGM01;
    OCR0A = 16;
    // Call interrupt on compare match
    TIMSK0 |= 1 << OCIE0A;
    sei();
    // Set clock prescaler to 1/1024
    TCCR0B |= (1 << CS02) | (1 << CS00);
}


/* Pattern selection & playback */

/* Run the current step for the current pattern, incrementing the step number. */
static inline void run_pattern_step(void) {
    run_step(PATTERNS + current_pattern);
}

/* Move to the next pattern, initialize it, then run its first step. */
static inline void next_pattern(void) {
    current_pattern = (current_pattern + 1) % PATTERN_COUNT;
    initialize_pattern(PATTERNS + current_pattern);
    run_pattern_step();
}


int main(void) {
    clock_prescale_set(clock_div_1);

    init_button();
    apa102_init_spi();
    apa102_set_all_leds(rgb(0x000000));
    initialize_pattern(PATTERNS);

    bool powered_on = false;
    while (1) {
        while (last_hold_count < POWER_ON_COUNT) {}
        last_hold_count = 0;
        powered_on = true;
        extend_pattern(PATTERNS + current_pattern, EXTEND_RETRACT_DELAY);
        while (powered_on) {
            if (last_hold_count > POWER_OFF_COUNT) {
                last_hold_count = 0;
                powered_on = false;
            } else if (last_hold_count > SWITCH_PATTERN_COUNT) {
                last_hold_count = 0;
                next_pattern();
            } else if (total_pattern_steps > 1 || playing_pattern_series) {
                run_pattern_step();
            } else {
                // Single step patterns don't require step updates
                _delay_ms(50);
            }
        }
        retract_pattern(PATTERNS + current_pattern, EXTEND_RETRACT_DELAY);
        apa102_set_all_leds(rgb(0x000000));
    }
    return 0;
}
