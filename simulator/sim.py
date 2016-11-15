#!/usr/bin/python3
import numpy as np
from .flight import Flight
from .xbee import XBee
from .bmp import BMP 


# Let's simulate some balloons.

if __name__ == '__main__':
    # If your XBee is somewhere else, change this.
    xb = XBee('/dev/xbee', 1200)
    # First order of business: figure out who's who.
