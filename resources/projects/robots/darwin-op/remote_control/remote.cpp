#include "remote.hpp"
#include <webots/Accelerometer.hpp>
#include <webots/Gyro.hpp>
#include <webots/Camera.hpp>
#include <webots/LED.hpp>
#include <webots/Servo.hpp>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace webots;
using namespace std;

static const char *servoNames[NSERVOS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

Remote::Remote():
    Robot()
{
  mTimeStep = getBasicTimeStep();

  mAccelerometer = getAccelerometer("Accelerometer");
  mGyro = getGyro("Gyro");
  mCamera = getCamera("Camera");
  mCamera->enable(16);

  mEyeLed = getLED("EyeLed");
  mHeadLed = getLED("HeadLed");
  mBackLedRed = getLED("BackLedRed");
  mBackLedGreen = getLED("BackLedGreen");
  mBackLedBlue = getLED("BackLedBlue");

  for (int i=0; i<NSERVOS; i++)
    mServos[i] = getServo(servoNames[i]);

}

Remote::~Remote() {
}

void Remote::myStep() {
  int ret = step(1); // we want the step the shorter as possible (Webots is in charge of the timing management)
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

void Remote::remoteStep() {
  myStep();
}

const double * Remote::getRemoteAccelerometer() const {
  return mAccelerometer->getValues();
}

const double * Remote::getRemoteGyro() const {
  return mGyro->getValues();
}

const unsigned char * Remote::getRemoteImage() const {
  return mCamera->getImage();
}

void Remote::setRemoteLED(int index, int value) {
  switch(index) {
    case 0: mEyeLed->set(value);
      break;
    case 1: mHeadLed->set(value);
      break;
    case 2: mBackLedRed->set(value);
      break;
    case 3: mBackLedGreen->set(value);
      break;
    case 4: mBackLedBlue->set(value);
      break;
  }
}

void Remote::setRemoteServoPosition(int index, int value) {
  double position = (M_PI * value) / 2048;
  mServos[index]->setPosition(position);
}

void Remote::setRemoteServoVelocity(int index, int value) {
  double velocity = (value*0.114*M_PI) / 30;
  mServos[index]->setVelocity(velocity);
}

void Remote::setRemoteServoAcceleration(int index, int value) {
  double acceleration = value/100000;
  mServos[index]->setAcceleration(acceleration);
}

void Remote::setRemoteServoMotorForce(int index, int value) {
  double force = (value * 2.5) / 1023;
  mServos[index]->setMotorForce(force);
}

void Remote::setRemoteServoForce(int index, int value) {
  double force = (value * 2.5) / 1023;
  mServos[index]->setForce(force);
}

void Remote::setRemoteServoControlP(int index, int value) {
  double p = value/1000;
  mServos[index]->setControlP(p);
}

double Remote::getRemoteServoPosition(int index) {
  return (mServos[index]->getPosition()*10000);
}

double Remote::getRemoteServoForce(int index) {
  return (mServos[index]->getMotorForceFeedback()*10000);
}

double Remote::getRemoteTime() const {
  return this->getTime();
}
