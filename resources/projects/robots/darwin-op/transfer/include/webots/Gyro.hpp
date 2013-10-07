/*******************************************************************************************************/
/* File:         Gyro.hpp                                                                              */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Gyro Webots API for the DARwIn-OP real robot                           */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef GYRO_HPP
#define GYRO_HPP

#include <webots/Robot.hpp>
#include <webots/Device.hpp>

namespace webots {
  class Gyro: public Device  {
    public:
                    Gyro(const std::string &name); //Use Robot::getGyro() instead
      virtual      ~Gyro();

      virtual void  enable(int ms);
      virtual void  disable();
      const double *getValues() const;
      int           getSamplingPeriod() const;

    private:
      void          setValues(const int *integerValues);

      double        mValues[3];
    
      friend int Robot::step(int ms);
  };
}

#endif // GYRO_HPP
