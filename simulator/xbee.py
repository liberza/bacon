#!/usr/bin/python3
import serial
import queue

# class for managing XBee API mode 
class XBee():
    FRAME_DELIM = 0x7E
    ESCAPE = 0x7D
    XON = 0x11
    XOFF = 0x13

    SPECIAL_BYTES = (FRAME_DELIM, ESCAPE, XON, XOFF)
    
    rx_bytes = bytearray()
    rx_queue = queue.Queue()

    def __init__(self, devfile, baud):
        self.serial = serial.Serial(port=devfile, 
                                    baudrate=baud,
                                    parity=serial.PARITY_NONE,
                                    stopbits=serial.STOPBITS_ONE,
                                    bytesize=serial.EIGHTBITS
                                    )

    def tx(self, data, dest=0x000000000000FFFF, opts=0x00):
        pass

    def escape(self, data):
        escaped = bytearray()
        for byte in data:
            if byte in self.SPECIAL_BYTES:
                escaped.append(self.ESCAPE)
                escaped.append(byte ^ 0x20)
            else:
                escaped.append(byte)
        return escaped

    def unescape(self, data):
        unescaped = bytearray()
        # create an iterator from range() so we can use next() on it
        q = iter(range(len(data)))
        for i in q:
            if data[i] == self.ESCAPE:
                unescaped.append(data[i+1] ^ 0x20)
                next(q, None) # skip the next iteration
            else:
                unescaped.append(data[i])
        return unescaped
                

if __name__ == '__main__':
    xb = XBee('/dev/xbee', 1200)
    data = bytearray((b'ASDFLOL qwerty \x11 hello {}{}'))
    print(data)
    new = xb.escape(data)
    print(new)
    unescaped = xb.unescape(new)
    print(unescaped)
