#!/usr/bin/python3
#include "xbee.py"

# BMP - BACON Message Protocol

class bmp():
    MSG_TYPES = {
        'SIM_ALT':      'S',        # simulated altitude.
        'PAYLOAD_ALT':  'A',        # current payload altitude
        'ALT_REQUEST':  'R',        # altitude request. includes compensation amount.
        'WAT_REQUEST':  'W',        # 'Who Art Thou' request. like ARP but super simple.
        'WAT_REPLY':    'R',        # 'Who Art Thou' reply. send "RS" if sim, "RP" if payload.
    }

    def sim_alt_str(alt):
        ''' alt should be a signed 32-bit integer. '''
        print(MSG_TYPES['SIM_ALT'] + alt)

    def parse_request(req):
        ''' req should be a RX frame. req[15] is the first data byte. '''
        if req[3] != 0x90:
            raise Exception("parse_request(): Non-RX frame passed!")

        if req[15] == MSG_TYPES['ALT_REQUEST']:
            # payload is requesting altitude. req[1:-1] is the time it
            # has opened its valve since the last request, in ms.
            ret = (MSG_TYPES['ALT_REQUEST'], alt_req[1:-1])

        else if req[15] == MSG_TYPES['WAT_REQUEST']:
            # ret[1] is the string to send back as a WAT_REPLY.
            ret = (MSG_TYPES['WAT_REQUEST'], MSG_TYPES['WAT_REPLY'] + "S")

        else if req[15] == MSG_TYPES['WAT_REPLY']:
            ret = (MSG_TYPES['WAT_REPLY'], ret[4:11])

        else:
            return None
