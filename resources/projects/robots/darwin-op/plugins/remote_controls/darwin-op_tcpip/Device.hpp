/*
 * File:         Device.hpp
 * Date:         June 2012
 * Description:  Abstraction of a Webots device
 * Author:       fabien.rohrer@cyberbotics.com
 * Modifications:
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <webots/types.h>
#include <webots/nodes.h>

class Device {
public:
  // Device Manager is responsible to create/destroy devices
  Device(WbDeviceTag tag, int index);
  virtual ~Device() {}

  WbDeviceTag tag() const { return mTag; }

  int index() const { return mIndex; }

private:
  WbDeviceTag mTag;
  WbNodeType mType;
  const char *mName;

  int mIndex;
};

#endif
