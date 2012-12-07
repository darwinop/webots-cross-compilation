#include "VisualTracking.hpp"
#include <webots/Servo.hpp>
#include <webots/LED.hpp>
#include <webots/Camera.hpp>
#include <managers/DARwInOPVisionManager.hpp>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace webots;
using namespace managers;
using namespace std;

static const char *servoNames[NSERVOS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

VisualTracking::VisualTracking():
    Robot()
{
  mTimeStep = getBasicTimeStep();
  
  mEyeLED = getLED("EyeLed");
  mHeadLED = getLED("HeadLed");
  mCamera = getCamera("Camera");
  mCamera->enable(mTimeStep);
  
  for (int i=0; i<NSERVOS; i++) {
    mServos[i] = getServo(servoNames[i]);
    mServos[i]->enablePosition(mTimeStep);
  }
  
  mVisionManager = new DARwInOPVisionManager(mCamera->getWidth(), mCamera->getHeight(), 350, 15, 50, 10, 0.02, 30);
}

VisualTracking::~VisualTracking() {
}

void VisualTracking::myStep() {
  int ret = step(mTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

// function containing the main feedback loop
void VisualTracking::run() {
	
  cout << "---------------Visual Tracking---------------" << endl;
  cout << "This example illustrate the possibilities of the vision manager." << endl;
  cout << "Move the red ball by pressing ctrl + shift and selecting it (mouse left pressed)." << endl;
	
  // First step to update sensors values
  myStep();

  while (true) {
    double x, y;
    bool ballInFieldOfView = mVisionManager->getBallCenter(x, y, mCamera->getImage());
    
    // Eye led indicate if ball has been found
    if(ballInFieldOfView)
      mEyeLED->set(0x00FF00);
    else
      mEyeLED->set(0xFF0000);
    
    // Move the head in direction of the ball if it has been found
    if(ballInFieldOfView) {
      // Horizontal
      if(x > 0.6 * mCamera->getWidth())
        mServos[18]->setPosition(mServos[18]->getPosition() - 0.2);
      else if(x < 0.4 * mCamera->getWidth())
        mServos[18]->setPosition(mServos[18]->getPosition() + 0.2);
     // Vertical
      if(y > 0.6 * mCamera->getHeight())
        mServos[19]->setPosition(mServos[19]->getPosition() - 0.1);
      else if(y < 0.4 * mCamera->getHeight())
        mServos[19]->setPosition(mServos[19]->getPosition() + 0.1);
    }
    
    
    // step
    myStep();
  }
}