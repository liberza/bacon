#!/usr/bin/python3
import numpy as np
from datetime import datetime, timedelta
import time
import binascii

# local imports
from payload import Payload
from xbee import XBee
from liveplot import LivePlot
import bmp

# make this commandline args at some point. for now, this is completely fine.
MASS1 = 2.9
MASS2 = 3.2
PROFILE = "profiles/umhab52.json"

# Let's simulate some balloons.

if __name__ == "__main__":
    try:
        # If your XBee is somewhere else, change this.
        xb = XBee("/dev/xbee", 1200)
    except:
        print("XBee not connected. Try plugging it in or changing the default from /dev/xbee.")
        exit()

    # two payloads, 0.5L of ballast.
    p1 = Payload(PROFILE, MASS1, 0.5, "P1")
    p2 = Payload(PROFILE, MASS2, 0.5, "P2")

    # First order of business: figure out who's who.
    print("Performing initial peering...")
    bmp.init_peering(p1, p2, xb)

    # Ok, we have two payloads.
    # Wait for one to request an altitude.
    print("Waiting for initial altitude requests...")

    # create plot of the flight
    lp = LivePlot()
    p1_line = lp.create_line()
    p2_line = lp.create_line()

    launched = False
    time_launched = None
    p1_ready = False
    p2_ready = False
    p1_landed = False
    p2_landed = False
    while((p1_landed == False) or (p2_landed == False)):
        msg = None
        # just listen. the payloads will ask for altitudes.
        while(msg == None):
            msg = xb.rx()

        parsed = bmp.parse(msg)
        msg_type = parsed[0]

        if (msg_type == bmp.MSG_TYPES['WAT_REQUEST']):
            # Say "I am a simulator"
            addr = int.from_bytes(parsed[2], byteorder="big")
            xb.tx(parsed[1], addr)
        elif (msg_type == bmp.MSG_TYPES['ALT_REQUEST']):
            # Time to simulate.
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
                alt_m = cur_p.alt()/10.0 # decimeters to meters
                speed = (alt_m-last_alt_m)/(cur_time-last_time)
                if (speed == None):
                    speed = 0
                print("{0} {1}: alt={2:.1f}m speed={3:.2f}m/s mass={4:.3f}kg drop_time={5}".format(timestamp,cur_p.name,alt_m,speed,cur_p.mass,ballast_time_ms))
                alt = cur_p.alt()
                cur_p.last_alt = alt
                # send the payload's alt
                xb.tx("s" + str(int(alt)), cur_p.addr)
                # update graph
                if (time_launched is not None):
                    if (cur_p.name == "P1"):
                            lp.update_line(cur_time - time_launched, alt_m, p1_line)
                    elif (cur_p.name == "P2"):
                        lp.update_line(cur_time - time_launched, alt_m, p2_line)
            else:
                # if alt is None, the payload finished simulation.
                if (cur_p.addr == p1.addr):
                    p1_landed = True
                elif (cur_p.addr == p2.addr):
                    p2_landed = True
                continue
            
            if (p1_ready and p2_ready and launched == False):
                launched = True
                time_launched = time.time()
                time_elapsed = 0
                print("Launched!")

        elif (msg_type == bmp.MSG_TYPES['WAT_REQUEST']):
            # Respond, saying "I am the simulator".
            addr = int.from_bytes(parsed[2], byteorder="big")
            xb.tx(parsed[1], addr)

        '''
        elif (msg_type == bmp.MSG_TYPES['PAYLOAD_ALT']):
            if (addr == p1.addr):
                p1_ready = True
                cur_p = p1
            elif (addr == p2.addr):
                p2_ready = True
                cur_p = p2
        '''

