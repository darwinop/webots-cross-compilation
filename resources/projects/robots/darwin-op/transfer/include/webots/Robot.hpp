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

#include <minIni.h>

#include <pthread.h>

#define NSERVOS 20

namespace Robot {
  class CM730;
  class LinuxCM730;
}

class Keyboard;

namespace webots {
  class Device;
  class Accelerometer;
  class Camera;
  class Gyro;
  class LED;
  class Servo;
  class Speaker;
  
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
      Speaker             *getSpeaker(const std::string &name) const;
      virtual void         keyboardEnable(int ms);
      virtual void         keyboardDisable();
      virtual int          keyboardGetKey();
      
      ::Robot::CM730      *getCM730() const { return mCM730; }

    protected:
      static void         *KeyboardTimerProc(void *param);// thread function

    private:
      void                 initDevices();
      void                 initDarwinOP();
      void                 LoadINISettings(minIni* ini, const std::string &section);
      Device              *getDevice(const std::string &name) const;
      Keyboard            *mKeyboard;
      bool                 mKeyboardEnable;
      pthread_t            mKeyboardThread; // thread structure

      int                  mTimeStep;
      ::Robot::LinuxCM730 *mLinuxCM730;
      ::Robot::CM730      *mCM730;
      struct timeval       mStart;
      double               mPreviousStepTime;
      std::map<const std::string, Device *> mDevices;
  };
}

#endif //ROBOT_HPP
