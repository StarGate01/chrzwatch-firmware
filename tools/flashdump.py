#!/usr/bin/env python3

import serial

print("Starting ROM dump.\n")

# Setup serial, dump file and trigger dump
s = serial.Serial('/dev/ttyUSB0', 921600)
f = open('dump.bin', 'wb')
s.write(b'\x64')
s.flush()

# Read 255 byte chunks
total_size = 0
while total_size < 0x1FFFFF:
    f.write(s.read(255))
    total_size += 255
    print("Bytes received: " + str(total_size) + ", " + str(round((total_size / 0x1FFFFF) * 100.0, 2)) + "% done")

# Read rest
f.write(s.read(0x1FFFFF - total_size))

# Finish
print("Done dumping " + str(total_size) + " bytes.")
f.close()