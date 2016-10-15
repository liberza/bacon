#!/usr/bin/python3
import serial
import queue

# class for managing XBee API mode 
class XBee():
    FRAME_DELIM = b'\x7E'
    ESCAPE = b'\x7D'
    XON = b'\x11'
    XOFF = b'\x13'

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

    def tx(data, dest=0x000000000000FFFF, opts=0x00):
        pass

    def escape(data):
        escaped = b''
        for byte in data:
            if byte in SPECIAL_BYTES:
                escaped += ESCAPE
                escaped += byte ^ 0x20
            else:
                escaped += byte
