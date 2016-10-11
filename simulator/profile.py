#!/usr/bin/python3
import json
import numpy as np


class FlightProfile():
    '''
    Instance contains altitude data for a balloon flight. Use alt() method
    to get interpolated data.
    '''

    def __init__(self, filename, mass):
        self.parse_profile(filename)
        self.mass = mass

    def parse_profile(self, filename):
        '''
        Parse a JSON file containing an ascent profile.
        '''

        with open(filename, 'r') as data_file:
            profile = json.load(data_file)
        self.alts = np.array(profile['data'])
        self.timestep = profile['timestep']
        self.times = np.arange(0, self.alts.size*self.timestep, self.timestep)

    def alt(self, s):
        '''
        Returns the altitude at the desired time.
        s is the time in seconds, with 0 being the beginning
        of the flight.
        '''

        index = s / self.timestep

        # alt = None if seconds is outside of the flight time.
        if (index > self.alts.size):
            alt = None
        elif (index < 0):
            alt = None

        # otherwise, linearly interpolate between the two closest values.
        else:
            alt = np.empty
            alt = np.interp(index, self.times, self.alts)

        return alt
            
if __name__ == '__main__':
    # initialize FlightProfiles. 'PAYLOAD_X_ID' is the digimesh ID of payload X.
    fp1 = FlightProfile('profiles/umhab48.json', 'PAYLOAD_1_ID')
    fp2 = FlightProfile('profiles/umhab48.json', 'PAYLOAD_2_ID')
    fp1.mass = 5 # kg
    fp2.mass = 4 # kg
    xbee = XBee.init('/dev/xbee', ‘1200’)  # initialize serial for XBee, 1200baud
    timestep = 5    # transmit altitude every 5 seconds
    ft = 0      # flight time starts at 0
    while(True):
        # Wait for payloads to request an altitude, send it, and update the
        # payload’s mass. Add noise into the system for realistic simulation.
        if (alt_request(fp1)):
            XBee.sendAlt(fp1.id, fp1.adj_alt(ft))
            fp1.mass -= XBee.getBallastDropped(fp1.id)*mass_noise()
            ft += timestep
        elif (alt_request(fp2)):
            XBee.sendAlt(fp2.id, fp2.adj_alt(ft))
            fp2.mass -= XBee.getBallastDropped(fp2.id)*mass_noise()
            ft += timestep




    print(fp.timestep)
    print(fp.alts)
    print(fp.alts.size)
    print(fp.times.size)
    print(fp.alt(24))

