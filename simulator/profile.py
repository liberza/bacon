#!/usr/bin/python3
import json
import numpy as np


class FlightProfile():
    '''
    Instance contains altitude data for a balloon flight. Use alt() method
    to get interpolated data.
    '''

    def __init__(self, filename):
        self.parse_profile(filename)

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
    fp = FlightProfile('umhab48.txt')
    print(fp.timestep)
    print(fp.alts)
    print(fp.alts.size)
    print(fp.times.size)
    print(fp.alt(24))
