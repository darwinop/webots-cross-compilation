#ifndef PLAYER_HPP
#define PLAYER_HPP

//-----------------------------------------------------------------------------
//  File:         Player class (to be used in a Webots controllers)
//  Description:  Base class for FieldPlayer and GoalKeeper
//  Project:      Robotstadium, the online robot soccer competition
//  Author:       Yvan Bourquin - www.cyberbotics.com
//  Date:         May 4, 2008
//  Changes:      Sep 27, 2011: Updated for the DARwIn-OP (Fabien Rohrer)
//-----------------------------------------------------------------------------

#include <webots/Robot.hpp>

namespace webots {
  class Accelerometer;
  class Camera;
  class Gyro;
  class LED;
  class Emitter;
  class Receiver;
  class Motor;
}

namespace managers {
  class DARwInOPGaitManager;
  class DARwInOPMotionManager;
}

using namespace webots;
using namespace std;

class Player : public Robot {
public:
  Player(int playerID, int teamID);
  virtual ~Player();
  
  // find out which color to play
  bool isBlue() const;
  bool isRed() const;
  
  enum { NMOTORS = 20 };
  
  // pure virtual: effective implementation in derived classes
  virtual void run() = 0;
  
  // overridden method
  virtual int step(int ms);

protected:
  virtual void runStep();
  virtual void sleepSteps(int steps);
  virtual void resetAllMotors(int wait_ms = 1000);
  
  // global control step (must be a multiple of WorldInfo.basicTimeStep)
  static const int SIMULATION_STEP;

  struct RoboCupGameControlData *gameControlData;
  int playerID, teamID;

  // devices
  Camera *camera;
  Motor *motors[NMOTORS];
  Accelerometer *accelerometer;
  Gyro *gyro;
  LED *eyeLed, *headLed;
  Emitter *emitter, *superEmitter;
  Receiver *receiver;
  
  double motorMinPositions[Player::NMOTORS];
  double motorMaxPositions[Player::NMOTORS];
  
  managers::DARwInOPGaitManager *gaitManager;
  managers::DARwInOPMotionManager *motionManager;

private:
  void updateGameControl();
  void printTeamInfo(const struct TeamInfo *team);
  void printGameControlData(const struct RoboCupGameControlData *gcd);
  void readIncomingMessages();
  void sendMoveRobotMessage(double tx, double ty, double tz, double alpha);
  void sendMoveBallMessage(double tx, double ty, double tz);
};

#endif
