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
      virtual void  setPosition(double position);

    private:
      static void   initStaticMap();
      static std::map<const std::string, int> mNamesToIDs;
  };
}

#endif //SERVO_HPP
