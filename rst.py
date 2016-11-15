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
send(ser,'#') # reset bus pirate (slow, maybe not needed)
ser.close() # disconnect so we can access it from another app
print("buspirate reset!") # let the user know we're done.

