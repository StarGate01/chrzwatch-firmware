from os.path import join, isfile
from shutil import copyfile

Import("env")

MBED_DIR = env.PioPlatform().get_package_dir("framework-mbed")

patches = [
    (join(MBED_DIR, "features", "FEATURE_BLE", "targets", "TARGET_NORDIC", "TARGET_NORDIC_CORDIO", "TARGET_NRF5x", "NRFCordioHCIDriver.cpp"),
        join("patch", "mbed", "ble_sleep.patch")),
]

replaces = [
    (join(MBED_DIR, "targets", "TARGET_NORDIC", "TARGET_NRF5x", "TARGET_NRF52", "TARGET_MCU_NRF52832", "device", "TOOLCHAIN_GCC_ARM", "NRF52832.ld"),
        join("patch", "mbed", "linker.ld"))
]

for patch in patches:
    print("Patching " + patch[0] + " using " + patch[1])
    assert isfile(patch[0]) and isfile(patch[1])
    env.Execute("patch %s %s" % patch)

for rep in replaces:
    print("Replacing " + rep[0] + " with " + rep[1])
    assert isfile(rep[1])
    copyfile(rep[1], rep[0])