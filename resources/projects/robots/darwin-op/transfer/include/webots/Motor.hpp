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
      enum {
        ROTATIONAL = 0
      };

                    Motor(const std::string &name); //Use Robot::getMotor() instead
      virtual      ~Motor();

      virtual void  setPosition(double position);
      double        getTargetPosition() const;
      virtual void  setVelocity(double vel);
      virtual void  setAcceleration(double acceleration);
      virtual void  setAvailableTorque(double availableTorque);
      virtual void  setControlPID(double p, double i, double d);
      double        getMinPosition() const;
      double        getMaxPosition() const;

      virtual void  enablePosition(int ms);
      virtual void  disablePosition();
      int           getPositionSamplingPeriod() const;
      double        getPosition() const;

      // note: *Force* functions are not implemented because DARwIn-OP is controlled in torques

      // torque feedback API is useless on the real robot:
      // indeed, the torque feedback is available at each step no matter the sampling period
      virtual void  enableTorqueFeedback(int ms);
      virtual void  disableTorqueFeedback();
      int           getTorqueFeedbackSamplingPeriod() const;
      double        getTorqueFeedback() const;

      virtual void  setTorque(double torque);

      int           getType() const;

      // functions not implemented in the regular Webots API
      void          updateSpeed(int ms);

    private:
      static void   initStaticMap();

      static std::map<const std::string, int> mNamesToIDs;
      static std::map<const std::string, int> mNamesToLimUp;
      static std::map<const std::string, int> mNamesToLimDown;
      static std::map<const std::string, int> mNamesToInitPos;

      int           getGoalPosition() const;
      int           getTorqueEnable() const;
      int           getPGain() const;
      int           getMovingSpeed() const;
      int           getTorqueLimit() const;
      double        getSpeed() const;

      void          setPresentPosition(int position);
      void          setPresentSpeed(int speed);
      void          setPresentLoad(int load);
    
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
      int           mTorqueFeedback;

      // For Bulk Read //
      int           mPresentPosition;
      int           mPresentSpeed;
      int           mPresentLoad;

      friend int Robot::step(int ms);
      friend     Robot::Robot();
  };
}

#endif //MOTOR_HPP
