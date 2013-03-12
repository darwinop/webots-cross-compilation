#include "VisualTracking.hpp"
#include <webots/Servo.hpp>
#include <webots/LED.hpp>
#include <webots/Camera.hpp>
#include <DARwInOPVisionManager.hpp>

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
  mCamera->enable(2*mTimeStep);
  
  for (int i=0; i<NSERVOS; i++)
    mServos[i] = getServo(servoNames[i]);

  
  mVisionManager = new DARwInOPVisionManager(mCamera->getWidth(), mCamera->getHeight(), 355, 15, 60, 15, 0.1, 30);
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
  double horizontal = 0.0;
  double vertical = 0.0;
	
  cout << "---------------Visual Tracking---------------" << endl;
  cout << "This example illustrates the possibilities of Vision Manager." << endl;
  cout << "Move the red ball by holding ctrl + shift keys and select it (left mouse click)." << endl;
	
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
        horizontal -= 0.05;
      else if(x < 0.4 * mCamera->getWidth())
        horizontal += 0.05;
      // Vertical
      if(y > 0.6 * mCamera->getHeight())
        vertical -= 0.02;
      else if(y < 0.4 * mCamera->getHeight())
        vertical += 0.02;
    }
    
    mServos[18]->setPosition(horizontal);
    mServos[19]->setPosition(vertical);
    
    // step
    myStep();
  }
}
