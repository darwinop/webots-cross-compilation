// File:          DARwInOPGaitManager.hpp
// Date:          20th of September 2011
// Description:   Facade between webots and the darwin-op framework
//                allowing to handle the gait generator
// Author:        fabien.rohrer@cyberbotics.com

#ifndef DARWINOP_GAIT_MANAGER_HPP
#define DARWINOP_GAIT_MANAGER_HPP

#include <string>

#define DGM_NSERVOS 20
#define DGM_BOUND(x,a,b) (((x)<(a))?(a):((x)>(b))?(b):(x))

namespace webots {
  class Robot;
  class Servo;
}

namespace Robot {
  class Action;
}

namespace managers {
  class DARwInOPGaitManager {
    public:
                       DARwInOPGaitManager(webots::Robot *robot, const std::string &iniFilename);
      virtual         ~DARwInOPGaitManager();
      bool             isCorrectlyInitialized() { return mCorrectlyInitialized; }
      
      void             setXAmplitude(double x) { mXAmplitude = DGM_BOUND(x, -1.0, 1.0) * 15.0; }
      void             setAAmplitude(double a) { mAAmplitude = DGM_BOUND(a, -1.0, 1.0) * 15.0; }
      void             start();
      void             step(int ms);
      void             stop();

    private:
      webots::Robot   *mRobot;
      bool             mCorrectlyInitialized;
      double           mXAmplitude;
      double           mAAmplitude;

#ifndef CROSSCOMPILATION
      double           valueToPosition(unsigned short value);
      webots::Servo   *mServos[DGM_NSERVOS];
#endif
  };
}

#endif
