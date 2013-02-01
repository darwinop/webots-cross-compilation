/*
 * File:         Servos.hpp
 * Date:         January 2013
 * Description:  Abstraction of a servo
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef SERVO_HPP
#define SERVO_HPP

#include "SingleValueSensor.hpp"
#include "Device.hpp"

class ServoR : public SingleValueSensor {
public:
  // Device Manager is responsible to create/destroy devices
  ServoR(WbDeviceTag tag, int index) :
    SingleValueSensor(tag, index),
    mServoRequested(false),
    mPositionRequested(false),
    mVelocityRequested(false),
    mAccelerationRequested(false),
    mMotorForceRequested(false),
    mControlPRequested(false),
    mForceRequested(false)
  {}
  virtual ~ServoR() {}
  
  bool isServoRequested() const { return mServoRequested; }
  void resetServoRequested() { mServoRequested = false; }
  void setServoRequested() { mServoRequested = true; }

  // Actuators part
  void setPosition(double position) { mPosition = position; }
  void setVelocity(double vel) { mVelocity = vel; }
  void setAcceleration(double force) { mAcceleration = force; }
  void setMotorForce(double motor_force) { mMotorForce = motor_force; }
  void setControlP(double p) { mControlP = p; }
  void setForce(double force) { mForce = force; }
  
  double position() { return mPosition; }
  double velocity() { return mVelocity; }
  double acceleration() { return mAcceleration; }
  double motorForce() { return mMotorForce; }
  double controlP() { return mControlP; }
  double force() { return mForce; }
  
  bool isPositionRequested() const { return mPositionRequested; }
  void resetPositionRequested() { mPositionRequested = false; }
  void setPositionRequested() { mPositionRequested = true; }
  
  bool isVelocityRequested() const { return mVelocityRequested; }
  void resetVelocityRequested() { mVelocityRequested = false; }
  void setVelocityRequested() { mVelocityRequested = true; }
  
  bool isAccelerationRequested() const { return mAccelerationRequested; }
  void resetAccelerationRequested() { mAccelerationRequested = false; }
  void setAccelerationRequested() { mAccelerationRequested = true; }
  
  bool isMotorForceRequested() const { return mMotorForceRequested; }
  void resetMotorForceRequested() { mMotorForceRequested = false; }
  void setMotorForceRequested() { mMotorForceRequested = true; }
  
  bool isControlPRequested() const { return mControlPRequested; }
  void resetControlPRequested() { mControlPRequested = false; }
  void setControlPRequested() { mControlPRequested = true; }
  
  bool isForceRequested() const { return mForceRequested; }
  void resetForceRequested() { mForceRequested = false; }
  void setForceRequested() { mForceRequested = true; }

private:

  bool mServoRequested;

  // Actuators part
  double mPosition;
  double mVelocity;
  double mAcceleration;
  double mMotorForce;
  double mControlP;
  double mForce;
  
  bool mPositionRequested;
  bool mVelocityRequested;
  bool mAccelerationRequested;
  bool mMotorForceRequested;
  bool mControlPRequested;
  bool mForceRequested;
};

#endif
