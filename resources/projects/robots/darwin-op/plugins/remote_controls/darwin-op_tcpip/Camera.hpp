/*
 * File:         Camera.hpp
 * Date:         January 2013
 * Description:  Abstraction of a camera for the DARwIn-OP
 * Author:       david.mansolino@epfl.ch
 * Modifications:
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Sensor.hpp"

#include <string>

class CameraR : public Sensor {
  public:

    // Device Manager is responsible to create/destroy devices
             CameraR(WbDeviceTag tag);
    virtual ~CameraR() {}

    int      width() const { return mWidth; }
    int      height() const { return mHeight; }

  private:
    int      mWidth;
    int      mHeight;
};

#endif
