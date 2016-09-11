#!/usr/bin/python3

def parse(inFile):
    f = open(inFile, 'r')
    for line in f:
        data = line.split(',')
        if (data[0] == '$GPGGA'):
            print(data[9].rstrip() + ',')

if __name__ == '__main__':
    parse('UMHAB-52-N1YIP')
