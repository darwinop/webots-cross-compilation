#include "Symmetry.hpp"
#include <webots/Motor.hpp>

#ifdef CROSSCOMPILATION
#include <webots/Speaker.hpp>
#endif

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace webots;
using namespace std;

static const char *motorNames[NMOTORS] = {
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
  
  //Get all the 20 Motors and enable them
  for (int i=0; i<NMOTORS; i++) {
    mMotors[i] = getMotor(motorNames[i]);
    mMotors[i]->enablePosition(mTimeStep);
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
  
#ifdef CROSSCOMPILATION
  getSpeaker("Speaker")->speak("Hello, my name is darwin OP. Please move my right arm, and I will move the left one symmetrically.", "en", 120); // English version
  //getSpeaker("Speaker")->speak("Bonjour, je m'appelle darwin OP. Bougez mon bras droit, et je bougerai le gauche symmetriquement.", "fr", 120); // French version
#endif
  
  cout << "-------Symmetry example of DARwIn-OP-------" << endl;
  cout << "The right arm is free while the left one mimics it." << endl;
  cout << "In order to move the left arm, add a force to the right arm:" << endl;
  cout << "keep alt pressed and select the right arm." << endl;
  cout << "Now you just have to move the mouse without releasing it." << endl;
  cout << "This example illustrate also the selfCollision which is activated by default" << endl;
  
  double position[3] = {0,0,0};
  
  mMotors[0]->setAvailableTorque(0.0);
  mMotors[2]->setAvailableTorque(0.0);
  mMotors[4]->setAvailableTorque(0.0);
    
  // step
  myStep();
  
  while (true) {
      
    // Get position of right arm of the robot
    position[0] = mMotors[0]->getPosition();
    position[1] = mMotors[2]->getPosition();
    position[2] = mMotors[4]->getPosition();
    
    // Set position of XX arm of the robot
    // the inversion of sign is done because of the symmetry
    mMotors[1]->setPosition(-position[0]);
    mMotors[3]->setPosition(-position[1]);
    mMotors[5]->setPosition(-position[2]);
    
    // step
    myStep();
  }
}
