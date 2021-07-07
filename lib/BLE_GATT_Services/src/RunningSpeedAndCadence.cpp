/**
 * @file RunningSpeedAndCadence.cpp
 * @author Christoph Honal
 * @brief Implements the definitions from RunningSpeedAndCadence.h
 * @version 0.1
 * @date 2021-07-06
 */

#include "RunningSpeedAndCadence.h"


RunningSpeedAndCadenceService::RunningSpeedAndCadenceService(BLE& ble, enum RSCFeatureFlags featureFlags, enum SensorLocation sensorLocation):
    _ble(ble),
    _RSCMeasurementCharacteristic(GattCharacteristic::UUID_RSC_MEASUREMENT_CHAR,
        _rscMeasurementValueBytes, UUID_RSC_MEASUREMENT_MAX_VALUE_SIZE, UUID_RSC_MEASUREMENT_MAX_VALUE_SIZE,
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
    _RSCFeatureCharacteristic(GattCharacteristic::UUID_RSC_FEATURE_CHAR,
        _rscFeatureFlagsValueBytes, UUID_RSC_FEATURE_FLAG_VALUE_SIZE, UUID_RSC_FEATURE_FLAG_VALUE_SIZE,
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),
    _SensorLocationCharacteristic(UUID(UUID_SENSOR_LOCATION_CHAR), 
        _rscSensorLocationValueBytes, UUID_RSC_SENSOR_LOCATION_VALUE_SIZE, UUID_RSC_SENSOR_LOCATION_VALUE_SIZE,
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ)
{
    // Setup buffers
    memset(_rscMeasurementValueBytes, 0, UUID_RSC_MEASUREMENT_MAX_VALUE_SIZE);
    memcpy(_rscFeatureFlagsValueBytes, &featureFlags, UUID_RSC_FEATURE_FLAG_VALUE_SIZE);
    memcpy(_rscSensorLocationValueBytes, &sensorLocation, UUID_RSC_SENSOR_LOCATION_VALUE_SIZE);

    // Setup BLE service definition
    GattCharacteristic *charsTable[] = { &_RSCMeasurementCharacteristic, &_RSCFeatureCharacteristic, &_SensorLocationCharacteristic };
    GattService RunningSpeedAndCadenceGATT(GattService::UUID_RUNNING_SPEED_AND_CADENCE, charsTable, 
        (featureFlags & RSCFeatureFlags::MULTIPLE_SENSOR_LOCATIONS_SUPPORTED)? 3:2); // Only add sensor location if feature is enabled

    // Attach GATT server
    _ble.gattServer().addService(RunningSpeedAndCadenceGATT);

}

void RunningSpeedAndCadenceService::updateMeasurement(const struct RSCMeasurement_t& measurement)
{
    // Copy mandatory part
    memcpy(_rscMeasurementValueBytes, &measurement, UUID_RSC_MEASUREMENT_MANDATORY_VALUE_SIZE);
    uint8_t effective_size = UUID_RSC_MEASUREMENT_MANDATORY_VALUE_SIZE;
   
    // Copy optional parts
    if(measurement.flags & INSTANTANEOUS_STRIDE_LENGTH_PRESENT) // C1: instantaneous stride length
    {
        memcpy(_rscMeasurementValueBytes + effective_size, 
            &(measurement.instantaneous_stride_length), UUID_RSC_MEASUREMENT_C1_VALUE_SIZE);
        effective_size += UUID_RSC_MEASUREMENT_C1_VALUE_SIZE;
    }
    if(measurement.flags & TOTAL_DISTANCE_PRESENT) // C2: total distance
    {
        memcpy(_rscMeasurementValueBytes + effective_size, 
            &(measurement.total_distance), UUID_RSC_MEASUREMENT_C2_VALUE_SIZE);
        effective_size += UUID_RSC_MEASUREMENT_C2_VALUE_SIZE;
    }

    // Write to BLE stack
    _ble.gattServer().write(_RSCMeasurementCharacteristic.getValueHandle(), 
        _rscMeasurementValueBytes, effective_size);
}

void RunningSpeedAndCadenceService::updateSensorLocation(enum SensorLocation location)
{
    memcpy(_rscSensorLocationValueBytes, &location, UUID_RSC_SENSOR_LOCATION_VALUE_SIZE);

    // Write to BLE stack
    _ble.gattServer().write(_SensorLocationCharacteristic.getValueHandle(), 
        _rscSensorLocationValueBytes, UUID_RSC_SENSOR_LOCATION_VALUE_SIZE);
}