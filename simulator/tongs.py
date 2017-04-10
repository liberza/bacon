#!/usr/bin/python3
import numpy as np
from datetime import datetime, timedelta
import time
import binascii
import websockets
import asyncio
import json

# local imports
from payload import Payload
from xbee import XBee
from liveplot import LivePlot
import bmp

# make this commandline args at some point. for now, this is completely fine.
MASS1 = 3.5
MASS2 = 3.7
#BALLAST = 0.581 # half liter bottle capacity, minus 10ml
BALLAST = 1.2 # 2L bottle filled halfway with 0.8kg/L isopropyl alcohol
PROFILE = "profiles/umhab52.json"

# Let's simulate some balloons.

async def run_sim(websocket, path):
    try:
        # If your XBee is somewhere else, change this.
        xb = XBee("/dev/xbee", 1200)
    except:
        print("XBee not connected. Try plugging it in or changing the default from /dev/xbee.")
        exit()

    # two payloads
    p1 = Payload(PROFILE, MASS1, BALLAST, "P1")
    p2 = Payload(PROFILE, MASS2, BALLAST, "P2")

    # First order of business: figure out who's who.
    print("Performing initial peering...")
    bmp.init_peering(p1, p2, xb)

    # Ok, we have two payloads.
    # Wait for one to request an altitude.
    print("Waiting for initial altitude requests...")

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
                #cur_p.adjust_time(cur_time)
                p1.adjust_time(cur_time)
                p2.adjust_time(cur_time)
            else:
                #cur_p.last_request_time = cur_time
                p1.last_request_time = cur_time
                p2.last_request_time = cur_time
            # Send them their current alt.
            if (cur_p.alt() is not None):
                alt = cur_p.alt()
                cur_p.last_alt = alt
                # send the payload's alt
                xb.tx("s" + str(int(alt)), cur_p.addr)

                timestamp = datetime.fromtimestamp(cur_time).strftime("[%H:%M:%S]")
                #alt_m = cur_p.alt()/10.0 # decimeters to meters
                #speed = (alt_m-last_alt_m)/(cur_time-last_time)
                p1_alt_m = p1.alt()/10.0
                p2_alt_m = p2.alt()/10.0
                #if (speed == None):
                #    speed = 0
                #print("{0} {1}: alt={2:.1f}m speed={3:.2f}m/s mass={4:.3f}kg drop_time={5}".format(timestamp,cur_p.name,alt_m,speed,cur_p.mass,ballast_time_ms))
                #print("P1 alt: {0}, P2 alt: {1}, Delta alt: {2}, P1 mass: {3}, P2 mass: {4}".format(p1_alt_m, p2_alt_m, p1_alt_m - p2_alt_m, p1.mass, p2.mass))
                await websocket.send('{{"p1_a": {0}, "p2_a": {1}, "p1_m": {2}, "p2_m": {3}, "dist": {4}}}'.format(p1_alt_m, p2_alt_m, p1.mass, p2.mass, p1_alt_m - p2_alt_m))

                # update graph
                #if (time_launched is not None):
                    #if (cur_p.name == "P1"):
                        #lp.update_line(cur_time - time_launched, alt_m, p1_line)
                    #elif (cur_p.name == "P2"):
                        #lp.update_line(cur_time - time_launched, alt_m, p2_line)
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


if __name__ == "__main__":
    # ASCII art adapted from www.chris.com/ASCII/index.php?art=objects/balloons
    print('''
          ,'````',
    .`',.'        `..'``',      ____
  ,'                     `', .-'    '-.                                 
 ;                        ..' /)       '.         ,'``',  ,..,   ,..,
  ',            ,.,    .'`/  (/          \     ,.'      '`    '.'    ;
    '.,.`'.,,,.`   `''`  ;                ;  ,`                       ',
                         |                | ;                           ;
                         |                | `.,     ;    ,...         ,'
             ____        ;                ;    `.,.' `,.'    `,.     ;
          .-'    '-.      \              /                      `.,,'
        .' /)       '.     '.          .'   
       /  (/          \      '-._  _.-'      
      ;                ;         )(           
      |                |        (__)          
      |    ( ͡° ͜ʖ ͡°)    |         /            ╔╦╗╔═╗╔╗╔╔═╗╔═╗
      ;                ;         |             ║ ║ ║║║║║ ╦╚═╗
       \              /          ;             ╩ ╚═╝╝╚╝╚═╝╚═╝
        '.          .'          /     The on-ground simulator for BACON
          '-._  _.-'          |--|        
              )(              |  |    
             (__)             |__|     
              |                ;     
              |                /
              |               ;  
              ;               ;
''')
    print("Waiting for websocket connection...")
    start_server = websockets.serve(run_sim, '127.0.0.1', 5678)
    asyncio.get_event_loop().run_until_complete(start_server)
    asyncio.get_event_loop().run_forever()
