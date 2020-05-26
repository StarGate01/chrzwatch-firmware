# CHRZ Watch Firmware

Custom firmware for the NRF52 based smartwatch I6HRC using the Mbed framework.

## Usage

Install **Visual Studio Code** and the **PlatformIO** extension. Then use the `i6hrc` env for deployment, or the `nrf52_dk` env for debugging on a NRF52-DK board.

Use `doxygen` to generate documentation.

### Modding a I6HRC watch

Solder the SWD testpoints to the unused USB data lines like this: https://www.youtube.com/watch?v=0Fu-VSuKHEg .

You might want to fabricate a custom USB to ISP adapter.

## Thanks to and pinmap source

http://atcnetz.blogspot.com/2019/02/arduino-auf-dem-fitness-tracker-dank.html

https://www.mikrocontroller.net/topic/467136