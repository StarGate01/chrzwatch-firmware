from os.path import join, isfile

Import("env")

MBED_DIR = env.PioPlatform().get_package_dir("framework-mbed")
original_file = join(MBED_DIR, "features", "FEATURE_BLE", "targets", "TARGET_NORDIC", "TARGET_NORDIC_CORDIO", "TARGET_NRF5x", "NRFCordioHCIDriver.cpp")
patched_file = join("patch", "mbed", "ble_sleep.patch")
assert isfile(original_file) and isfile(patched_file)
env.Execute("patch %s %s" % (original_file, patched_file))