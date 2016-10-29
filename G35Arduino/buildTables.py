#!/usr/bin/env python
# encoding: utf-8
"""
untitled.py

Created by Jonny Dyer on 2011-10-29.
Copyright (c) 2011 __MyCompanyName__. All rights reserved.
"""

import sys
import os
from math import *
import numpy as np
import random
import matplotlib.pyplot as plt

FILENAME = 'tables.h'
N_ENTRIES = 200
ENTRIES_CYCLE = 100
T_CONST = 2

def arrayPrint(x):
    the_str = '\t{ '
    for i,item in enumerate(x):
        the_str += str(int(item))
        if i < x.size-1:
            the_str += ','
    
    the_str += '},\n'
    return the_str

def main():
    
    print 'Building lookup tables.',
    
    outfile = open(FILENAME,'w')
    
    outfile.write('#include <avr/pgmspace.h>\n\n#define NUM_WAVE_ENTRIES {0}\nunsigned char wave_table[{0}] PROGMEM = \n{{\n'.format(N_ENTRIES))
    
    np.set_string_function(arrayPrint,False)
    
    # calculate
    time = np.linspace(0,10*pi,N_ENTRIES)
    wave_vals = np.zeros_like(time)
    
    wave_vals = (np.exp(-time / T_CONST) + (0.05 * np.cos(time * 2) + .05) * np.exp(-time / T_CONST / 5)) * 200
    
    if 1:
        plt.plot(time, wave_vals)
        plt.grid(True)
        plt.show()
    
    # Output to file
    for row in range(0,N_ENTRIES):
        outfile.write(str(int(max(0,wave_vals[row]))) + ',\n')
        print '.',
        
    outfile.write('};\n')
    
    outfile.close()
    
    print '\nLookup tables built'
    pass


if __name__ == '__main__':
    main()

