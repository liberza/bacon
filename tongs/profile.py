#!/usr/bin/python3

import json

class AltitudeProfile():
    def __init__(self, filename):
        self.parseProfile(filename)

    def parseProfile(self, filename):
        with open(filename, 'r') as data_file:
            profile = json.load(data_file)
        self.data = profile['data']
        self.timestep = profile['timestep']
        

if __name__ == '__main__':
    ap = AltitudeProfile('umhab52.txt')
    print(ap.timestep)
    print(len(ap.data))
