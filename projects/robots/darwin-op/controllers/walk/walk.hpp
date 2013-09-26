// File:          Walk.hpp
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
};

class Walk : public webots::Robot {
  public:
                                     Walk();
    virtual                         ~Walk();
    void                             run();
    void                             checkIfFallen();
    
  private:
    int                              mTimeStep;
    
    void                             myStep();
    void                             wait(int ms);
    
    webots::Motor                   *mMotors[NMOTORS];
    webots::Accelerometer           *mAccelerometer;
    
    managers::DARwInOPMotionManager *mMotionManager;
    managers::DARwInOPGaitManager   *mGaitManager;
};

#endif
