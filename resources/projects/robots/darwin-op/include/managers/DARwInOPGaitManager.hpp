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
  class Walking;
}

namespace managers {
  using namespace Robot;
  class DARwInOPGaitManager {
    public:
                       DARwInOPGaitManager(webots::Robot *robot, const std::string &iniFilename);
      virtual         ~DARwInOPGaitManager();
      bool             isCorrectlyInitialized() { return mCorrectlyInitialized; }
      
      void             setXAmplitude(double x) { mXAmplitude = DGM_BOUND(x, -1.0, 1.0) * 20.0; } 
      void             setYAmplitude(double y) { mYAmplitude = DGM_BOUND(y, -1.0, 1.0) * 40.0; }
      void             setAAmplitude(double a) { mAAmplitude = DGM_BOUND(a, -1.0, 1.0) * 50.0; }
      void             setMoveAimOn(bool q) { mMoveAimOn = q; }
      void             setBalanceEnable(bool q) { mBalanceEnable = q; }

      void             start();
      void             step(int ms);
      void             stop();

    private:
      webots::Robot   *mRobot;
      bool             mCorrectlyInitialized;
      Walking         *mWalking;
      double           mXAmplitude;
      double           mAAmplitude;
      double           mYAmplitude;
      bool             mMoveAimOn;
      bool             mBalanceEnable;

#ifndef CROSSCOMPILATION
      double           valueToPosition(unsigned short value);
      webots::Servo   *mServos[DGM_NSERVOS];
#endif
  };
}

#endif
