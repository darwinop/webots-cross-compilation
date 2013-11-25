#include "FieldPlayer.hpp"
#include "../nao_soccer_supervisor/RoboCupGameControlData.h"
#include <DARwInOPMotionManager.hpp>
#include <DARwInOPGaitManager.hpp>
#include <iostream>
#include <cmath>
#include <cassert>

using namespace webots;

static double clamp(double value, double min, double max) {
  if (min > max) {
    assert(0);
    return value;
  }
  return value < min ? min : value > max ? max : value;
}

FieldPlayer::FieldPlayer(int playerID, int teamID) : Player(playerID, teamID) {
}

FieldPlayer::~FieldPlayer() {
}

// simple and not robust ball detection algorithm
// finding the last pixel which seems like the ball color
// - return: indicates if the algorithm found the ball
// - args: return the position of the ball [-1.0, 1.0]
bool FieldPlayer::getBallCenter(double &x, double &y) {
  static int width  = camera->getWidth();
  static int height = camera->getHeight();
  
  const unsigned char *im = camera->getImage();
  int x0 = -1;
  int y0 = -1;
  for (int j=0; j<height; j++) {
    for (int i=0; i<width; i++) {
      int r = Camera::imageGetRed  (im, width, i, j);
      int g = Camera::imageGetGreen(im, width, i, j);
      int b = Camera::imageGetBlue (im, width, i, j);
      if (r>200 && g>60 && g<205 && b<40) {
        x0 = i;
        y0 = j;
      }
    }
  }
  
  if (x0==-1 && y0==-1) {
    x = 0.0;
    y = 0.0;
    return false;
  } else {
    x = 2.0 * x0 / width  - 1.0;
    y = 2.0 * y0 / height - 1.0;
    return true;
  }
}

void FieldPlayer::run() {
  // play the motion preparing the robot to walk
  gaitManager->start();
  
  // main loop
  double px = 0.0;
  double py = 0.0;
  int fup = 0;
  int fdown = 0;
  const double acc_tolerance = 80.0;
  const double acc_step = 20;
  
  bool wasWaiting = false;

  while (true) {
    if (gameControlData->state != STATE_PLAYING) {
      resetAllMotors();
      wasWaiting = true;
      continue;
    }
    
    if (wasWaiting) {
      wasWaiting = false;
      motionManager->playPage(9);
      sleepSteps(5);
    }

    double x, y, neckPosition, headPosition;
    bool ballInFieldOfView = getBallCenter(x, y);
    const double *acc = accelerometer->getValues();
    const double xFactor = 0.5;
    const double aFactor = 0.5;
    
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
      motionManager->playPage(1); // init position
      motionManager->playPage(10); // f_up
      motionManager->playPage(9); // walkready position    
      fup = 0;
    }
    // the back face is down
    else if (fdown > acc_step) {
      motionManager->playPage(1); // init position
      motionManager->playPage(11); // b_up
      motionManager->playPage(9); // walkready position
      fdown = 0;
    }
    // if the ball is in the field of view,
    // go in the direction of the ball and kick it
    else if (ballInFieldOfView) {
      // compute the direction of the head
      // the head move at maximum by 0.01 [rad] at each time step
      x  = 0.01*x + px;
      y  = 0.01*y + py;
      px = x;
      py = y;
      neckPosition = clamp(-x, motorMinPositions[18], motorMaxPositions[18]);
      headPosition = clamp(-y, motorMinPositions[19], motorMaxPositions[19]);

      // go forwards and turn according to the head rotation
      if (y < 0.1) // ball far away, go quickly
        gaitManager->setXAmplitude(1.0 * xFactor);
      else // ball close, go slowly
        gaitManager->setXAmplitude(0.5 * xFactor);
      gaitManager->setAAmplitude(aFactor * neckPosition);
      gaitManager->step(SIMULATION_STEP);
      
      // Move head
      motors[18]->setPosition(neckPosition);
      motors[19]->setPosition(headPosition);
      
      // if the ball is close enough
      // kick the ball with the right foot
      if (y > 0.4) {
        sleepSteps(5);
        if (x < 0.0)
          motionManager->playPage(13); // left kick
        else
          motionManager->playPage(12); // right kick
        motionManager->playPage(9); // walkready position
        sleepSteps(5);
        px = 0.0;
        py = 0.0;
      }
    
    // the ball is not in the field of view,
    // search it by turning round and moving vertically the head 
    } else {
      // turn round
      gaitManager->setXAmplitude(0.0);
      gaitManager->setAAmplitude(aFactor);
      gaitManager->step(SIMULATION_STEP);
      
      // move the head vertically
      motors[18]->setPosition(0.0);
      neckPosition = -0.7*sin(2.0*getTime());
      neckPosition = clamp(neckPosition, motorMinPositions[19], motorMaxPositions[19]);
      motors[19]->setPosition(neckPosition);
    }
    
    // step
    runStep();
  }
}
