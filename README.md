# CHRZ Watch Firmware

Custom firmware for the NRF52 based smartwatch I6HRC using the Mbed framework.

## Development setup

Install **Visual Studio Code** and the **PlatformIO** extension. Then use the `i6hrc` env for deployment, or the `nrf52_dk` env for debugging on a NRF52-DK board.

Use `doxygen` to generate documentation.

## Modding a I6HRC watch

Solder the SWD testpoints to the unused USB data lines like this: https://www.youtube.com/watch?v=0Fu-VSuKHEg .

You might want to fabricate a custom USB to ISP adapter.

### Flashing the watch

Flashing should work with any SWD capeable programmer, like for example the ST-Link V2. If the watch refuses to flash, hangs or is stuck in a bootloop, try connecting to it using OpenOCD (http://openocd.org/) while spamming the reset button (or short `SWDIO` to `VCC`). 

```
openocd -d2 -f interface/stlink-v2.cfg -c "transport select hla_swd" -f target/nrf52.cfg
```

On a successful connection OpenOCD displays something like "`Info : nrf52.cpu: hardware has 6 breakpoints, 4 watchpoints`". Then open up GDB via telnet and halt the core using `reset halt`.

```
telnet localhost 4444
```

You can then exit the OpenOCD instance if you want to use a other tool for flashing (e.g. PlatformIO). The core should hang at the reset vector and accept SWD connections and flash commands reliably.

## Thanks to and pinmap source

http://atcnetz.blogspot.com/2019/02/arduino-auf-dem-fitness-tracker-dank.html

https://www.mikrocontroller.net/topic/467136