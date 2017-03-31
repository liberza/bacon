# avr details
MCU   = atmega328p
F_CPU = 8000000UL  

# programmer settings
BAUD  = 9600UL
PROGRAMMER_TYPE = buspirate
PROGRAMMER_ARGS = -P /dev/buspirate -V

# bin locations
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

# Program names
MAIN = bacon
TEST_SOLENOID = test_solenoid
TEST_ALT = test_alt

# Sources
LIB_SOURCES=$(wildcard lib/*.c)
SOLENOID_SOURCES=$(wildcard tests/test_solenoid.c lib/*.c)
ALT_SOURCES=$(wildcard tests/test_alt.c lib/*.c)
MAIN_SOURCES=$(wildcard bacon.c lib/*.c)

# Object files: will find all .c/.h files in *_SOURCES
SOLENOID_OBJECTS=$(SOLENOID_SOURCES:.c=.o)
ALT_OBJECTS=$(ALT_SOURCES:.c=.o)
MAIN_OBJECTS=$(MAIN_SOURCES:.c=.o)

# Headers: only in LIB_SOURCES
HEADERS=$(LIB_SOURCES:.c=.h)

# Compilation options
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I.
# Enable warnings and optimization
CFLAGS = -O3 -g -std=gnu99 -Wall
# Use short (8-bit) data types 
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
# Split up object files per function and remove unused code
CFLAGS += -ffunction-sections -fdata-sections 
LDFLAGS += -Wl,--gc-sections 
TARGET_ARCH = -mmcu=$(MCU)

# To make .o files from .c files
%.o: %.c $(HEADERS) Makefile
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

$(MAIN).elf: $(MAIN_OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

$(TEST_SOLENOID).elf: $(SOLENOID_OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

$(TEST_ALT).elf: $(ALT_OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ 

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

.PHONY: all disassemble size clean flash fuses alt solenoid flash_alt \
		flash_solenoid bacon

bacon: $(MAIN).hex

solenoid: $(TEST_SOLENOID).hex

alt: $(TEST_ALT).hex

all: bacon

# Optionally create listing file from .elf
disassemble: $(MAIN).lst

# Optionally show how big the resulting program is 
size:  $(MAIN).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(MAIN).elf

clean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *.eeprom \
	lib/*.elf lib/*.hex lib/*.obj lib/*.o lib/*.d lib/*.eep lib/*.lst lib/*.lss \
	lib/*.sym lib/*.map lib/*.eeprom tests/*.elf tests/*.hex tests/*.obj \
	tests/*.o tests/*.d tests/*.eep tests/*.lst tests/*.lss tests/*.sym \
	tests/*.map tests/*.eeprom

### avrdude details ###
# Comment out ./rst.py if you don't need to reset a buspirate before flashing...
fuses:
	./rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

flash: $(MAIN).hex 
	./rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_solenoid: $(TEST_SOLENOID).hex
	./rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_alt: $(TEST_ALT).hex
	./rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<

flash_eeprom: $(MAIN).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

avrdude_terminal:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt
