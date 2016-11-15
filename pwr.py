#!/usr/bin/python3
"""python3 control of buspirate (SWHarden.com)"""

import serial

BUSPIRATE_PORT = '/dev/buspirate' 

def send(ser,cmd):
    """send the command and listen to the response."""
    ser.write(str(cmd+'\n').encode('ascii')) # send our command
    for line in ser.readlines(): # while there's a response
        pass

ser=serial.Serial(BUSPIRATE_PORT, 115200, timeout=1) # is com free?
assert ser.isOpen() #throw an exception if we aren't connected
#send(ser,'#') # reset bus pirate (slow, maybe not needed)
send(ser,'m') # change mode (goal is to get away from HiZ)
send(ser,'9') # mode 9 is DIO
send(ser,'W') # turn power supply to ON. Lowercase w for OFF.
ser.close() # disconnect so we can access it from another app
print("pwr on!") # let the user know we're done.
