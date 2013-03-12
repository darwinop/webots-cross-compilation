#include "Symmetry.hpp"
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

//Constructor
Symmetry::Symmetry():
    Robot()
{
  //Get time step
  mTimeStep = getBasicTimeStep();
  
  //Get the two RGB_LEDs
  mEyeLED = getLED("EyeLed");
  mHeadLED = getLED("HeadLed");
  
  //Get all the 20 Servos and enable them
  for (int i=0; i<NSERVOS; i++) {
    mServos[i] = getServo(servoNames[i]);
    mServos[i]->enablePosition(mTimeStep);
  }

}

//Destructor
Symmetry::~Symmetry() {
}

//Step function
void Symmetry::myStep() {
  int ret = step(mTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

//Wait function
void Symmetry::wait(int ms) {
  double startTime = getTime();
  double s = (double) ms / 1000.0;
  while (s + startTime >= getTime())
    myStep();
}

// function containing the main feedback loop
void Symmetry::run() {
  
  cout << "-------Symmetry example of DARwIn-OP-------" << endl;
  cout << "The right arm is free while the left one mimic it." << endl;
  cout << "In order to move the left arm, select the robot with the mouse," << endl;
  cout << "press ctr+alt and select the right arm." << endl;
  cout << "Now you just have to move the mouse without releasing it." << endl;
  cout << "This example also illustrates self-collision which is active by default" << endl;
  
  double position[3] = {0,0,0};
  
  mServos[0]->setMotorForce(0.0);
  mServos[2]->setMotorForce(0.0);
  mServos[4]->setMotorForce(0.0);
    
  // step
  myStep();
  
  while (true) {
      
    // Get position of right arm of the robot
    position[0] = mServos[0]->getPosition();
    position[1] = mServos[2]->getPosition();
    position[2] = mServos[4]->getPosition();
    
    // Set position of XX arm of the robot
    // the inversion of sign is done because of the symmetry
    mServos[1]->setPosition(-position[0]);
    mServos[3]->setPosition(-position[1]);
    mServos[5]->setPosition(-position[2]);
    
    // step
    myStep();
  }
}
