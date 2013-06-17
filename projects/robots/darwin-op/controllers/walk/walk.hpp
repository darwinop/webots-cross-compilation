// File:          walk.hpp
// Date:          January 2013
// Description:   Example showing how to use the gait manager
//                and keyboard inputs
// Author:        david.mansolino@epfl.ch

#ifndef WALK_HPP
#define WALK_HPP

#define NMOTORS 20

#include <webots/Robot.hpp>

namespace managers {
  class DARwInOPMotionManager;
  class DARwInOPGaitManager;
}

namespace webots {
  class Motor;
  class LED;
  class Camera;
  class Accelerometer;
  class Gyro;
  class Speaker;
  
  class walk : public Robot {
    public:
                                       walk();
      virtual                         ~walk();
      void                             run();
      void                             checkIfFallen();
      
    private:
      int                              mTimeStep;
      
      void                             myStep();
      void                             wait(int ms);
      
      Motor                           *mMotors[NMOTORS];
      LED                             *mEyeLED;
      LED                             *mHeadLED;
      Accelerometer                   *mAccelerometer;
      
      managers::DARwInOPMotionManager *mMotionManager;
      managers::DARwInOPGaitManager   *mGaitManager;
  };
};

#endif
