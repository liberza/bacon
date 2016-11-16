#!/usr/bin/python3
import numpy as np
from datetime import datetime, timedelta
import time
import binascii

# local imports
from payload import Payload
from xbee import XBee
import bmp

# make this commandline args at some point. not right now.
MASS1 = 4.75
MASS2 = 5.25
PROFILE = "profiles/umhab52.json"

# Let's simulate some balloons.

if __name__ == "__main__":
    # If your XBee is somewhere else, change this.
    xb = XBee("/dev/xbee", 1200)

    p1 = Payload(PROFILE, MASS1)
    p2 = Payload(PROFILE, MASS2)

    # First order of business: figure out who's who.
    bmp.init_peering(p1, p2, xb)

    # Ok, we have two payloads.
    # Wait for one to request an altitude.

    msg = None
    while(msg == None):
        msg = xb.rx()

    parsed = bmp.parse(msg)
    msg_type = parsed[0]

    if (msg_type == bmp.MSG_TYPES['WAT_REQUEST']):
        # send wat
        pass
    else if (msg_type == bmp.MSG_TYPES['ALT_REQUEST']):
        # do simulation stuff
        if (parsed[1] == p1.addr):
            p = p1
        else if (parsed[1] == p2.addr):
            p = p2
        else:
            print("Another payload?")
            continue

        
    else:
        # don't care
        pass
