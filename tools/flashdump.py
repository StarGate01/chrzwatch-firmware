#!/usr/bin/env python3

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("localhost", 19021))
f = open('dump.bin', 'wb')

print("Starting ROM dump.")

total_size = -10 # START DUMP length
has_begun = False
while True:
    res = s.recv(1024)
    sres = res.decode("ascii", "ignore")

    if("START DUMP" in sres):
        has_begun = True

    if has_begun:
        f.write(res)
        total_size += len(res)
        print("Bytes received: " + str(total_size))

    if(total_size >= 0x001FFFFF):
        break

# Finish
print("Done dumping " + str(total_size) + " bytes.")
f.close()