#include "MotionPlayer.hpp"

#include <webots/utils/Motion.hpp>

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace webots;

//Constructor
MotionPlayer::MotionPlayer():
    Robot()
{
  //Get time step
  mTimeStep = getBasicTimeStep();
}

//Destructor
MotionPlayer::~MotionPlayer() {
}

//Step function
void MotionPlayer::myStep() {
  int ret = step(mTimeStep);
  if (ret == -1)
    exit(EXIT_SUCCESS);
}

//Wait function
void MotionPlayer::wait(int ms) {
  double startTime = getTime();
  double s = (double) ms / 1000.0;
  while (s + startTime >= getTime())
    myStep();
}

// function containing the main feedback loop
void MotionPlayer::run() {
  
  cout << "-------MotionPlayer example of DARwIn-OP-------" << endl;
  cout << "This example plays a Webots motion file" << endl;
  
  // step
  myStep();
  
  Motion motion("hand_high.motion");
  motion.setLoop(true);
  motion.play();
  while (true)
    myStep();
}
