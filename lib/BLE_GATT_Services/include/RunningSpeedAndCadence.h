/**
 * @file RunningSpeedAndCadence.h
 * @author Christoph Honal
 * @brief Defines the Running speed and cadence BLE service and helpers
 * @version 0.1
 * @date 2021-07-06
 */

#ifndef RUNNING_SPEED_AND_CADENCE_SERVICE_H
#define RUNNING_SPEED_AND_CADENCE_SERVICE_H

#define BLE_FEATURE_GATT_SERVER 1
#define BLE_ROLE_BROADCASTER 1

#include <mbed.h>
#include "ble/BLE.h"

#define UUID_SENSOR_LOCATION_CHAR                 0x2A5D
#define UUID_RSC_MEASUREMENT_MANDATORY_VALUE_SIZE 4
#define UUID_RSC_MEASUREMENT_C1_VALUE_SIZE        2
#define UUID_RSC_MEASUREMENT_C2_VALUE_SIZE        4
#define UUID_RSC_MEASUREMENT_MAX_VALUE_SIZE       10
#define UUID_RSC_FEATURE_FLAG_VALUE_SIZE          2
#define UUID_RSC_SENSOR_LOCATION_VALUE_SIZE       1

/**
 * @brief Provides a BLE service to broadcast running speed and cadence
 * 
 */
class RunningSpeedAndCadenceService 
{
    
    public:
        /**
         * @brief RSC measurement flag definitions
         * 
         */
        enum RSCMeasurementFlags : uint8_t {
            INSTANTANEOUS_STRIDE_LENGTH_PRESENT = 1, //!< Instantaneous Stride Length Present, requires C1
            TOTAL_DISTANCE_PRESENT              = 2, //!< Total Distance Present, requires C2
            RUNNING_NOT_WALKING                 = 4, //!< Set = Running, Unset = Walking
        };

        /**
         * @brief Data layout of the RSC measurement characteristic
         * 
         */
        struct __attribute__ ((packed)) RSCMeasurement_t
        {
            enum RSCMeasurementFlags flags; //!< Feature support flags
            uint16_t instantaneous_speed; //!< Unit is in m/s with a resolution of 1/256 s
            uint8_t instantaneous_cadence; //!< Unit is in 1/minute (or RPM) with a resolutions of 1 1/min (or 1 RPM) 
            uint16_t instantaneous_stride_length; //!< C1. Unit is in meter with a resolution of 1/100 m (or centimeter).
            uint32_t total_distance; //!< C2. Unit is in meter with a resolution of 1/10 m (or decimeter).
            uint32_t total_steps; //!< Unused data, for custom use. Amount of steps total.
        };

        /**
         * @brief RSC general feature flag definitions
         * 
         */
        enum RSCFeatureFlags : uint16_t {
            INSTANTANEOUS_STRIDE_LENGTH_MEASUREMENT_SUPPORTED = 1,  //!< Instantaneous Stride Length Measurement Supported
            TOTAL_DISTANCE_MEASUREMENT_SUPPORTED              = 2,  //!< Total Distance Measurement Supported
            WALKING_OR_RUNNING_STATUS_SUPPORTED               = 4,  //!< Walking or Running Status Supported
            CALIBRATION_PROCEDURE_SUPPORTED                   = 8,  //!< Calibration Procedure Supported
            MULTIPLE_SENSOR_LOCATIONS_SUPPORTED               = 16, //!< Multiple Sensor Locations Supported, requires C1
        };

        /**
         * @brief Sensor location definitions
         * 
         */
        enum SensorLocation : uint8_t {
            OTHER        = 0 ,  //!< Other
            TOP_OF_SHOE  = 1 ,  //!< Top of shoe
            IN_SHOE      = 2 ,  //!< In shoe
            HIP          = 3 ,  //!< Hip
            FRONT_WHEEL  = 4 ,  //!< Front Wheel
            LEFT_CRANK   = 5 ,  //!< Left Crank
            RIGHT_CRANK  = 6 ,  //!< Right Crank
            LEFT_PEDAL   = 7 ,  //!< Left Pedal
            RIGHT_PEDAL  = 8 ,  //!< Right Pedal
            FRONT_HUB    = 9 ,  //!< Front Hub
            REAR_DROPOUT = 10,  //!< Rear Dropout
            CHAINSTAY    = 11,  //!< Chainstay
            REAR_WHEEL   = 12,  //!< Rear Wheel
            REAR_HUB     = 13,  //!< Rear Hub
        };

        /**
         * @brief Construct a new Running Speed And Cadence Service object
         * 
         * @param ble BLE instance
         */
        RunningSpeedAndCadenceService(BLE& ble, 
            enum RSCFeatureFlags featureFlags = RSCFeatureFlags::MULTIPLE_SENSOR_LOCATIONS_SUPPORTED, 
            enum SensorLocation sensorLocation = SensorLocation::OTHER);

        /**
         * @brief Publishes a new measurement
         * 
         * @param measurement The data to be published with flags contained
         */
        void updateMeasurement(const struct RSCMeasurement_t& measurement);

        /**
         * @brief Updates the sensor location
         * 
         * @param location The new location
         */
        void updateSensorLocation(enum SensorLocation location);

    protected:
        BLE& _ble; //!< Reference to the BLE instance
        uint8_t _rscMeasurementValueBytes[UUID_RSC_MEASUREMENT_MAX_VALUE_SIZE]; //!< Buffer for RSC measurement
        uint8_t _rscFeatureFlagsValueBytes[UUID_RSC_FEATURE_FLAG_VALUE_SIZE]; //!< Buffer for feature flag
        uint8_t _rscSensorLocationValueBytes[UUID_RSC_SENSOR_LOCATION_VALUE_SIZE]; //!< Buffer for sensor location
        GattCharacteristic _RSCMeasurementCharacteristic; //!< RSC Measurement characteristic definition
        GattCharacteristic _RSCFeatureCharacteristic; //!< RSC Feature characteristic definition
        GattCharacteristic _SensorLocationCharacteristic; //!< C1. Sensor location characteristic definition
        // Not implemented // GattCharacteristic _SCControlPointCharacteristic //!< C2. Control point characteristic definition

};
 
#endif
 