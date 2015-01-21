#ifndef QUAD_PLUS_MULTIROTOR_MOTOR_MAPPER_HPP_
#define QUAD_PLUS_MULTIROTOR_MOTOR_MAPPER_HPP_

#include "communication/communicator.hpp"
#include "communication/rate_limited_stream.hpp"
#include "controller/setpoint_types.hpp"
#include "motor/pwm_motor_mapper.hpp"
#include "variant/pwm_platform.hpp"

class MultirotorQuadPlusMotorMapper : public PWMMotorMapper<4> {
public:
  MultirotorQuadPlusMotorMapper(PWMPlatform& pwmPlatform, Communicator& communicator);

  void init() override;
  void run(bool armed, actuator_setpoint_t& input) override;

private:
  RateLimitedStream throttleStream;
};

#endif
