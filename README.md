# CHRZ Watch Firmware

Custom firmware for the NRF52 based smartwatch I6HRC using the ARM Mbed 5.14 RTOS. Supported by a custom version of the Gadgetbridge Android app.

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
- [x] Heartrate sensor driver

Power saving

- [x] Low power idle thread
- [x] Energy saving display
- [x] Energy saving touch input
- [ ] Energy saving heartrate sensor
- [ ] Endurance tests & verification

Other

- [x] Timekeeping
- [x] Graceful reboot on error
- [ ] Wakeup on wrist turn
- [ ] Step detection algorithm
- [x] Heartrate detection algorithm
- [x] Basic UI
- [ ] Fancy UI
- [x] Basic Android support
- [ ] Complete Android support

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
   - Driver: TBA
 - Heart rate sensor: **AFE4404**
   - General: https://www.ti.com/product/AFE4404
   - Datasheet: https://www.ti.com/lit/ds/symlink/afe4404.pdf
   - Driver: https://platformio.org/lib/show/11099/Heartrate3_AFE4404
 - Font ROM: **GT24L24A2Y**
   - General: https://lcsc.com/product-detail/_Gotop-GT24L24A2Y_C124690.html
   - Datasheet: https://datasheet.lcsc.com/szlcsc/1912111436_Gotop-GT24L24A2Y_C124690.pdf
   - Driver: N/A, currently unused
 - Ambient light sensor: Some **photodiode** combined with a **LED**
   - Driver: Simple ADC
 - Battery voltage sensor: Down-scaled battery voltage (0.3V - 0.4V)
   - Driver: Simple ADC
 - Vibration motor:
   - Driver: Simple GPIO

See `doc/pinout.png` for the pin mapping by *Aaron Christophel*.

## Development setup

Install **Visual Studio Code** and the **PlatformIO** extension. Then use the `i6hrc` env for deployment, or the `nrf52_dk` env for debugging on a NRF52-DK board.

Use `doxygen` or the "Build Documentation" task to generate documentation.

### Modding a I6HRC watch

Solder the SWDCLK and SWDIO testpoints to the unused USB data lines. You might want to fabricate a custom USB to ISP adapter.

Do NOT connect the watch to a USB host port or "smart" (e.g. Quick Charge) adapter, because the USB protocol will confuse the SWD interface and reset/fault the chip. Instead, use a "dumb" charger, which only uses the outer USB 5V pins.

### Unlocking the flash memory

First of all, the flash memory has to be reset in oder for the chip to accept new firmware and debug instructions. This requires lower level SWD access, and thus cant be performed by the cheap ST-Link V2 clones because they only provide high level access.

#### Using a CMSIS-DAP

Either buy any **CMSIS-DAP** capable adapter, or buy a cheap **ST-Link V2** clone (the little USB adapter one), and open it. Then use any SWD capable programmer, like for example *another* ST-Link V2 to reprogram the first one with the firmware for CMSIS-DAP functionality (https://raw.githubusercontent.com/x893/CMSIS-DAP/master/Firmware/STM32/hex/CMSIS-DAP-STLINK21.hex). You can use any ST-compatible flash tool, like for example the ST-Link utility or OpenOCD.

Then, connect the watch to the CMSIS-DAP, and the adapter to your PC via USB. Then connect to it using OpenOCD.

```
$ openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg -c "init; reset halt; nrf5 mass_erase; reset; exit"
```

#### Using a Black Magic Probe

Buy or assemble - *Bluepill* boards (https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html) are available for a few bucks - a **Black Magic Probe** (https://github.com/blacksphere/blackmagic/wiki). Connect the SWD interface to the watch, and the probe via USB to your PC. Then connect to the probe using GDB.

```
$ arm-none-eabi-gdb
(gdb) target extended-remote /dev/ttyACM0
(gdb) set non-stop on 
(gdb) mon swdp_scan
(gdb) mon erase_mass
```

### Uploading new firmware

Flashing the unlocked chip should then work with any basic SWD capable programmer, like for example the **ST-Link V2** (the cheap clones work too). The **CMSIS-DAP** or the **Black Magic Probe** can be used as well.

You can use **OpenOCD** (http://openocd.org/) to connect to the chip and manage its flash memory:

High level SWD access using a *ST-Link V2* clone:

```
$ openocd -d2 -f interface/stlink-v2.cfg -c "transport select hla_swd" -f target/nrf52.cfg
```

Lower level SWD access using a *CMSIS-DAP*:

```
$ openocd -f interface/cmsis-dap.cfg -c "transport select swd" -f target/nrf52.cfg
```

The PlatformIO IDE is set up to use OpenOCD via some hardware adapter (default: CMSIS-DAP) to program the chip. This can be changed in the `platformio.ini` file.

#### Troubleshooting

If the watch refuses to flash, hangs in low power mode or is stuck in a bootloop, try connecting to it using OpenOCD while spamming the reset button on your adapter (or short `SWDCLK` to `VCC` - This should trigger a debug init halt). This should not be needed during normal flashing and execution. Sometimes, this condition occurs randomly when the `SWDCLK` pin is left floating. On a successful connection OpenOCD displays something like "`Info : nrf52.cpu: hardware has 6 breakpoints, 4 watchpoints`". This means the chip was reset and is now in debug mode. The script `tools/reset.sh` or the task "Reset Target" automates this spamming.

Optionally, append `-c "reset halt"` to the OpenOCD command. The chip then halts at the first instruction, which may be good for debugging.

## Connecting to a phone

A fork of the Android app **Gadgetbridge** (https://gadgetbridge.org/) with support for this firmware is available at *Codeberg*: https://codeberg.org/StarGate01/Gadgetbridge/src/branch/chrzwatch .

You can use the Android app "**nRF Connect**" (https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) to test the Bluetooth functions. Sometimes, connecting takes multiple tries.

## Stock firmware

A dump of the stock ROM can be found at `doc/stock_rom_dump.bin` (armv7le) for decompiling (e.g. using **Ghidra**: https://ghidra-sre.org/) or restoring the watch (untested).

## Thanks to

Thanks to *Aaron Christophel* for providing instructions on how to modify the hardware, mapping out the pins and providing some demo Arduino code.

 - https://github.com/atc1441
 - http://atcnetz.blogspot.com/2019/02/arduino-auf-dem-fitness-tracker-dank.html
 - https://www.mikrocontroller.net/topic/467136
 - https://www.youtube.com/watch?v=0Fu-VSuKHEg 

### Library credits and modifications

Hot-patches can be found in this repository under `/patch`.

- ARM Mbed RTOS and API: https://os.mbed.com/
   - Hot-patch Nordic BLE driver to support deep sleep
   - Hot-patch NRF52 linker memory map to support crash dump retention

All modified libraries have been or will be published to https://platformio.org , their source code can be found in this repository under `/lib`.

 - The `CurrentTimeService` module of the `BLE_GATT_Services` library (https://platformio.org/lib/show/7372/BLE_GATT_Services) is based on `BLE_CurrentTimeService` by *Takehisa Oneta*: https://os.mbed.com/users/ohneta/code/BLE_CurrentTimeService/
   - Deferred calls in ISR context to EventQueue
   - Added documentation
   - Added LOW_POWER macro (default 1) to use a low-power ticker.
 - The `Adafruit_ST7735_Mini` library (https://platformio.org/lib/show/7412/Adafruit_ST7735_Mini) is based on the `Adafruit_ST7735` library by *Andrew Lindsay*: https://platformio.org/lib/show/2150/Adafruit_ST7735, which in turn is a port of a library by Adafruit: https://github.com/adafruit/Adafruit-ST7735-Library
   - Added support for the `R_MINI160x80` display type
   - Added documentation
   - Added an explicit dependency to `Adafruit_GFX` port by *Andrew Lindsay*: https://platformio.org/lib/show/2147/Adafruit_GFX, which is a port of https://github.com/adafruit/Adafruit-GFX-Library
 - The `Heartrate3_AFE4404` library (https://platformio.org/lib/show/11099/Heartrate3_AFE4404) is based on the library and example code of `Click_Heartrate3_AFE4404` by *MikroElektronika* / *Corey Lakey*: https://github.com/MikroElektronika/Click_Heartrate3_AFE4404
   - Added Mbed integration
   - Added interrupt handling
   - Added powerdown/up functionality
 - The `kionix-kx123-driver` library (TBA) is based on the library of the same name by *Rohm*: https://platformio.org/lib/show/3975/kionix-kx123-driver
   - Fixed include paths
 - The `RegisterWriter` library (TBA) is based on the library of the same name by *Rohm* / *Mikko Koivunen*: https://platformio.org/lib/show/10695/RegisterWriter
   - Fixed include paths
   - Fixed default pins