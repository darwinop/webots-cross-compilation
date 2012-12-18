#include "Sample.hpp"
#include <webots/Servo.hpp>
#include <webots/LED.hpp>
#include <webots/Camera.hpp>
#include <webots/Accelerometer.hpp>
#include <webots/Gyro.hpp>
#include <managers/DARwInOPMotionManager.hpp>
#include <managers/DARwInOPGaitManager.hpp>
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

Sample::Sample():
    Robot()
{
  mTimeStep = getBasicTimeStep();
  
  mEyeLED = getLED("EyeLed");
  mHeadLED = getLED("HeadLed");
  mHeadLED->set(0x10C040);
  mBackLedRed = getLED("BackLedRed");
  mBackLedGreen = getLED("BackLedGreen");
  mBackLedBlue = getLED("BackLedBlue");
  mCamera = getCamera("Camera");
  mCamera->enable(mTimeStep);
  mAccelerometer = getAccelerometer("Accelerometer");
  mAccelerometer->enable(mTimeStep);
  mGyro = getGyro("Gyro");
  mGyro->enable(mTimeStep);
  
  for (int i=0; i<NSERVOS; i++) {
    mServos[i] = getServo(servoNames[i]);
    mServos[i]->enablePosition(mTimeStep);
  }
  
  mMotionManager = new DARwInOPMotionManager(this);
  mGaitManager = new DARwInOPGaitManager(this, "config.ini");
  mVisionManager = new DARwInOPVisionManager(mCamera->getWidth(), mCamera->getHeight(), 30, 15, 50, 10, 0.01, 30);
}

Sample::~Sample() {
}

void Sample::myStep() {
  int ret = step(mTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

void Sample::wait(int ms) {
  double startTime = getTime();
  double s = (double) ms / 1000.0;
  while (s + startTime >= getTime())
    myStep();
}

// Ball detection based on the ball color using the Vision Manager
// - return: indicates if the algorithm found the ball
// - args: return the position of the ball [-1.0, 1.0]

bool Sample::getBallCenter(double &x, double &y) {
  static int width  = mCamera->getWidth();
  static int height = mCamera->getHeight();
  
  const unsigned char *im = mCamera->getImage();
  bool find = mVisionManager->getBallCenter(x, y, im);
  
  if(!find) {
    x = 0.0;
    y = 0.0;
    return false;
  } else {
    x = 2.0 * x / width  - 1.0;
    y = 2.0 * y / height - 1.0;
    return true;
  }
}

// function containing the main feedback loop
void Sample::run() {
	
  cout << "---------------Demo of DARwIn-OP---------------" << endl;
  cout << "This demo illustrate all the possibilities available for the DARwIn-OP." << endl;
  cout << "Including Motion play, walking algorithm and image processing." << endl;
	
  // First step to update sensors values
  myStep();
	
  // set eye led to green
  mEyeLED->set(0x10C040);
  
  // play the hello motion
  mMotionManager->playPage(1); // init position
  mMotionManager->playPage(24); // hello
  mMotionManager->playPage(9); // walkready position
  wait(200);

  // play the motion preparing the robot to walk
  mGaitManager->start();
  mGaitManager->step(mTimeStep);
  
  // main loop
  double px = 0.0;
  double py = 0.0;
  int fup = 0;
  int fdown = 0;
  const double acc_tolerance = 80.0;
  const double acc_step = 100;
  
  while (true) {
    double x, y;
    bool ballInFieldOfView = getBallCenter(x, y);
    const double *acc = mAccelerometer->getValues();
    
    // count how many steps the accelerometer
    // says that the robot is down
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
      mMotionManager->playPage(1); // init position
      mMotionManager->playPage(10); // f_up
      mMotionManager->playPage(9); // walkready position
      fup = 0;
    }
    // the back face is down
    else if (fdown > acc_step) {
      mMotionManager->playPage(1); // init position
      mMotionManager->playPage(11); // b_up
      mMotionManager->playPage(9); // walkready position
      fdown = 0;
    }
    // if the ball is in the field of view,
    // go in the direction of the ball and kick it
    else if (ballInFieldOfView) {
      // set eye led to blue
      mEyeLED->set(0x1040C0);
      
      // compute the direction of the head
      // the head move at maximum by 0.015 [rad] at each time step
      x  = 0.015*x + px;
      y  = 0.015*y + py;
      px = x;
      py = y;

      // go forwards and turn according to the head rotation
      mGaitManager->setXAmplitude(1.0);
      mGaitManager->setAAmplitude(-x);
      mGaitManager->step(mTimeStep);
      mServos[18]->setPosition(-x);
      mServos[19]->setPosition(-y);
      
      // if the ball is close enough
      // kick the ball with the right foot
      if (y > 0.6) {
        // set eye led to green
        mEyeLED->set(0x10C040);
        if (x<0.0)
          mMotionManager->playPage(13); // left kick
        else
          mMotionManager->playPage(12); // right kick
        mMotionManager->playPage(9); // walkready position
        px = 0.0;
        py = 0.0;
      }
    }
    
    // the ball is not in the field of view,
    // search it by turning round and moving vertically the head 
    else {
      // set eye led to red
      mEyeLED->set(0xC01040);

      // turn round
      mGaitManager->setXAmplitude(0.0);
      mGaitManager->setAAmplitude(0.3);
      mGaitManager->step(mTimeStep);
      
      // move the head vertically
      mServos[19]->setPosition(0.7*sin(2.0*getTime()));
    }
    
    // step
    myStep();
  }
}
