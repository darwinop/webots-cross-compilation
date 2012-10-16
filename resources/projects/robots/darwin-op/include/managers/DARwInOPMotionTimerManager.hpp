// File:          DARwInOPMotionTimerManager.hpp
// Date:          16th of October 2012
// Description:   Facade between webots and the darwin-op framework
//                allowing to to start the LinuxMotionTimer in order
//                to play the Robotis motion files and the walking algorithm.
// Author:        david.mansolino@epfl.ch

#ifndef DARWINOP_MOTION_TIMER_MANAGER_HPP
#define DARWINOP_MOTION_TIMER_MANAGER_HPP

namespace Robot {
  class LinuxMotionTimer;
}

namespace managers {
  using namespace Robot;
  class DARwInOPMotionTimerManager {
    public:
                       DARwInOPMotionTimerManager();
      virtual         ~DARwInOPMotionTimerManager();
      static void      MotionTimerInit();

    private:
      static bool      mStarted;

  };
}

#endif
