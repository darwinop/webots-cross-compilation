#include "DeviceManager.hpp"

#include "Device.hpp"
#include "Camera.hpp"
#include "Led.hpp"
#include "Servo.hpp"
#include "SingleValueSensor.hpp"
#include "TripleValuesSensor.hpp"

#include <webots/robot.h>

#define NSERVOS 20

static const char *servoNames[NSERVOS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

using namespace std;

DeviceManager *DeviceManager::cInstance = NULL;

DeviceManager *DeviceManager::instance() {
  if (!cInstance)
    cInstance = new DeviceManager();
  return cInstance;
}

void DeviceManager::cleanup() {
  if (cInstance) {
    delete cInstance;
    cInstance = NULL;
  }
}

DeviceManager::DeviceManager() {
  clear();

  mAccelerometer = new TripleValuesSensor(wb_robot_get_device("Accelerometer"), 0);
  mDevices.push_back(mAccelerometer);
  mGyro = new TripleValuesSensor(wb_robot_get_device("Gyro"), 0);
  mDevices.push_back(mGyro);
  
  mCamera = new CameraR(wb_robot_get_device("Camera"));
  mDevices.push_back(mCamera);
  
  mLeds[0] = new Led(wb_robot_get_device("EyeLed"), 0);
  mLeds[1] = new Led(wb_robot_get_device("HeadLed"), 1);
  mLeds[2] = new Led(wb_robot_get_device("BackLedRed"), 2);
  mLeds[3] = new Led(wb_robot_get_device("BackLedGreen"), 3);
  mLeds[4] = new Led(wb_robot_get_device("BackLedBlue"), 4);
  for (int i=0; i<5; i++) {
    mDevices.push_back(mLeds[i]);
  }
  
  for (int i=0; i<20; i++) {
    mServos[i] = new ServoR(wb_robot_get_device(servoNames[i]), i);
    mDevices.push_back(mServos[i]);   
    
    mServosForceFeedback[i] = new SingleValueSensor(wb_robot_get_device(servoNames[i]), i);
    mDevices.push_back(mServosForceFeedback[i]);
  }
}

DeviceManager::~DeviceManager() {
  clear();
}

Device *DeviceManager::findDeviceFromTag(WbDeviceTag tag) const {
  vector<Device *>::const_iterator it;
  for (it = mDevices.begin(); it < mDevices.end(); it++) {
    Device *d = *it;
    if (d->tag() == tag)
      return d;
  }
  return NULL;
}

void DeviceManager::clear() {
  vector<Device *>::const_iterator it;
  for (it = mDevices.begin(); it < mDevices.end(); it++)
    delete *it;

  mDevices.clear();

  mCamera = NULL;
  mAccelerometer = NULL;
  mGyro = NULL;

  for (int i=0; i<5; i++)
    mLeds[i] = NULL;
    
  for (int i=0; i<20; i++) {
    mServos[i] = NULL;
    mServosForceFeedback[i] = NULL;
  }
    
}

void DeviceManager::apply(int simulationTime) {
  vector<Device *>::const_iterator it;

  // check if some sensors need to be requested
  for (it=mDevices.begin() ; it < mDevices.end(); it++) {
    Device *d = *it;
    Sensor *s = dynamic_cast<Sensor *>(d);

    if (s && s->isEnabled() && s->lastRefreshTime() + s->rate() <= simulationTime) {
      s->setLastRefreshTime(simulationTime);
      s->setSensorRequested();
    }
  }
}
