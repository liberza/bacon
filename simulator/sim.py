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
    print("Performing initial peering...")
    bmp.init_peering(p1, p2, xb)

    # Ok, we have two payloads.
    # Wait for one to request an altitude.
    print("Waiting for initial altitude requests...")

    launched = False
    p1_ready = False
    p2_ready = False
    p1_landed = False
    p2_landed = False
    while((p1_landed == False) or (p2_landed == False)):
        msg = None
        while(msg == None):
            msg = xb.rx()

        parsed = bmp.parse(msg)
        msg_type = parsed[0]

        if (msg_type == bmp.MSG_TYPES['WAT_REQUEST']):
            # send wat
            pass
        elif (msg_type == bmp.MSG_TYPES['ALT_REQUEST']):
            # do simulation stuff
            if (parsed[1] == p1.addr):
                p1_ready = True
                cur_p = p1
            elif (parsed[1] == p2.addr):
                p2_ready = True
                cur_p = p2
            else:
                print("Another payload?")
                continue
            
            ballast_time_ms = parsed[2]
            cur_p.drop_mass(ballast_time_ms)
            if (launched):
                time_elapsed += round(time.time() - time_launched)
                cur_p.adjust_time(time_elapsed)
            # Send them their current alt.
            if (cur_p.alt() is not None):
                alt = cur_p.alt()
                print("Sending alt: " + "s" + str(alt))
                xb.tx("s" + str(alt), cur_p.addr)
            else:
                if (cur_p.addr == p1.addr):
                    p1_landed = True
                elif (cur_p.addr == p2.addr):
                    p2_landed = True
                else:
                    print("Another payload?")
                continue
            
            if (p1_ready and p2_ready and launched == False):
                launched = True
                time_launched = time.time()
                time_elapsed = 0
                print("Launched!")
