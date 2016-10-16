#!/usr/bin/python3
import serial
import queue

# class for managing XBee API mode 
class XBee():
    FRAME_DELIM = 0x7E
    ESCAPE = 0x7D
    XON = 0x11
    XOFF = 0x13

    FRAME_TX = 0x10
    FRAME_AT = 0x09
    BUF_FULL_TIMEOUT = 5

    SPECIAL_BYTES = (FRAME_DELIM, ESCAPE, XON, XOFF)
    
    rx_queue = queue.Queue()

    def __init__(self, devfile, baud):
        self.serial = serial.Serial(port=devfile, 
                                    baudrate=baud,
                                    parity=serial.PARITY_NONE,
                                    stopbits=serial.STOPBITS_ONE,
                                    bytesize=serial.EIGHTBITS
                                    )
        # need to do some initialization here.
        # enter AT mode and get the address of the xbee. upper 32 bits:"ATSH" lower 32 bits:"ATSL"
        # also get the max payload size with "ATNP"
        self.addr = 0x00000000000001337
        self.max_payload = 100

    def tx(self, data, dest=0x000000000000FFFF, opts=0x00):
        '''
        data is an unescaped string.
        dest is the 64-bit digimesh address to tx to.
        opts are the frame options.
        '''
        if (len(data) > self.max_payload):
            print("data too long, splitting frames is not supported yet.")

        frame_size = len(data) + 15     # tx api frame has 15 bytes overhead
        frame = bytearray(((frame_size >> 8) & 0x0FF, 
                           (frame_size & 0x0FF),
                           self.FRAME_TX, 
                           # destination
                           (dest >> 64) & 0x0FF,
                           (dest >> 56) & 0x0FF,
                           (dest >> 48) & 0x0FF,
                           (dest >> 40) & 0x0FF,
                           (dest >> 32) & 0x0FF,
                           (dest >> 24) & 0x0FF,
                           (dest >> 16) & 0x0FF,
                           (dest >> 8) & 0x0FF,
                           (dest & 0x0FF),
                           0xFF, # reserved
                           0xFE, # reserved
                           0x00, # broadcast radius (default 0x00 for radius=max hops)
                           0x00, # tx options (default, use whatever is already set)
                           ))

        # append the data
        frame += bytearray(data.encode())

        # append checksum
        frame.append(0xFF - (sum(frame[3:]) & 0x0FF))
        
        # escape the frame
        frame = self.escape(frame)

        # prepend the unescaped delimiter
        frame = bytearray(b'\x7E') + frame

        print(frame)

        return self.serial.write(frame)

    def rx(self):
        '''
        Read bytes from serial if any are waiting.
        Then validate frames and add to rx_queue.
        '''
        num_bytes = self.serial.in_waiting
        incoming = self.serial.read(num_bytes)
        sequence = incoming.split(bytes(b'\x7E'))        
        for s in sequence:
            if (self.valid_frame(s)):
                self.rx_queue.put(s, self.BUF_FULL_TIMEOUT)
            
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

    def valid_frame(self, frame):
        # do frame validation here...
        if frame:
            return True
        else:
            return False
                

if __name__ == '__main__':
    xb = XBee('/dev/xbee', 1200)
    data = bytearray((b'ASDFLOL qwerty \x11 hello {}{}'))
    print(data)
    new = xb.escape(data)
    print(new)
    unescaped = xb.unescape(new)
    print(unescaped)
    xb.tx('lol this uses the api{} heh asdfasdfasdf')
    xb.rx()
    print(xb.rx_queue)
