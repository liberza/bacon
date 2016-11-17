#!/usr/bin/python3
from xbee import XBee
from datetime import datetime, timedelta


# BMP - BACON Message Protocol

TIMEOUT = timedelta(seconds=3)

'''
Here's how this works:
    Uppercase letter: request
    Lowercase letter: response

All message types are in MSG_TYPES. Simulator doesn't care about
PAYLOAD_ALT. Simulator response to WAT_REQUEST with a WAT_REPLY + "S" for "sim".
'''

MSG_TYPES = {
    'PAYLOAD_ALT':  'a',        # current payload altitude
    'ALT_REQUEST':  'S',        # simulated altitude request. includes compensation amount.
    'SIM_ALT':      's',        # simulated altitude.
    'WAT_REQUEST':  'W',        # 'Who Art Thou' request. think ARP, but instead of asking for one addr, it asks for all.
    'WAT_REPLY':    'w',        # 'Who Art Thou' reply. send "wS" if sim, "wP" if payload.
    'PEER_ADDR':    'p',        # Just to prove that the payloads do, in fact, communicate.
}

def sim_alt_str(alt):
    ''' alt should be a signed 32-bit integer. '''
    print(MSG_TYPES['SIM_ALT'] + alt)

def parse(msg):
    ''' msg should be a RX frame. msg[14] is the first data byte. '''
    if msg[2] != 0x90:
        return (None,)

    msg_type = chr(msg[14])
    if msg_type == MSG_TYPES['ALT_REQUEST']:
        # payload is requesting altitude. msg[14:-1] is the time it
        # has opened its valve since the last request, in ms.
        # FIXME: make msg[3:11] automatically an int.
        ret = (MSG_TYPES['ALT_REQUEST'], msg[3:11], int(msg[15:-1].decode()))

    elif msg_type == MSG_TYPES['WAT_REQUEST']:
        # ret[1] is the string to send back as a WAT_REPLY. ret[2] is the addr to respond to.
        # we are a simulator, hence "S"
        ret = (MSG_TYPES['WAT_REQUEST'], MSG_TYPES['WAT_REPLY'] + "S", msg[3:11])

    elif msg_type == MSG_TYPES['WAT_REPLY']:
        # ret[1] is the device type (probably P for payload) and ret[2] is its address.
        ret = (MSG_TYPES['WAT_REPLY'], chr(msg[15]), msg[3:11])

    elif msg_type == MSG_TYPES['PEER_ADDR']:
        ret = (MSG_TYPES['PEER_ADDR'], msg[3:11], msg[16:-1])

    else:
        # Not a bmp message. At least not one we, the simulator, are interested in.
        ret = (None,)

    return ret

# Do peering between the sim and payloads, and confirm that the payloads are peered as well.
def init_peering(p1, p2, xb):
    p1_peered = False
    p2_peered = False
    xb.tx(MSG_TYPES['WAT_REQUEST'], XBee.BROADCAST)
    # Loop until we confirm the addresses of both payloads, and that they peered with eachother.
    while ((p1.addr == None) or (p2.addr == None) or (p1_peered == False) or (p2_peered == False)):
        msg = None
        start = datetime.now()
        while (msg == None):
            now = datetime.now()
            if (now - start >= TIMEOUT):
                # Resend the WAT request to the network
                xb.tx(MSG_TYPES['WAT_REQUEST'], XBee.BROADCAST)
                start = datetime.now()

            msg = xb.rx()

        # Got a message.
        if ((msg != None) and (msg[2] == XBee.FRAME_TYPES['RX'])):
            parsed = parse(msg)
            if (parsed[0] == MSG_TYPES['WAT_REPLY']) and (parsed[1] == 'P'):
                addr = int.from_bytes(parsed[2], byteorder="big")
                if (p1.addr == None):
                    p1.addr = addr
                    print("P1 Address: {:016x}".format(p1.addr))
                elif ((p2.addr == None) and (addr != p1.addr)):
                    p2.addr = int.from_bytes(parsed[2], byteorder="big")
                    print("P2 Address: {:016x}".format(p2.addr))
                
            elif (parsed[0] == MSG_TYPES['WAT_REQUEST']):
                # Respond, saying "I am the simulator".
                addr = int.from_bytes(parsed[2], byteorder="big")
                xb.tx(parsed[1], addr)
            elif (parsed[0] == MSG_TYPES['PEER_ADDR']):
                payload_addr = int.from_bytes(parsed[1], byteorder="big")
                peer_addr = int.from_bytes(parsed[2], byteorder="big")
                if ((payload_addr == p1.addr) and (p1_peered == False)):
                    print("P1 ({:016x}) peered successfully with P2 ({:016x})".format(payload_addr, peer_addr))
                    p1_peered = True
                elif ((payload_addr == p2.addr) and (p2_peered == False)):
                    print("P2 ({:016x}) peered successfully with P1 ({:016x})".format(payload_addr, peer_addr))
                    p2_peered = True

        msg = None
