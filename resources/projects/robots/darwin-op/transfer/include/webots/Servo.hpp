/*******************************************************************************************************/
/* File:         Servo.hpp                                                                             */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Servo Webots API for the DARwIn-OP real robot                          */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef SERVO_HPP
#define SERVO_HPP

#include <webots/Device.hpp>
#include <map>

namespace webots {
  class Servo: public Device  {
    public:
                    Servo(const std::string &name, const Robot *robot); //Use Robot::getServo() instead
      virtual      ~Servo();
      virtual void  setAcceleration(double force);
      virtual void  setVelocity(double vel);
      virtual void  enablePosition(int ms);
      virtual void  disablePosition();
      virtual void  setForce(double force);
      virtual void  setMotorForce(double motor_force);
      virtual void  setControlP(double p);
      virtual void  enableMotorForceFeedback(int ms);
      virtual void  disableMotorForceFeedback();
      double getMotorForceFeedback() const;
      double getPosition() const;
      virtual void  setPosition(double position);
      
              void  updateSpeed(int ms);

    private:
      static void   initStaticMap();
      // For acceleration module //
      double        mAcceleration;
      double        mActualVelocity;
      double        mMaxVelocity;
      // For SynchWrite //
      int mGoalPosition;
      int mTorqueEnable;
      int mPGain;
      int mMovingSpeed;
      int mTorqueLimit;
      
      static std::map<const std::string, int> mNamesToIDs;
      static std::map<const std::string, int> mNamesToLimUp;
      static std::map<const std::string, int> mNamesToLimDown;
      static std::map<const std::string, int> mNamesToInitPos;
      double getSpeed() const;
      
    friend int Robot::step(int ms);
  };
}

#endif //SERVO_HPP
