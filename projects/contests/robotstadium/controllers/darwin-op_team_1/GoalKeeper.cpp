#include "GoalKeeper.hpp"
#include <managers/DARwInOPMotionManager.hpp>

using namespace webots;

GoalKeeper::GoalKeeper(int playerID, int teamID) : Player(playerID, teamID) {
}

GoalKeeper::~GoalKeeper() {
}

void GoalKeeper::run() {
  step(SIMULATION_STEP);

  while (true) {
    resetAllServos();
    runStep();
  }
}
