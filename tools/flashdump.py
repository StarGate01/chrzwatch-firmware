#!/usr/bin/env python3

print("Starting ROM dump.\n")

# Setup serial, dump file and trigger dump

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 19021))

f = open('dump.bin', 'wb')

# Read 255 byte chunks
total_size = 0
while True:
    res = s.recv(1024)
    f.write(res)
    total_size += len(res)
    print("Bytes received: " + str(total_size))
    if("END DUMP" in res.decode("ascii", "ignore")):
        break

# Finish
print("Done dumping " + str(total_size) + " bytes.")
f.close()