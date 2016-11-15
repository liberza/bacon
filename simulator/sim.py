#!/usr/bin/python3
import numpy as np
from datetime import datetime, timedelta

# local imports
from payload import Payload
from xbee import XBee
import bmp

# make this commandline args at some point. not right now.
MASS1 = 4.75
MASS2 = 5.25
PROFILE = "profiles/umhab52.json"
BROADCAST = 0x000000000000FFFF
TIMEOUT = timedelta(seconds=5)

# Send a Who Art Thou request, set addresses for two payloads.
def wat(p1, p2):
    while ((p1.addr == None) and (p2.addr == None)):
        xb.tx(bmp.MSG_TYPES['WAT_REQUEST'], BROADCAST)
        msg = None
        start = datetime.now()
        while (msg == None):
            now = datetime.now()
            if (now - start > TIMEOUT):
                xb.tx(bmp.MSG_TYPES['WAT_REQUEST'], BROADCAST)
                start = datetime.now()

            msg = xb.rx()
            if ((msg != None) and (msg[3] != XBee.FRAME_TYPES['RX'])):
                msg = None

        if msg != None:
            parsed = bmp.parse(msg)
            if (parsed[0] == bmp.MSG_TYPES['WAT_REPLY']) and (parsed[1] == 'P'):
                if (p1.addr == None):
                    p1.addr = parsed[2]
                    print(p1.addr)
                elif (p2.addr == None):
                    p2.addr = parsed[2]
                    print(p2.addr)
                else:
                    print("This should be impossible. But here we are.")
    

# Let's simulate some balloons.

if __name__ == "__main__":
    # If your XBee is somewhere else, change this.
    xb = XBee("/dev/xbee", 1200)

    p1 = Payload(PROFILE, MASS1)
    p2 = Payload(PROFILE, MASS2)

    # First order of business: figure out who's who.
    wat(p1, p2)

    # Ok, we have two payloads.
    # Wait for one to request an altitude.
