# Run at 8Mhz
F_CPU = 8000000UL


# Lumenati 8-LED Strip
#LED_COUNT = 8

# Dotstar 144/m Strip
#LED_COUNT = 60
#LED_COUNT = 80
LED_COUNT = 100
#LED_COUNT = 144


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

CFLAGS  = -Os -g -std=gnu99 -Wall
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


.PHONY: all flash clean

all: $(TARGET).hex

flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<

clean:
	rm -f *.hex *.elf *.map
	rm -f src/*.o $(LIBDIR)/*.o
