// File:          DARwInOPMotionManager.hpp
// Date:          20th of September 2011
// Description:   Facade between webots and the darwin-op framework
//                allowing to play the Robotis motion files
// Author:        fabien.rohrer@cyberbotics.com

#ifndef DARWINOP_MOTION_MANAGER_HPP
#define DARWINOP_MOTION_MANAGER_HPP

#include <string>

#define DMM_NSERVOS 20

namespace webots {
  class Robot;
  class Servo;
}

namespace Robot {
  class Action;
}

namespace managers {
  class DARwInOPMotionManager {
    public:
                       DARwInOPMotionManager(webots::Robot *robot);
      virtual         ~DARwInOPMotionManager();
      bool             isCorrectlyInitialized() { return mCorrectlyInitialized; }
      void             playPage(int id);

    private:
      webots::Robot   *mRobot;
      bool             mCorrectlyInitialized;
      ::Robot::Action   *mAction;
      int              mBasicTimeStep;

#ifndef CROSSCOMPILATION
      void             myStep();
      void             wait(int ms);
      void             achieveTarget(int msToAchieveTarget);
      double           valueToPosition(unsigned short value);

      webots::Servo   *mServos[DMM_NSERVOS];
      double           mTargetPositions[DMM_NSERVOS];
      double           mCurrentPositions[DMM_NSERVOS];
#endif
  };
}

#endif
