from os.path import join, isfile

Import("env")

MBED_DIR = env.PioPlatform().get_package_dir("framework-mbed")

patches = [
    (join(MBED_DIR, "features", "FEATURE_BLE", "targets", "TARGET_NORDIC", "TARGET_NORDIC_CORDIO", "TARGET_NRF5x", "NRFCordioHCIDriver.cpp"),
        join("patch", "mbed", "ble_sleep.patch")),
    (join(MBED_DIR, "targets", "TARGET_NORDIC", "TARGET_NRF5x", "TARGET_NRF52", "TARGET_MCU_NRF52832", "device", "TOOLCHAIN_GCC_ARM", "NRF52832.ld"),
        join("patch", "mbed", "linker_crashdump.patch"))
]

for patch in patches:
    assert isfile(patch[0]) and isfile(patch[1])
    env.Execute("patch %s %s" % patch)