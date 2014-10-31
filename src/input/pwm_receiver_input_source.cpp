#include <input/pwm_receiver_input_source.hpp>

controller_input_t PWMReceiverInputSource::read() {
  controller_input_t input = {
    .pitch_sp = 0.0f,
    .roll_sp = 0.0f,
    .yaw_sp = 0.0f,
    .throttle_sp = 0.0f
  };

  return input;
}