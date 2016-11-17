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
MASS1 = 2.8
MASS2 = 3.2
PROFILE = "profiles/umhab52.json"

# Let's simulate some balloons.

if __name__ == "__main__":
    # If your XBee is somewhere else, change this.
    xb = XBee("/dev/xbee", 1200)

    p1 = Payload(PROFILE, MASS1, "P1")
    p2 = Payload(PROFILE, MASS2, "P2")

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
            addr = int.from_bytes(parsed[2], byteorder="big")
            xb.tx(parsed[1], addr)
        elif (msg_type == bmp.MSG_TYPES['ALT_REQUEST']):
            # do simulation stuff
            addr = int.from_bytes(parsed[1], byteorder="big")
            if (addr == p1.addr):
                p1_ready = True
                cur_p = p1
            elif (addr == p2.addr):
                p2_ready = True
                cur_p = p2
            else:
                print("Another payload?")
                continue
            
            ballast_time_ms = parsed[2]
            cur_p.drop_mass(ballast_time_ms)
            cur_time = time.time()
            last_time = cur_p.last_request_time
            last_alt_m = cur_p.last_alt/10.0
            if (launched):
                cur_p.adjust_time(cur_time)
            else:
                cur_p.last_request_time = cur_time
            # Send them their current alt.
            if (cur_p.alt() is not None):
                timestamp = datetime.fromtimestamp(cur_time).strftime("[%H:%M:%S]")
                alt_m = cur_p.alt()/10.0
                speed = (alt_m-last_alt_m)/(cur_time-last_time)
                if (speed == None):
                    speed = 0
                print("{0} {1}: alt={2:.1f}m speed={3:.2f}m/s mass={4:.3f}kg drop_time={5}".format(timestamp,cur_p.name,alt_m,speed,cur_p.mass,ballast_time_ms))
                alt = cur_p.alt()
                cur_p.last_alt = alt
                xb.tx("s" + str(int(alt)), cur_p.addr)
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
        elif (msg_type == bmp.MSG_TYPES['PAYLOAD_ALT']):
            if (addr == p1.addr):
                p1_ready = True
                cur_p = p1
            elif (addr == p2.addr):
                p2_ready = True
                cur_p = p2
            else:
                print("Another payload?")
                continue
            print(cur_p.name + " says one alt is " + parsed[1].decode())
