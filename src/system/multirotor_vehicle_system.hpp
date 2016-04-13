#ifndef MULTIROTOR_SYSTEM_HPP_
#define MULTIROTOR_SYSTEM_HPP_

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
#include "controller/angular_acceleration_controller.hpp"
#include "controller/position_controller.hpp"
#include "controller/controller_pipeline.hpp"
#include "controller/setpoint_types.hpp"
#include "controller/zero_controller.hpp"
#include "input/input_source.hpp"
#include "motor/motor_mapper.hpp"
#include "motor/pwm_device_group.hpp"

// Filesystem
#include "filesystem/logger.hpp"

// Platform
#include "variant/pwm_platform.hpp"
#include "variant/platform.hpp"

// World estimation
#include "estimator/world_estimator.hpp"

// Sensors
#include "sensor/sensors.hpp"

enum class MultirotorControlMode {
  DISARMED,
  POSITION,
  VELOCITY,
  ANGULAR_POS,
  ANGULAR_VEL,
};

class MultirotorVehicleSystem : public VehicleSystem, public MessageListener {
public:
  MultirotorVehicleSystem(
      ParameterRepository& params,
      Sensors& sensors,
      WorldEstimator& estimator, InputSource& inputSource,
      MotorMapper& motorMapper, Communicator& communicator,
      Logger& logger, Platform& platform);

  void update() override;
  bool healthy() const;

  void on(const protocol::message::set_arm_state_message_t& m) override;

private:
  ParameterRepository& params;

  Sensors& sensors;
  WorldEstimator& estimator;
  InputSource& inputSource;

  PositionController posController;
  AngularPositionController attPosController;
  AngularVelocityController attVelController;
  AngularAccelerationController attAccController;
  ControllerPipeline<ActuatorSetpoint> pipeline;

  ZeroController<ActuatorSetpoint> zeroController;

  MotorMapper& motorMapper;
  Platform& platform;
  RateLimitedStream stream;
  Logger& logger;

  MultirotorControlMode mode;

  bool calibrated;
  void calibrate(SensorMeasurements meas);

  float altSp;
  float yawPosSp;

  void DisarmedMode(SensorMeasurements meas, WorldEstimate est, ControllerInput input, ActuatorSetpoint& sp);
  void AngularRateMode(SensorMeasurements meas, WorldEstimate est, ControllerInput input, ActuatorSetpoint& sp);
  void AngularPosMode(SensorMeasurements meas, WorldEstimate est, ControllerInput input, ActuatorSetpoint& sp);
  void PosMode(SensorMeasurements meas, WorldEstimate est, ControllerInput input, ActuatorSetpoint& sp);

  /**
   * RGB LED stuff.
   */
  void SetLED(float r, float g, float b);
  void BlinkLED(float r, float g, float b, float freq);
  void PulseLED(float r, float g, float b, float freq);
  void RGBLED(float freq);
};

#endif
