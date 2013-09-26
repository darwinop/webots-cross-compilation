/*
When running this controller in the real robot
do not interface via SSh client (i,e, PuTTY)
The result will be on a segmentation fault error.

Instead interface with the robot via remote desktop
(i.e. VNC). Open a terminal window, compile controller
and run.
*/
#include "Walk.hpp"
#include <webots/LED.hpp>
#include <webots/Accelerometer.hpp>
#include <webots/Gyro.hpp>
#include <webots/Motor.hpp>
#include <DARwInOPMotionManager.hpp>
#include <DARwInOPGaitManager.hpp>

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace webots;
using namespace managers;
using namespace std;

static const char *motorNames[NMOTORS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

Walk::Walk():
    Robot()
{
  mTimeStep = getBasicTimeStep();
  
  getLED("HeadLed")->set(0xFF0000);
  getLED("EyeLed")->set(0x00FF00);
  mAccelerometer = getAccelerometer("Accelerometer");
  mAccelerometer->enable(mTimeStep);
  
  getGyro("Gyro")->enable(mTimeStep);
  
  for (int i=0; i<NMOTORS; i++) {
    mMotors[i] = getMotor(motorNames[i]);
    mMotors[i]->enablePosition(mTimeStep);
  }
  
  keyboardEnable(mTimeStep);
  
  mMotionManager = new DARwInOPMotionManager(this);
  mGaitManager = new DARwInOPGaitManager(this, "config.ini");
  
}

Walk::~Walk() {
}

void Walk::myStep() {
  int ret = step(mTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

void Walk::wait(int ms) {
  double startTime = getTime();
  double s = (double) ms / 1000.0;
  while (s + startTime >= getTime())
    myStep();
}



// function containing the main feedback loop
void Walk::run() {

  cout << "-------Walk example of DARwIn-OP-------" << endl;
  cout << "This example illustrates Gait Manager" << endl;
  cout << "Press the space bar to start/stop walking" << endl;
  cout << "Use the arrow keys to move the robot while walking" << endl;

  // First step to update sensors values
  myStep();

  // play the hello motion
  mMotionManager->playPage(9); // init position
  wait(200);
  
  // main loop
  int key = 0;
  bool isWalking = false;
  
  while (true) {
    checkIfFallen();

    mGaitManager->setXAmplitude(0.0);
    mGaitManager->setAAmplitude(0.0);
    
    // get keyboard key
    while((key = keyboardGetKey()) != 0) {
      switch(key) {
        case ' ' : // Space bar 
          if(isWalking) {
            mGaitManager->stop(); 
            isWalking = false; 
            wait(200);
          }
          else {
            mGaitManager->start(); 
            isWalking = true; 
            wait(200);
          }
          break;
        case KEYBOARD_UP : 
          mGaitManager->setXAmplitude(1.0);
          break;
        case KEYBOARD_DOWN : 
          mGaitManager->setXAmplitude(-1.0);
          break;
        case KEYBOARD_RIGHT : 
          mGaitManager->setAAmplitude(-0.5);
          break;
        case KEYBOARD_LEFT : 
          mGaitManager->setAAmplitude(0.5);
          break;
      }
    }

    mGaitManager->step(mTimeStep);
    
    // step
    myStep(); 
  }
}

void Walk::checkIfFallen() {
  static int fup = 0;
  static int fdown = 0;
  static const double acc_tolerance = 80.0;
  static const double acc_step = 100;
  
  // count how many steps the accelerometer
  // says that the robot is down
  const double *acc = mAccelerometer->getValues();
  if (acc[1] < 512.0 - acc_tolerance)
    fup++;
  else
    fup = 0;
  
  if (acc[1] > 512.0 + acc_tolerance)
    fdown++;
  else
    fdown = 0;
  
  // the robot face is down
  if (fup > acc_step) {
    mMotionManager->playPage(10); // f_up
    mMotionManager->playPage(9); // init position    
    fup = 0;
  }
  // the back face is down
  else if (fdown > acc_step) {
    mMotionManager->playPage(11); // b_up
    mMotionManager->playPage(9); // init position
    fdown = 0;
  }
}
