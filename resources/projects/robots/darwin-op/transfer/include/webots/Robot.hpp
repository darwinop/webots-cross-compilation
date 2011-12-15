/*******************************************************************************************************/
/* File:         Robot.hpp                                                                             */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Robot Webots API for the DARwIn-OP real robot                          */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef ROBOT_HPP
#define ROBOT_HPP

#include <string>
#include <map>
#include <sys/time.h>

namespace Robot {
  class CM730;
  class LinuxCM730;
}

namespace webots {
  class Device;
  class Accelerometer;
  class Camera;
  class Gyro;
  class LED;
  class Servo;
  
  class Robot {
    public:
                           Robot();
      virtual             ~Robot();
      virtual int          step(int ms);
      std::string          getName() const;
      double               getTime() const;
      int                  getMode() const;
      double               getBasicTimeStep() const;
      Accelerometer       *getAccelerometer(const std::string &name) const;
      Camera              *getCamera(const std::string &name) const;
      Gyro                *getGyro(const std::string &name) const;
      LED                 *getLED(const std::string &name) const;
      Servo               *getServo(const std::string &name) const;
      
      ::Robot::CM730      *getCM730() const { return mCM730; }

    private:
      void                 initDevices();
      void                 initDarwinOP();
      Device              *getDevice(const std::string &name) const;

      ::Robot::LinuxCM730 *mLinuxCM730;
      ::Robot::CM730      *mCM730;
      struct timeval       mStart;
      double               mPreviousStepTime;
      std::map<const std::string, Device *> mDevices;
  };
}

#endif //ROBOT_HPP
