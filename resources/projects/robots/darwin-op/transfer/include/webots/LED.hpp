/*******************************************************************************************************/
/* File:         LED.hpp                                                                               */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the LED Webots API for the DARwIn-OP real robot                            */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef LED_HPP
#define LED_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>

#include <map>

namespace webots {
  class LED: public Device  {
    public:
                   LED(const std::string &name, const Robot *robot); //Use Robot::getLED() instead
      virtual     ~LED();
      virtual void set(int value);

    private:
      static void   initStaticMap();
      static std::map<const std::string, int> mNamesToIDs;
      void setColor(int color);
      static void setBackPanel(int state);
      
      int mColor;
      static int mBackPanel;
      
    friend int Robot::step(int ms);
  };
}

#endif // LED_HPP
