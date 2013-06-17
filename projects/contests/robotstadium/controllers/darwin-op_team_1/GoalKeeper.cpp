#include "GoalKeeper.hpp"
#include <DARwInOPMotionManager.hpp>

using namespace webots;

GoalKeeper::GoalKeeper(int playerID, int teamID) : Player(playerID, teamID) {
}

GoalKeeper::~GoalKeeper() {
}

void GoalKeeper::run() {
  step(SIMULATION_STEP);

  while (true) {
    resetAllMotors();
    runStep();
  }
}
