#include "entry_points.hpp"
#include "Wrapper.hpp"

bool wbr_init(WbrInterface *ri) {
  Wrapper::init();

  ri->mandatory.wbr_start = Wrapper::start;
  ri->mandatory.wbr_stop = Wrapper::stop;
  ri->mandatory.wbr_has_failed = Wrapper::hasFailed;
  ri->mandatory.wbr_robot_step = Wrapper::robotStep;
  ri->mandatory.wbr_stop_actuators = Wrapper::stopActuators;

  ri->wbr_set_refresh_rate = Wrapper::setRefreshRate;
  ri->wbr_servo_set_motor_force_refresh_rate = Wrapper::setMotorForceRefreshRate;
  
  ri->wbr_led_set = Wrapper::ledSet;
  
  ri->wbr_camera_set_fov = Wrapper::cameraSetFOV;
  
  ri->wbr_servo_set_position = Wrapper::servoSetPosition;
  ri->wbr_servo_set_velocity = Wrapper::servoSetVelocity;
  ri->wbr_servo_set_acceleration = Wrapper::servoSetAcceleration;
  ri->wbr_servo_set_motor_force = Wrapper::servoSetMotorForce;
  ri->wbr_servo_set_force = Wrapper::servoSetForce;
  ri->wbr_servo_set_control_p = Wrapper::servoSetControlP;

  return true;
}

void wbr_cleanup() {
  Wrapper::cleanup();
}
