# Build any single standalone ATmega128 example.
# Requires avr-gcc, avr-libc and avr-objcopy.
MCU ?= atmega128
F_CPU ?= 16000000UL
SOURCE ?= 04_Wafer_Rotate_System/main.c
TARGET ?= wafer_rotate
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu99

.PHONY: all clean
all: build/$(TARGET).hex

build:
	mkdir -p build

build/$(TARGET).elf: $(SOURCE) | build
	$(CC) $(CFLAGS) -o $@ $<

build/$(TARGET).hex: build/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -rf build
