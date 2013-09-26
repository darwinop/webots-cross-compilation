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
  class Motor;
  class Speaker;
  
  class Robot {
    public:
    
      enum {
        KEYBOARD_END=312,
        KEYBOARD_HOME,
        KEYBOARD_LEFT,
        KEYBOARD_UP,
        KEYBOARD_RIGHT,
        KEYBOARD_DOWN,
        KEYBOARD_PAGEUP=366,
        KEYBOARD_PAGEDOWN,
        KEYBOARD_NUMPAD_HOME=375,
        KEYBOARD_NUMPAD_LEFT,
        KEYBOARD_NUMPAD_UP,
        KEYBOARD_NUMPAD_RIGHT,
        KEYBOARD_NUMPAD_DOWN,
        KEYBOARD_NUMPAD_END=382,
        KEYBOARD_KEY=0x0000ffff,
        KEYBOARD_SHIFT=0x00010000,
        KEYBOARD_CONTROL=0x00020000,
        KEYBOARD_ALT=0x00040000
      };
      
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
      Motor               *getMotor(const std::string &name) const;
      Speaker             *getSpeaker(const std::string &name) const;
      virtual void         keyboardEnable(int ms);
      virtual void         keyboardDisable();
      int                  keyboardGetKey() const;

      // not member(s) of the Webots API function: please don't use
      ::Robot::CM730      *getCM730() const { return mCM730; }
      static Robot        *getInstance() { return cInstance; }

    protected:
      static void         *KeyboardTimerProc(void *param);// thread function

    private:
      void                 initDevices();
      void                 initDarwinOP();
      void                 LoadINISettings(minIni *ini, const std::string &section);
      Device              *getDevice(const std::string &name) const;

      static Robot        *cInstance;

      std::map<const std::string, Device *> mDevices;

      Keyboard            *mKeyboard;
      bool                 mKeyboardEnable;
      pthread_t            mKeyboardThread; // thread structure
      int                  mTimeStep;
      ::Robot::LinuxCM730 *mLinuxCM730;
      ::Robot::CM730      *mCM730;
      struct timeval       mStart;
      double               mPreviousStepTime;
  };
}

#endif //ROBOT_HPP
