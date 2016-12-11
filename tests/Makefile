##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########------------------------------------------------------##########

MCU   = atmega328p
F_CPU = 8000000UL  
BAUD  = 9600UL
## Also try BAUD = 19200 or 38400 if you're feeling lucky.

##########------------------------------------------------------##########
##########                 Programmer Defaults                  ##########
##########          Set up once, then forget about it           ##########
##########        (Can override.  See bottom of file.)          ##########
##########------------------------------------------------------##########

PROGRAMMER_TYPE = buspirate
# PROGRAMMER_TYPE = avrispmkII
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -P /dev/buspirate
# PROGRAMMER_ARGS = -P /dev/mkii

##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########     Won't need to change if they're in your PATH     ##########
##########------------------------------------------------------##########

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

## The name of your project (without the .c)
## Or name it automatically after the enclosing directory
TEST_SOLENOID = test_solenoid
TEST_ALT = test_alt

# Object files: will find all .c/.h files in current directory
#  and in LIBDIR.  If you have any other (sub-)directories with code,
#  you can add them in to SOURCES below in the wildcard statement.
LIB_SOURCES=$(wildcard ../lib/*.c)
HEADERS=$(LIB_SOURCES.c=.h)

SOLENOID_SOURCES=$(wildcard test_solenoid.c ../lib/*.c)
SOLENOID_OBJECTS=$(SOLENOID_SOURCES:.c=.o)

ALT_SOURCES=$(wildcard test_alt.c ../lib/*.c)
ALT_OBJECTS=$(ALT_SOURCES:.c=.o)

## Compilation options, type man avr-gcc if you're curious.
CPPFLAGS = -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -I.
CFLAGS = -O3 -g -std=gnu99 -Wall
## Use short (8-bit) data types 
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections 
## Optional, but often ends up with smaller code
LDFLAGS = -Wl,--gc-sections 
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)

## Explicit pattern rules:
##  To make .o files from .c files 
%.o: %.c $(HEADERS) Makefile
	 $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c -o $@ $<;

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

## These targets don't have files named after them
.PHONY: all solenoid alt eeprom clean flash_solenoid flash_alt fuses

all: 
	@echo "Pick a test to make. Choices: solenoid alt"

solenoid: $(TEST_SOLENOID).hex

alt: $(TEST_ALT).hex

debug:
	@echo
	@echo "Source files:"   $(SOURCES)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo   

clean:
	rm -f *.elf *.hex *.obj \
	*.o *.d *.eep *.lst \
	*.lss *.sym *.map \
	*.eeprom

##########------------------------------------------------------##########
##########              Programmer-specific details             ##########
##########           Flashing code to AVR using avrdude         ##########
##########------------------------------------------------------##########

fuses:
	# Comment out ./rst.py if not using the buspirate to program.
	./../rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

flash_solenoid: $(TEST_SOLENOID).hex 
	# Comment out ./rst.py if not using the buspirate to program.
	./../rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<
	./../pwr.py

flash_alt: $(TEST_ALT).hex
	./../rst.py
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$<
	./../pwr.py

## An alias
program: flash

flash_eeprom: $(TARGET).eeprom
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U eeprom:w:$<

avrdude_terminal:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -nt
