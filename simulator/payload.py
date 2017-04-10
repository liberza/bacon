#!/usr/bin/python3
import json
import numpy as np
import random


class Payload():
    '''
    Instance contains altitude data for a balloon flight. Use alt() method
    to get interpolated data.
    '''

    addr = None
    name = "Payload"
    time_index = 0.0 # index into the flight profile data, in seconds.
    timestep = 1.0 # gets larger as weight decreases.
    last_request_time = 0
    last_alt = 0
    
    def __init__(self, filename, mass, ballast, name):
        self.parse_profile(filename)
        self.initial_mass = mass # kg
        self.mass = self.initial_mass # kg
        # ballast is included in total mass.
        self.initial_ballast = ballast # liters 
        self.name = name

    def parse_profile(self, filename):
        '''
        Parse a JSON file containing an ascent profile.
        '''

        with open(filename, 'r') as data_file:
            profile = json.load(data_file)
        # Create an array of int32's. Alt is in decimeters.
        self.alts = (np.array(profile['data'])*10).astype(np.int32)
        self.ref_timestep = profile['timestep'] # s
        self.ref_mass = profile['mass'] # s
        self.times = np.arange(0, self.alts.size*self.ref_timestep, self.ref_timestep)

    def alt(self):
        '''
        Returns the altitude at the desired time.
        s is the time in seconds, with 0 being the beginning
        of the flight.
        '''

        # alt = None if seconds is outside of the flight time.
        if (self.time_index > self.alts.size):
            alt = None
            print("time index > alt size")
            print(self.time_index)
        elif (self.time_index < 0):
            alt = None
            print("time index < 0")
            print(self.time_index)

        # otherwise, linearly interpolate between the two closest values.
        else:
            alt = np.empty
            alt = np.interp(self.time_index, self.times, self.alts)

        return alt

    # Did curve-fitting on HabHub data to come up with timestep adjustment.
    def adjust_time(self, time_elapsed):
        time_delta = time_elapsed - self.last_request_time
        self.last_request_time = time_elapsed
        x = self.ref_mass / self.mass
        self.timestep = 1.0/(-0.0815243*x*x*x + 0.1355*x*x - 0.391461*x + 1.33748611)
        self.time_index += time_delta*self.timestep
        

    def drop_mass(self, ballast_time_ms):
        # experimental results show 4.925ml/s drain rate. with current setup.
        # we give it random +-10% error, because the payload is getting
        # blasted by high winds and the ballast is sloshing around.
        noise = random.uniform(0.9, 1.1)
        new_mass = self.mass - (noise*ballast_time_ms*0.004925/1000)*0.8
        if (new_mass > self.initial_mass - self.initial_ballast):
            self.mass = new_mass
        else:
            self.mass = self.initial_mass - self.initial_ballast

if __name__ == '__main__':
    # initialize Flights. 'PAYLOAD_X_ID' is the digimesh ID of payload X.
    fp1 = Payload('profiles/umhab52.json', 1.2)
    fp2 = Payload('profiles/umhab48.json', 1.4)
    '''
    xbee = XBee.init('/dev/xbee', 1200)  # initialize serial for XBee, 1200baud
    ft = 0      # flight time starts at 0
    cur_payload = None
    while(True):
        # Wait for payloads to request an altitude, send it, and update the
        # payload’s mass. Add noise into the system for realistic simulation.
        req = alt_request_wait();
        if ((req.addr != fp1.addr) and (req.addr != fp2.addr)):
            if (fp1.addr == None):
                fp1.addr = req.addr
                cur_payload = fp1
            else if (fp2.addr == None):
                fp2.addr = req.addr
                cur_payload = fp2
            else:
                print('Got another XBee\'s frame. Maybe change the network id.')

        elif (req.addr == fp1.addr):
            print('got a fp1 alt request')
            cur_payload = fp1
        else:
            print('got a fp2 alt request')
            cur_payload = fp2

        XBee.sendAlt(cur_payload.addr, cur_payload.alt())

        fp1.mass -= XBee.getBallastDropped(fp1.id)*mass_noise()

        ft += timestep

    print(fp.timestep)
    print(fp.alts)
    print(fp.alts.size)
    print(fp.times.size)
    print(fp.alt(24))
    '''
