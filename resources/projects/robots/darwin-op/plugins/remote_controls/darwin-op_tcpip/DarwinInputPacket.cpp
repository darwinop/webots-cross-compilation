#include "DarwinInputPacket.hpp"

#include "Camera.hpp"
#include "Device.hpp"
#include "DeviceManager.hpp"
#include "DarwinOutputPacket.hpp"
#include "Sensor.hpp"
#include "Motor.hpp"
#include "TripleValuesSensor.hpp"
#include "SingleValueSensor.hpp"

#include <webots/remote_control.h>

#include <iostream>

#include <cstdlib>

#include <QtCore/QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QtWidgets>
#else
#include <QtGui/QtGui>
#endif

#include <core/StandardPaths.hpp>

using namespace std;

DarwinInputPacket::DarwinInputPacket(int maxSize) :
  Packet(maxSize)
{
  CameraR *camera = DeviceManager::instance()->camera();
  mCameraWidth = camera->width();
  mCameraHeight = camera->height();
  QCoreApplication::addLibraryPath(webotsQtUtils::StandardPaths::getWebotsHomePath() + "/lib/qt");
}

DarwinInputPacket::~DarwinInputPacket() {
}

void DarwinInputPacket::decode(int simulationTime, const DarwinOutputPacket &outputPacket) {
  // ---
  // Sensors
  // ---
  
  // the order of the sensors should match with DarwinOutputPacket::apply()
 
  int currentPos = 5;

  // Accelerometer
  if (outputPacket.isAccelerometerRequested()) {
    double values[3];
    for (int i = 0; i < 3; i++) {
      values[i] = (double)readIntAt(currentPos);
      currentPos += 4;
    }
    TripleValuesSensor *accelerometer = DeviceManager::instance()->accelerometer();
    wbr_accelerometer_set_values(accelerometer->tag(), values);
    accelerometer->resetSensorRequested();
  }
  
  // Gyro
  if (outputPacket.isGyroRequested()) {
    double values[3];
    for (int i = 0; i < 3; i++) {
      values[i] = (double)readIntAt(currentPos);
      currentPos += 4;
    }
    TripleValuesSensor *gyro = DeviceManager::instance()->gyro();
    wbr_gyro_set_values(gyro->tag(), values);
    gyro->resetSensorRequested();
  }

  // Camera
  if (outputPacket.isCameraRequested()) {
    
    int image_length = readIntAt(currentPos);
    currentPos += 4;
    
    CameraR *camera = DeviceManager::instance()->camera();

    QImage image(mCameraWidth, mCameraHeight, QImage::Format_RGB32);
    if (!(image.loadFromData(getBufferFromPos(currentPos), image_length, "JPEG")))
      printf("Problem while loading jpeg image\n");
      
    // Convert RGB buffer to BGRA buffer
    static unsigned char imageBGRA[320 * 240 * 4];
    for (int i = 0; i < mCameraHeight; i++) {
      for (int j = 0; j < mCameraWidth; j++) {
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 0] = qBlue(image.pixel(j,i));
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 1] = qGreen(image.pixel(j,i));
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 2] = qRed(image.pixel(j,i));
        imageBGRA[i * 4 * mCameraWidth + j * 4 + 3] = 255;
      }
    }
    wbr_camera_set_image(camera->tag(), (const unsigned char *)imageBGRA);
    camera->resetSensorRequested();
    currentPos += image_length;
  }
  
  // Motor position feedback
  for (int i = 0; i < 20; i++) {
    if (outputPacket.isMotorPositionFeedback(i)) {
      double value = (double)readIntAt(currentPos)  / 10000;
      currentPos += 4;
      MotorR *motor = DeviceManager::instance()->motor(i);
      wbr_motor_set_position_feedback(motor->tag(), value);
      motor->resetSensorRequested();
    }
  }
  
  // Motor torque feedback
  for (int i = 0; i < 20; i++) {
    if (outputPacket.isMotorForceFeedback(i)) {
      double value = (double)readIntAt(currentPos) / 10000;
      currentPos += 4;
      SingleValueSensor *motorForceFeedback = DeviceManager::instance()->motorForceFeedback(i);
      wbr_motor_set_torque_feedback(motorForceFeedback->tag(), value);
      motorForceFeedback->resetSensorRequested();
    }
  }
  
}
