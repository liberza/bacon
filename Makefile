CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p
OBJ2HEX=avr-objcopy
TARGET=xbee
#ADFLAGS= -v -v -v -v    -p m88 -c avrispmkII -P usb
ADFLAGS= -v -v -v -v -p atmega328p -c buspirate -P /dev/buspirate


.PHONY: fuses prog erase


prog : $(TARGET).hex $(TARGET).eeprom
	avrdude $(ADFLAGS) -V -U flash:w:$(TARGET).hex:i
#       avrdude $(ADFLAGS) -U eeprom:w:$(TARGET).eeprom:i

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

%.eeprom : %.obj
	$(OBJ2HEX) -j .eeprop -O ihex $< $@

erase :
	avrdude $(ADFLAGS) -E noreset -e
clean :
	rm -f *.hex *.obj *.o

fuses:
	avrdude $(ADFLAGS) -U lfuse:w:0x62:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
