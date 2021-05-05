# Use the full strip
LED_COUNT = 144

# Shift the LED we start at(if not using full strip)
#STARTING_LED = 73

# Max limit per strip = (3A regulator - 500mA MCU) / 2 strips
# A lower setting will make your batteries last longer
CURRENT_LIMIT = 1250

# Run at 20Mhz
F_CPU = 20000000UL

# ATmegaXX8a DIP
MCU = atmega168
MOSI = PB3
MOSI_DDR = DDRB
MOSI_PORT = PORTB
SS = PB2
SS_DDR = DDRB
SS_PORT = PORTB
SCK = PB5
SCK_DDR = DDRB
EXT_CRYSTAL_LFUSE = 0xF7
DEFAULT_LFUSE = 0x62


PROGRAMMER = usbtiny

LIBDIR = libs/AVR-APA102-library/src

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

TARGET = firmware

SOURCES=$(wildcard src/*.c $(LIBDIR)/*.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

CPPFLAGS  = -I. -I$(LIBDIR)
CPPFLAGS += -DF_CPU=$(F_CPU)
CPPFLAGS += -DMOSI=$(MOSI) -DMOSI_DDR=$(MOSI_DDR) -DMOSI_PORT=$(MOSI_PORT)
CPPFLAGS += -DSS=$(SS) -DSS_DDR=$(SS_DDR) -DSS_PORT=$(SS_PORT)
CPPFLAGS += -DSCK=$(SCK) -DSCK_DDR=$(SCK_DDR)
CPPFLAGS += -DLED_COUNT=$(LED_COUNT)

ifdef STARTING_LED
CPPFLAGS += -DSTARTING_LED=$(STARTING_LED)
endif

ifdef CURRENT_LIMIT
CURRENT_PER_LED = $$(( $(CURRENT_LIMIT) / $(LED_COUNT) ))
ifeq ($(shell test $(CURRENT_PER_LED) -lt 60; echo $$?), 0)
CPPFLAGS += -DCURRENT_PER_LED=$(CURRENT_PER_LED)UL
endif
endif

CFLAGS  = -Os -g -std=c11 -Wall -Wextra -pedantic
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS  = -Wl,-Map,$(TARGET).map
LDFLAGS += -Wl,--gc-sections

TARGET_ARCH = -mmcu=$(MCU)


%.o: %.c $(HEADERS) Makefile
	 $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@


.PHONY: all flash set_fuses reset_fuses clean

all: $(TARGET).hex

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<

set_fuses:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:$(EXT_CRYSTAL_LFUSE):m

reset_fuses:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:$(DEFAULT_LFUSE):m

clean:
	rm -f *.hex *.elf *.map
	rm -f src/*.o $(LIBDIR)/*.o
