#ifndef SENSOR_MEASUREMENTS_HPP_
#define SENSOR_MEASUREMENTS_HPP_

#include "sensor/accelerometer.hpp"
#include "sensor/gps.hpp"
#include "sensor/gyroscope.hpp"
#include "sensor/magnetometer.hpp"
#include "sensor/thermistor.hpp"

#include "util/optional.hpp"

struct SensorMeasurements {
  optional<AccelerometerReading> accel;
  optional<GPSReading> gps;
  optional<GyroscopeReading> gyro;
  optional<MagnetometerReading> mag;
  optional<ThermistorReading> therm;
};

#endif
