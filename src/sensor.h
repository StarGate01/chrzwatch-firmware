#ifndef SENSOR_H
#define SENSOR_H

#include <mbed.h>

class SensorService
{
    public:
        SensorService();
        void update();

        uint8_t getHRValue();
        uint8_t getBatteryValue();

    protected:
        uint8_t _hr_value;
        uint8_t _gyro_value[3];
        uint8_t _battery_value;

};

#endif
