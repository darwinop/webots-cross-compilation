/*
 * File:         Motors.hpp
 * Date:         January 2013
 * Description:  Abstraction of a motor
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef MOTOR_HPP
#define MOTOR_HPP

#include "SingleValueSensor.hpp"
#include "Device.hpp"

class MotorR : public SingleValueSensor {
  public:
    // Device Manager is responsible to create/destroy devices
             MotorR(WbDeviceTag tag, int index) :
               SingleValueSensor(tag, index),
               mMotorRequested(false),
               mPositionRequested(false),
               mVelocityRequested(false),
               mAccelerationRequested(false),
               mMotorAvailableTorqueRequested(false),
               mControlPRequested(false),
               mTorqueRequested(false) {}
    virtual ~MotorR() {}
    
    bool     isMotorRequested() const { return mMotorRequested; }
    void     resetMotorRequested() { mMotorRequested = false; }
    void     setMotorRequested() { mMotorRequested = true; }

    // Actuators part
    void     setPosition(double position) { mPosition = position; }
    void     setVelocity(double vel) { mVelocity = vel; }
    void     setAcceleration(double acceleration) { mAcceleration = acceleration; }
    void     setAvailableTorque(double torque) { mMotorAvailableTorque = torque; }
    void     setControlPID(double p, double i, double d) { mControlP = p; mControlI = i; mControlD = d; }
    void     setControlP(double p) { mControlP = p; } // legacy
    void     setTorque(double torque) { mTorque = torque; }
    
    double   position() { return mPosition; }
    double   velocity() { return mVelocity; }
    double   acceleration() { return mAcceleration; }
    double   motorForce() { return mMotorAvailableTorque; }
    double   controlP() { return mControlP; }
    double   controlI() { return mControlI; }
    double   controlD() { return mControlD; }
    double   torque() { return mTorque; }
    
    bool     isPositionRequested() const { return mPositionRequested; }
    void     resetPositionRequested() { mPositionRequested = false; }
    void     setPositionRequested() { mPositionRequested = true; }
    
    bool     isVelocityRequested() const { return mVelocityRequested; }
    void     resetVelocityRequested() { mVelocityRequested = false; }
    void     setVelocityRequested() { mVelocityRequested = true; }
    
    bool     isAccelerationRequested() const { return mAccelerationRequested; }
    void     resetAccelerationRequested() { mAccelerationRequested = false; }
    void     setAccelerationRequested() { mAccelerationRequested = true; }
    
    bool     isMotorForceRequested() const { return mMotorAvailableTorqueRequested; }
    void     resetAvailableTorqueRequested() { mMotorAvailableTorqueRequested = false; }
    void     setAvailableTorqueRequested() { mMotorAvailableTorqueRequested = true; }
    
    bool     isControlPIDRequested() const { return mControlPIDRequested; }
    void     resetControlPIDRequested() { mControlPIDRequested = false; }
    void     setControlPIDRequested() { mControlPIDRequested = true; }
    
    // P-control legacy code
    bool     isControlPRequested() const { return mControlPRequested; }
    void     resetControlPRequested() { mControlPRequested = false; }
    void     setControlPRequested() { mControlPRequested = true; }
    
    bool     isForceRequested() const { return mTorqueRequested; }
    void     resetTorqueRequested() { mTorqueRequested = false; }
    void     setTorqueRequested() { mTorqueRequested = true; }

  private:

    bool     mMotorRequested;

    // Actuators part
    double   mPosition;
    double   mVelocity;
    double   mAcceleration;
    double   mMotorAvailableTorque;
    double   mControlP, mControlD, mControlI;
    double   mTorque;
    
    bool     mPositionRequested;
    bool     mVelocityRequested;
    bool     mAccelerationRequested;
    bool     mMotorAvailableTorqueRequested;
    bool     mControlPRequested; // legacy
    bool     mControlPIDRequested;
    bool     mTorqueRequested;
};

#endif
