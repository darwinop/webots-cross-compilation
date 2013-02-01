#include "DarwinOutputPacket.hpp"

#include "Camera.hpp"
#include "Device.hpp"
#include "DeviceManager.hpp"
#include "Led.hpp"
#include "Servo.hpp"
#include "Sensor.hpp"
#include "TripleValuesSensor.hpp"
#include "SingleValueSensor.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

#include <webots/camera.h>

using namespace std;

DarwinOutputPacket::DarwinOutputPacket() :
  Packet(50000),
  mAnswerSize(0),
  mAccelerometerRequested(false),
  mGyroRequested(false),
  mCameraRequested(false)
{ 
  for(int c=0; c<20; c++) {
    mServoPositionFeedback[c] = false;
    mServoForceFeedback[c] = false;
  }
}

DarwinOutputPacket::~DarwinOutputPacket() {
}

void DarwinOutputPacket::clear() {
  Packet::clear();

  mAnswerSize = 0;

  mAccelerometerRequested = false;
  mGyroRequested = false;
  mCameraRequested = false;
  
  for(int c=0; c<20; c++) {
    mServoPositionFeedback[c] = false;
    mServoForceFeedback[c] = false;
  }
}

void DarwinOutputPacket::apply(int simulationTime) {

  mAnswerSize = 1;
  append(QByteArray(1, 'W'));

  // ---
  // Sensors
  // ---

  // the order of the sensors should match with DarwinInputPacket::decode()

  // accelerometer management
  TripleValuesSensor *accelerometer = DeviceManager::instance()->accelerometer();
  if (accelerometer->isSensorRequested()) {
    mAccelerometerRequested = true;
    append(QByteArray(1, 'A'));
    mAnswerSize += 3 * 4;
  }

  // gyro management
  TripleValuesSensor *gyro = DeviceManager::instance()->gyro();
  if (gyro->isSensorRequested()) {
    mGyroRequested = true;
    append(QByteArray(1, 'G'));
    mAnswerSize += 3 * 4;
  }

  // camera management
  // it's better to put the camera at the end in case of
  // retrieval after transmission troubles
  CameraR *camera = DeviceManager::instance()->camera();
  if (camera->isSensorRequested()) {
    mCameraRequested = true;
    append(QByteArray(1, 'C'));
    mAnswerSize += 4 * 320 * 240;
  }
  
  
  // ---
  // Actuators
  // ---

  // send the led commands if required
  for (int i=0; i<5; i++) {
    Led *led = DeviceManager::instance()->led(i);
    if (led->isLedRequested()) {
      append(QByteArray(1, 'L'));
      append(QByteArray(1, ((led->index()) & 0xFF)));
      append(QByteArray(1, (led->state() >> 16) & 0xFF));
      append(QByteArray(1, (led->state() >> 8) & 0xFF));
      append(QByteArray(1, led->state() & 0xFF));
      led->resetLedRequested();
    }
  }
  
  // Servos management
  for (int i=0; i<20; i++) {
    ServoR *servo = DeviceManager::instance()->servo(i);
    if (servo->isServoRequested()) {
      append(QByteArray(1, 'S'));
      append(QByteArray(1, ((servo->index()) & 0xFF)));
      
      // Position
      if (servo->isPositionRequested()) {
        append(QByteArray(1, 'p'));
        int value = (int)((servo->position() * 2048) / M_PI);
        appendINT(value);
        servo->resetPositionRequested();
      }
      // Velocity
      if (servo->isVelocityRequested()) {
        append(QByteArray(1, 'v'));
        int value = (int)((servo->velocity() * 30) / (0.114 * M_PI));
        appendINT(value);
        servo->resetVelocityRequested();
      }
      // Acceleration
      if (servo->isAccelerationRequested()) {
        append(QByteArray(1, 'a'));
        int value = (int)(servo->acceleration() * 100000);
        appendINT(value);
        servo->resetAccelerationRequested();
      }
      // MotorForce
      if (servo->isMotorForceRequested()) {
        append(QByteArray(1, 'm'));
        int value = (int)((servo->motorForce() * 1023) / 2.5);
        appendINT(value);
        servo->resetMotorForceRequested();
      }
      // ControlP
      if (servo->isControlPRequested()) {
        append(QByteArray(1, 'c'));
        int value = (int)(servo->controlP() * 1000);
        appendINT(value);
        servo->resetControlPRequested();
      }
      // Force
      if (servo->isForceRequested()) {
        append(QByteArray(1, 'f'));
        int value = (int)((servo->force() * 1023) / 2.5);
        appendINT(value);
        servo->resetForceRequested();
      }
      servo->resetServoRequested();
    }
  }

  for (int i=0; i<20; i++) {
    ServoR *servo = DeviceManager::instance()->servo(i);
    if (servo->isSensorRequested()) {
      mServoPositionFeedback[i] = true;
      append(QByteArray(1, 'P'));
      append(QByteArray(1, ((servo->index()) & 0xFF)));
      mAnswerSize += 4;
    }
  }
  
  for (int i=0; i<20; i++) {
    SingleValueSensor *servoForceFeedback = DeviceManager::instance()->servoForceFeedback(i);
    if (servoForceFeedback->isSensorRequested()) {
      mServoForceFeedback[i] = true;
      append(QByteArray(1, 'F'));
      append(QByteArray(1, ((servoForceFeedback->index()) & 0xFF)));
      mAnswerSize += 4;
    }
  }

  // This is require to end the packet
  // even if the size is correct
  append('\0');
}
