/*******************************************************************************************************/
/* File:         Accelerometer.hpp                                                                     */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Accelerometer Webots API for the DARwIn-OP real robot                  */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef ACCELEROMETER_HPP
#define ACCELEROMETER_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>

namespace webots {
  class Accelerometer: public Device  {
    public:
                    Accelerometer(const std::string &name); //Use Robot::getAccelerometer() instead
      virtual      ~Accelerometer();

      virtual void  enable(int ms);
      virtual void  disable();

      const double *getValues() const;
      int           getSamplingPeriod() const;

    private:
      double        mValues[3];
      void          setValues(const int *integerValues);
    
      friend int Robot::step(int ms);
  };
}

#endif // ACCELEROMETER_HPP
