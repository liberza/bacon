#!/usr/bin/python3
from xbee import XBee

if __name__ == "__main__":
    try:
        xb = XBee("/dev/xbee", 1200)
    except:
        print("XBee not connected. Try plugging it in or changing the defualt from /dev/xbee.")
        exit()

    while True:
        msg = None
        while (msg==None):
            msg = xb.rx()
        if msg[2] == 0x90:
            print(msg[14:-1].decode())
        else:
            print(msg[2])
