#ifndef ROCKET_SYSTEM_HPP_
#define ROCKET_SYSTEM_HPP_

#include "global_parameters.hpp"
#include "params/parameter_repository.hpp"
#include "system/vehicle_system.hpp"
#include "util/optional.hpp"

// Communication
#include "communication/communicator.hpp"
#include "communication/message_listener.hpp"

// Control
#include "controller/angular_position_controller.hpp"
#include "controller/angular_velocity_controller.hpp"
#include "controller/rocket_angular_acceleration_controller.hpp"
#include "controller/controller_pipeline.hpp"
#include "controller/setpoint_types.hpp"
#include "controller/zero_controller.hpp"
#include "input/input_source.hpp"
#include "motor/motor_mapper.hpp"
#include "motor/pwm_device_group.hpp"

// Filesystem
#include "filesystem/logger.hpp"

// Platform
#include "variant/digital_platform.hpp"
#include "variant/pwm_platform.hpp"
#include "variant/platform.hpp"

// World estimation
#include "estimator/world_estimator.hpp"

// Sensors
#include "sensor/sensor_measurements.hpp"

enum class CanardState {
  DISARMED,
  PRE_ARM,
  ARMED,
  FLIGHT,
  APOGEE,
  UNUSED_MICROGRAVITY,
  DESCENT,
  RECOVERY
};

class CanardSystem : public VehicleSystem, public MessageListener {
public:
  CanardSystem(
      ParameterRepository& params,
      Accelerometer& accel,
      optional<Accelerometer *> accelH,
      optional<Barometer *> bar,
      optional<Geiger *> ggr,
      optional<GPS *> gps,
      Gyroscope& gyr,
      optional<Magnetometer *> mag,
      WorldEstimator& estimator, InputSource& inputSource,
      MotorMapper& motorMapper, Communicator& communicator, Logger& logger,
      Platform& platform);

  void update() override;
  bool healthy();

  void on(const protocol::message::set_arm_state_message_t& m) override;

private:
  ParameterRepository& params;
  Accelerometer& accel;
  optional<Accelerometer *> accelH;
  optional<Barometer *> bar;
  optional<Geiger *> ggr;
  optional<GPS *> gps;
  Gyroscope& gyr;
  optional<Magnetometer *> mag;

  WorldEstimator& estimator;
  InputSource& inputSource;

  AngularPositionController attPosController;
  AngularVelocityController attVelController;
  RocketAngularAccelerationController attAccController;
  ControllerPipeline<ActuatorSetpoint> pipeline;

  ZeroController<ActuatorSetpoint> zeroController;

  MotorMapper& motorMapper;
  Platform& platform;

  RateLimitedStream systemStream;
  Logger& logger;

  void updateStreams(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Pin config
   */
  // Digital
  const uint8_t PIN_BUZZER = 0;   // PA5

  /**
   * For now, we proceed directly to PRE_ARM.
   */
  CanardState DisarmedState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Full data stream to ground station begins here.
   * Sensor calibration should be performed, hopefully with physical access to
   * the board.
   *
   * Proceed to ARMED on meeting all of the following conditions:
   *
   *   1. Sensor health checks passed
   *   2. GPS lock
   *   3. Software arm signal received from GS
   */
  CanardState PreArmState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Sensor calibration should be finished.
   *
   * Proceed to FLIGHT if X accel exceeds 1.2g.
   */
  CanardState ArmedState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Begin onboard data logging.
   *
   * Detect apogee based mainly on four measurements:
   *
   *   1. Pressure rate of change
   *   2. Net acceleration magnitude
   *   3. Non-roll rotation rate magnitude
   *   4. Orientation
   *
   * We first determine possible apogee based on the altitude rate of change,
   * which is estimated from the barometer and GPS. (Currently, only the
   * barometer is used for altitude.)
   *
   * We check for the motor cutoff acceleration drop to negative. If we sense
   * we are falling faster than -40 m/s, we transition to APOGEE. Otherwise, we
   * check for the ideal case of zero altitude change and that we are not
   * observing a false apogee during a subsonic transition.
   *
   * We do not track powered and coasting portions of flight in separate states
   * because a mach transition may happen at an unknowable time, and it's
   * probably easier to track state variables this way.
   */
  CanardState FlightState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Deploy drogue chute. If magnitude of net proper acceleration does not
   * change within 5 seconds, deploy main.
   */
  CanardState ApogeeState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Deploy main chute at 1500' AGL.
   */
  CanardState DescentState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * Turn off all telemetry (maybe?) except GPS.
   *
   * Try to conserve power.
   */
  CanardState RecoveryState(SensorMeasurements meas, WorldEstimate est, ActuatorSetpoint& sp);

  /**
   * RGB LED stuff.
   */
  void SetLED(float r, float g, float b);
  void BlinkLED(float r, float g, float b, float freq);
  void PulseLED(float r, float g, float b, float freq);
  void RGBLED(float freq);

  /**
   * In-class global vars
   */
  CanardState state;
  float motorDC;

  /**
   * Per-launch calibration
   */
  float groundAltitude;
  float velocity;
};

#endif
