# CHRZ Watch Firmware

Custom firmware for the NRF52 based smartwatch I6HRC using the ARM Mbed 5.14 RTOS.

## State of the project

Bluetooth

- [x] Complete BLE Stack
- [x] Battery GATT profile
- [x] Heartrate GATT profile
- [x] Current time GATT profile
- [ ] Steps & cadence GATT profile
- [ ] Phone alerts / Immediate alert GATT

Hardware interfacing

- [x] Display driver
- [x] Vibration
- [x] Charging detection
- [x] Touch buttons
- [x] Battery voltage sensor
- [ ] Step sensor driver
- [ ] Heartrate sensor driver

Power saving

- [x] Low power idle thread
- [x] Energy saving display
- [x] Energy saving touch input
- [ ] Endurance tests & verification

Other

- [x] Timekeeping
- [ ] Step detection algorithm
- [x] Basic UI
- [ ] Fancy UI

## Hardware overview

 - CPU: **NRF52832** with 512K ROM, 64K RAM
   - General: https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52832/Getting-started
   - Datasheet: https://infocenter.nordicsemi.com/pdf/nRF52832_PS_v1.0.pdf
   - OS: https://os.mbed.com/
 - Display: 0.96 inch LCD with **ST7735** driver
   - Datasheet: https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf
   - Driver: https://platformio.org/lib/show/7412/Adafruit_ST7735_Mini
 - Acceleration sensor: **KX023**
   - General: https://www.kionix.com/product/KX023-1025
   - Datasheet: http://kionixfs.kionix.com/en/datasheet/KX023-1025%20Specifications%20Rev%2012.0.pdf
   - Driver: TBD
 - Heart rate sensor: **AFE4404**
   - General: https://www.ti.com/product/AFE4404
   - Datasheet: https://www.ti.com/lit/ds/symlink/afe4404.pdf?ts=1597861981560&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FAFE4404
   - Driver: TBD
 - Font ROM: **GT24L24A2Y**
   - General: https://lcsc.com/product-detail/_Gotop-GT24L24A2Y_C124690.html
   - Datasheet: https://datasheet.lcsc.com/szlcsc/1912111436_Gotop-GT24L24A2Y_C124690.pdf
   - Driver: N/A, currently unused
 - Ambient light sensor: Some **photodiode** combined with a **LED**
   - Driver: Simple ADC
  
See `doc/pinout.png` for the pin mapping by *Aaron Christophel*.

## Development setup

Install **Visual Studio Code** and the **PlatformIO** extension. Then use the `i6hrc` env for deployment, or the `nrf52_dk` env for debugging on a NRF52-DK board.

Use `doxygen` or the "Build Documentation" task to generate documentation.

### Modding a I6HRC watch

Solder the SWDCLK and SWDIO testpoints to the unused USB data lines. You might want to fabricate a custom USB to ISP adapter.

Do NOT connect the watch to a USB host port or "smart" (e.g. Quick Charge) adapter, because the USB protocol will confuse the SWD interface and reset/fault the chip. Instead, use a "dumb" charger, which only uses the outer USB 5V pins.

### Flashing the watch

Flashing should work with any SWD capable programmer, like for example the **ST-Link V2** (the cheap clones work too). 

If the watch refuses to flash, hangs in low power mode or is stuck in a bootloop, try connecting to it using **OpenOCD** (http://openocd.org/) while spamming the reset button on your adapter (or short `SWDIO` to `VCC`). This should not be needed during normal flashing and execution.

```
openocd -d2 -f interface/stlink-v2.cfg -c "transport select hla_swd" -f target/nrf52.cfg
```

On a successful connection OpenOCD displays something like "`Info : nrf52.cpu: hardware has 6 breakpoints, 4 watchpoints`". The script `tools/reset.sh` or the task "Reset Target" automates this spamming.

Optinally, open up GDB via telnet and reset the core using `reset halt` via `telnet localhost 4444`.

You can then exit the OpenOCD instance if you want to use a other tool for flashing (e.g. PlatformIO). The core should run normally (or hang at the reset vector if you reset it) and accept SWD connections and flash commands reliably. The PlatformIO flash tool should leave the core in a useable, running state.

## Connecting to a phone

You can use the Android app "**nRF Connect**" (https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) to test the Bluetooth functions. Sometimes, connecting takes multiple tries.

## Stock firmware

A dump of the stock ROM can be found at `doc/stock_rom_dump.bin` (armv7le) for decompiling (e.g. using **Ghidra**: https://ghidra-sre.org/) or restoring the watch (untested).

## Thanks to and credits

Thanks to *Aaron Christophel* for providing instructions on how to modify the hardware, mapping out the pins and providing some demo Arduino code.

 - https://github.com/atc1441
 - http://atcnetz.blogspot.com/2019/02/arduino-auf-dem-fitness-tracker-dank.html
 - https://www.mikrocontroller.net/topic/467136
 - https://www.youtube.com/watch?v=0Fu-VSuKHEg 

### Library credits:

 - ARM Mbed RTOS and API: https://os.mbed.com/
 - The `CurrentTimeService` module of the `BLE_GATT_Services` library (https://platformio.org/lib/show/7372/BLE_GATT_Services) is based on `BLE_CurrentTimeService` by *Takehisa Oneta*: https://os.mbed.com/users/ohneta/code/BLE_CurrentTimeService/
   - Deferred calls in ISR context to EventQueue
   - Added documentation
 - The `Adafruit_ST7735_Mini` library (https://platformio.org/lib/show/7412/Adafruit_ST7735_Mini) is based on the `Adafruit_ST7735` library by *Andrew Lindsay*: https://platformio.org/lib/show/2150/Adafruit_ST7735, which in turn is a port of a library by Adafruit: https://github.com/adafruit/Adafruit-ST7735-Library
   - Added support for the `R_MINI160x80` display type
   - Added documentation
   - Added an explicit dependency to `Adafruit_GFX` port by *Andrew Lindsay*: https://platformio.org/lib/show/2147/Adafruit_GFX, which is a port of https://github.com/adafruit/Adafruit-GFX-Library
