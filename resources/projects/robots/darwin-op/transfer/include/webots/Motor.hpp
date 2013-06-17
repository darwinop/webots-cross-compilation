/*******************************************************************************************************/
/* File:         Motor.hpp                                                                             */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Motor Webots API for the DARwIn-OP real robot                          */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>
#include <map>

namespace webots {
  class Motor: public Device  {
    public:
      enum { ROTATIONAL = 0 };
                    Motor(const std::string &name, const Robot *robot); //Use Robot::getMotor() instead
      virtual      ~Motor();
      virtual void  setAcceleration(double acceleration);
      virtual void  setVelocity(double vel);
      virtual void  enablePosition(int ms);
      virtual void  disablePosition();
      virtual void  setTorque(double torque);
      virtual void  setAvailableTorque(double availableTorque);
      virtual void  setControlP(double p);
      virtual void  enableTorqueFeedback(int ms);
      virtual void  disableTorqueFeedback();
      double        getTorqueFeedback() const;
      double        getPosition() const;
      double        getTargetPosition();
      double        getMinPosition();
      double        getMaxPosition();
      int           getPositionSamplingPeriod();
      int           getType() const;
      virtual void  setPosition(double position);
      
              void  updateSpeed(int ms);

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

#endif //MOTOR_HPP
