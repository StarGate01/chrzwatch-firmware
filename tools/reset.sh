#!/bin/bash

echo "Spamming openocd connection, press SWD reset button"

RET="1"
while [ $RET = "1" ]; do
    openocd -d2 -f interface/stlink-v2.cfg -c "transport select hla_swd" -f target/nrf52.cfg
    RET=$?
done