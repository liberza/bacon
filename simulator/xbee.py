#!/usr/bin/python3
import serial
import queue
import time
import binascii

# class for managing XBee API mode 
class XBee():
    SPECIAL_BYTES = {
        'FRAME_DELIM':  0x7E,
        'ESCAPE':       0x7D,
        'XON':          0x11,
        'XOFF':         0x13,
    }

    FRAME_TYPES = {
        'AT':           0x08,
        'AT_QPV':       0x09,
        'TX':           0x10,
        'EXPLICIT_TX':  0x11,
        'REMOTE':       0x17,
        'AT_RESP':      0x88,
        'MODEM_STATUS': 0x8A,
        'TX_STATUS':    0x8B,
        'ROUTE_INFO':   0x8D,
        'RX':           0x90,
        'EXPLICIT_RX':  0x91,
        'NODE_ID':      0x95,
        'REMOTE_RESP':  0x97,
    }

    BUF_FULL_TIMEOUT = 5
    BUF_GET_TIMEOUT = 10

    rx_queue = queue.Queue()

    rx_buf = bytearray()

    def __init__(self, devfile, baud):
        self.serial = serial.Serial(port=devfile, 
                                    baudrate=baud,
                                    parity=serial.PARITY_NONE,
                                    stopbits=serial.STOPBITS_ONE,
                                    bytesize=serial.EIGHTBITS
                                    )
        self.serial.flush()
        # need to do some initialization here.
        # enter AT mode and get the address of the xbee. upper 32 bits:"ATSH" lower 32 bits:"ATSL"
        # also get the max payload size with "ATNP"
        self.addr = 0x13A2004146764D
        self.max_payload = 100

    def tx(self, data, dest=0x000000000000FFFF, opts=0x00):
        '''
        data is an unescaped string.
        dest is the 64-bit digimesh address to tx to.
        opts are the frame options.
        '''
        if (len(data) > self.max_payload):
            print("data too long, splitting frames is not supported yet.")

        frame_size = len(data) + 14     # tx api frame has 14 bytes overhead
        frame = bytearray(((frame_size >> 8) & 0x0FF, 
                           (frame_size & 0x0FF),
                           self.FRAME_TYPES['TX'],
                           0x01,
                           # destination
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
        frame.append(0xFF - (sum(frame[2:]) & 0x0FF))
        
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
        received = False
        num_bytes = self.serial.in_waiting
        self.rx_buf.extend(self.serial.read(num_bytes))
        sequence = self.rx_buf.split(bytes(b'\x7E'))        
        #print(sequence)
        #sequence =  [d+e for e in rx_buf.split(d) if e != ""]
        #print(self.rx_buf)
        for s in sequence:
            frame = self.validate_frame(s)
            if (frame is not None):
                self.rx_queue.put(frame, self.BUF_FULL_TIMEOUT)
                received = True

        if (received == True):
            self.rx_buf = bytearray()
            return self.rx_queue.get()

        else:
            return None

    def parse_frame(self, frame):
        for frametype, value in self.FRAME_TYPES.items():
            if frame[2] == value:
                break;
        #print(frame)

        if (value == self.FRAME_TYPES['AT_RESP']):
            pass
        elif (value == self.FRAME_TYPES['MODEM_STATUS']):
            pass
        elif (value == self.FRAME_TYPES['TX_STATUS']):
            pass
        elif (value == self.FRAME_TYPES['ROUTE_INFO']):
            pass
        elif (value == self.FRAME_TYPES['RX']):
            source = frame[3:10]
            opts = frame[13]
            data = frame[14:-1]
            return data
        elif (value == self.FRAME_TYPES['EXPLICIT_RX']):
            pass
        elif (value == self.FRAME_TYPES['TX']):
            source = frame[5:12]
            data = frame[16:-1]
            print(str(source))
            print(str(data))
        elif (value == self.FRAME_TYPES['NODE_ID']):
            pass
        elif (value == self.FRAME_TYPES['REMOTE_RESP']):
            pass
        else:
            raise Exception("Frame has invalid frame type.")

    def escape(self, data):
        escaped = bytearray()
        for byte in data:
            if byte in self.SPECIAL_BYTES.values():
                escaped.append(self.SPECIAL_BYTES['ESCAPE'])
                escaped.append(byte ^ 0x20)
            else:
                escaped.append(byte)
        return escaped

    def unescape(self, data):
        unescaped = bytearray()
        last_escaped = False
        # create an iterator from range() so we can use next() on it
        q = iter(range(len(data)))
        for i in q:
            if (last_escaped):
                unescaped.append(data[i] ^ 0x20)
                last_escaped = False
            elif data[i] == self.SPECIAL_BYTES['ESCAPE']:
                last_escaped = True
            else:
                unescaped.append(data[i])
        return unescaped

    def validate_frame(self, frame):
        # do frame validation here...
        frame = self.unescape(frame)
        if (frame is not None):
            if (len(frame) < 5):
                # already know this isn't a valid frame: not enough overhead
                frame = None
                return None
            else:
                frame_len = (frame[0] << 8) | frame[1]
            # make sure frame length is accurate
            if (len(frame[2:-1]) != frame_len):
                #print('invalid len: ' + str(len(frame[2:-1])) + ' act: ' + str(frame_len))
                frame = None
            # check checksum
            elif ((sum(frame[2:]) & 0x0FF) != 0xFF):
                #print('invalid checksum: ' + '{:02X}'.format(sum(frame[2:]) & 0x0FF))
                #print(frame)
                frame = None
        return frame
                
    def get_frame(self):
        '''
        Returns the oldest frame in the queue, or None.
        '''
        if (self.rx_queue.empty() == False):
            return self.rx_queue.get(block=False)
        else:
            return None
        

if __name__ == '__main__':
    xb = XBee('/dev/ttyUSB0', 1200)
    '''
    data = bytearray((b'ASDFLOL qwerty \x11 hello {}{}'))
    print(data)
    new = xb.escape(data)
    print(new)
    unescaped = xb.unescape(new)
    print(unescaped)
    '''
    while(True):
        r = xb.rx()
        while (r == None):
            r = xb.rx()
        h = binascii.hexlify(r[16:-1])
        q = [h[i:i+2] for i in range(0, len(h), 2)]
        c = ""
        for d in q:
            c += " " + d.decode()
        print(c)
        print("---")
        #xb.parse_frame(r)
