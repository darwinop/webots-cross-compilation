/*******************************************************************************************************/
/* File:         Servo.hpp                                                                             */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Servo Webots API for the DARwIn-OP real robot                          */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef SERVO_HPP
#define SERVO_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>
#include <map>

namespace webots {
  class Servo: public Device  {
    public:
      enum { WB_SERVO_ROTATIONAL = 0 };
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
      double getTargetPosition();
      double getMinPosition();
      double getMaxPosition();
      int           getSamplingPeriod();
      int           getType() const;
      virtual void  setPosition(double position);
      
              void  updateSpeed(int ms);
              bool alarm();

    private:
      static void   initStaticMap();
      // For acceleration module //
      double        mAcceleration;
      double        mActualVelocity;
      double        mMaxVelocity;
      // For SynchWrite //
      int           mGoalPosition;
      int           mTorqueEnable;
      int           mPGain;
      int           mMovingSpeed;
      int           mTorqueLimit;
      // For Bulk Read //
      int           mPresentPosition;
      int           mPresentSpeed;
      int           mPresentLoad;

      int getGoalPosition();
      int getTorqueEnable();
      int getPGain();
      int getMovingSpeed();
      int getTorqueLimit();
      void setPresentPosition(int position);
      void setPresentSpeed(int speed);
      void setPresentLoad(int load);
      
      friend int Robot::step(int ms);
      friend     Robot::Robot();

      static std::map<const std::string, int> mNamesToIDs;
      static std::map<const std::string, int> mNamesToLimUp;
      static std::map<const std::string, int> mNamesToLimDown;
      static std::map<const std::string, int> mNamesToInitPos;
      double getSpeed() const;
  };
}

#endif //SERVO_HPP
