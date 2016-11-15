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
BROADCAST = 0x000000000000FFFF
TIMEOUT = timedelta(seconds=5)

# Send a Who Art Thou request, set addresses for two payloads.
def wat(p1, p2):
    p1_peered = False
    p2_peered = False
    xb.tx(bmp.MSG_TYPES['WAT_REQUEST'], BROADCAST)
    while ((p1.addr == None) or (p2.addr == None) or (p1_peered == False) or (p2_peered == False)):
        msg = None
        start = datetime.now()
        while (msg == None):
            now = datetime.now()
            if (now - start >= TIMEOUT):
                print("Waiting for payload response...")
                xb.tx(bmp.MSG_TYPES['WAT_REQUEST'], BROADCAST)
                start = datetime.now()

            msg = xb.rx()

        # Got a message.
        if ((msg != None) and (msg[2] == XBee.FRAME_TYPES['RX'])):
            parsed = bmp.parse(msg)
            if (parsed[0] == bmp.MSG_TYPES['WAT_REPLY']) and (parsed[1] == 'P'):
                addr = int.from_bytes(parsed[2], byteorder="big")
                if (p1.addr == None):
                    p1.addr = addr
                    print("P1 Address: {:016x}".format(p1.addr))
                elif ((p2.addr == None) and (addr != p1.addr)):
                    p2.addr = int.from_bytes(parsed[2], byteorder="big")
                    print("P2 Address: {:016x}".format(p2.addr))
                elif (p1.addr == addr):
                    pass
                elif (p2.addr == addr):
                    pass
                else:
                    print("Got an extra address. Is there a third payload?")
                
            elif (parsed[0] == bmp.MSG_TYPES['WAT_REQUEST']):
                # Respond.
                addr = int.from_bytes(parsed[2], byteorder="big")
                xb.tx(parsed[1], addr)
            elif (parsed[0] == bmp.MSG_TYPES['PEER_ADDR']):
                payload_addr = int.from_bytes(parsed[1], byteorder="big")
                peer_addr = int.from_bytes(parsed[2], byteorder="big")
                if ((payload_addr == p1.addr) and (p1_peered == False)):
                    print("P1 ({:016x} peered successfully with P2 ({:016x})".format(payload_addr, peer_addr))
                    p1_peered = True
                elif ((payload_addr == p2.addr) and (p2_peered == False)):
                    print("P2 ({:016x} peered successfully with P1 ({:016x})".format(payload_addr, peer_addr))
                    p2_peered = True
            else:
                print(msg)

        msg = None
    

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
