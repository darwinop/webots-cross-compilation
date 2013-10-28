#include "Player.hpp"
#include "../nao_soccer_supervisor/RoboCupGameControlData.h"
#include <webots/Accelerometer.hpp>
#include <webots/Gyro.hpp>
#include <webots/Motor.hpp>
#include <webots/LED.hpp>
#include <webots/Emitter.hpp>
#include <webots/Receiver.hpp>
#include <webots/Motor.hpp>
#include <DARwInOPMotionManager.hpp>
#include <DARwInOPGaitManager.hpp>

#include <string>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <iostream>

using namespace std;
using namespace webots;
using namespace managers;

const int Player::SIMULATION_STEP = 40;  // milliseconds

static const char *motorNames[Player::NMOTORS] = {
  "ShoulderR" /*ID1 */, "ShoulderL" /*ID2 */, "ArmUpperR" /*ID3 */, "ArmUpperL" /*ID4 */,
  "ArmLowerR" /*ID5 */, "ArmLowerL" /*ID6 */, "PelvYR"    /*ID7 */, "PelvYL"    /*ID8 */,
  "PelvR"     /*ID9 */, "PelvL"     /*ID10*/, "LegUpperR" /*ID11*/, "LegUpperL" /*ID12*/,
  "LegLowerR" /*ID13*/, "LegLowerL" /*ID14*/, "AnkleR"    /*ID15*/, "AnkleL"    /*ID16*/,
  "FootR"     /*ID17*/, "FootL"     /*ID18*/, "Neck"      /*ID19*/, "Head"      /*ID20*/
};

static double motorInitPositions[Player::NMOTORS];

Player::Player(int playerID, int teamID) {
 
  // need space to store incoming data
  gameControlData = new RoboCupGameControlData;
  gameControlData->state = STATE_INITIAL;

  this->playerID = playerID;
  this->teamID = teamID;

  // get managers
  motionManager = new DARwInOPMotionManager(this);
  gaitManager = new DARwInOPGaitManager(this, "config.ini");
  
  // initialize accelerometer
  accelerometer = getAccelerometer("Accelerometer");
  accelerometer->enable(SIMULATION_STEP);

  // initialize gyrp
  gyro = getGyro("Gyro");
  gyro->enable(SIMULATION_STEP);

  // get all LEDs
  eyeLed = getLED("EyeLed");
  headLed = getLED("HeadLed");
  camera = getCamera("Camera");
  camera->enable(4*SIMULATION_STEP);
  
  // emitter/receiver devices that can be used for inter-robot communication
  // and for receiving RobotCupGameControleData
  emitter = getEmitter("emitter");
  receiver = getReceiver("receiver");
  receiver->enable(SIMULATION_STEP);

  // for sending 'move' request to Supervisor
  superEmitter = getEmitter("super_emitter");

  // get motors
  for (int i=0; i<NMOTORS; i++) {
    motors[i] = getMotor(motorNames[i]);
    motors[i]->enablePosition(SIMULATION_STEP);
    motorMinPositions[i] = motors[i]->getMinPosition();
    motorMaxPositions[i] = motors[i]->getMaxPosition();
  }

  while (isnan(motors[0]->getPosition()))
    step(SIMULATION_STEP);
  
  // get the init positions
  for (int i=0; i<NMOTORS; i++)
    motorInitPositions[i] = motors[i]->getPosition();
}

Player::~Player() {
  delete gameControlData;
}

bool Player::isBlue() const {
  return gameControlData->teams[TEAM_BLUE].teamNumber == teamID;
}

bool Player::isRed() const {
  return gameControlData->teams[TEAM_RED].teamNumber == teamID;
}

void Player::updateGameControl() {

  // choose goal color according to my team's color
  // and display team color on left foot LED
  if (isRed())
    headLed->set(0xC04040);
  else
    headLed->set(0x4040C0);
  
  // update chest button color according to game state
  switch (gameControlData->state) {
    case STATE_INITIAL:
    case STATE_FINISHED:
      eyeLed->set(0x000000);  // off
      break;
    case STATE_READY:
      eyeLed->set(0x2222ff);  // blue
      break;
    case STATE_SET:
      eyeLed->set(0xffff22);  // yellow
      break;
    case STATE_PLAYING:
      eyeLed->set(0x22ff22);  // green
      break;
  }
}

// information that describes a team
void Player::printTeamInfo(const struct TeamInfo *team) {
  cout << "  teamNumber: " << (int)team->teamNumber << endl;
  cout << "  teamColour: " << (int)team->teamColour << endl;
  cout << "  score: " << (int)team->score << endl;
}

// robocup game control date
void Player::printGameControlData(const struct RoboCupGameControlData *gcd) {
  cout << "----------RoboCupGameControlData----------\n";
  cout << "playersPerTeam: " << (int)gcd->playersPerTeam << endl;
  cout << "state: " << (int)gcd->state << endl;
  cout << "firstHalf: " << (int)gcd->firstHalf << endl;
  cout << "kickOffTeam: " << (int)gcd->kickOffTeam << endl;
  cout << "secsRemaining: " << (int)gcd->secsRemaining << endl;
  cout << "teams[TEAM_BLUE]:\n";
  printTeamInfo(&gcd->teams[TEAM_BLUE]);
  cout << "teams[TEAM_RED]:\n";
  printTeamInfo(&gcd->teams[TEAM_RED]);
  // For training only: this will be 0.0 during Robotstadium contest rounds:
  cout << "ballXPos: " << gcd->ballXPos << endl;
  cout << "ballZPos: " << gcd->ballZPos << endl;
  cout << "----------RoboCupGameControlData----------\n";
}

void Player::readIncomingMessages() {
  while (receiver->getQueueLength() > 0) {
    const void *data = receiver->getData();

    // important: verify packet header type
    if (memcmp(data, GAMECONTROLLER_STRUCT_HEADER, sizeof(GAMECONTROLLER_STRUCT_HEADER) - 1) == 0) {
      memcpy(gameControlData, data, sizeof(RoboCupGameControlData));
      //printGameControlData(gameControlData);  // uncomment to debug
      updateGameControl();
    }
    
    receiver->nextPacket();
  }
}

// move the robot to a specified position (via a message sent to the Supervisor)
// [tx ty tz]: the new robot position, alpha: the robot's heading direction
// For debugging only: this is disabled during the contest rounds
void Player::sendMoveRobotMessage(double tx, double ty, double tz, double alpha) {
  char buf[256];
  sprintf(buf, "move robot %d %d %f %f %f %f", playerID, teamID, tx, ty, tz, alpha);
  superEmitter->send(buf, strlen(buf) + 1);
}

// move the ball to a specified position (via a message sent to the Supervisor)
// [tx ty tz]: the new ball position
// For debugging only: this is disabled during the contest rounds
void Player::sendMoveBallMessage(double tx, double ty, double tz) {
  char buf[256];
  sprintf(buf, "move ball %f %f %f", tx, ty, tz);
  superEmitter->send(buf, strlen(buf) + 1);
}

void Player::runStep()  {
  step(SIMULATION_STEP);
}

// overidden method of the Robot baseclass
// we need to read incoming messages at every step
int Player::step(int ms) {
  readIncomingMessages();
  return Robot::step(ms);
}

void Player::sleepSteps(int steps) {
  for (int i = 0; i < steps; i++)
    step(SIMULATION_STEP);
}

// set all the motor position to 0 and
// wait at lest wait_ms milliseconds
void Player::resetAllMotors(int wait_ms) {
  for (int i=0; i<NMOTORS; i++)
    motors[i]->setPosition(motorInitPositions[i]);

  int steps_to_wait = wait_ms / SIMULATION_STEP;
  sleepSteps(steps_to_wait);
}

