// File:          VisualTracking.hpp
// Date:          7th of December 2011
// Description:   Simple controller showing how to use vision manager
// Author:        david.mansolino@epfl.ch

#ifndef VISUALTRACKING_HPP
#define VISUALTRACKING_HPP

#define NMOTORS 20

#include <webots/Robot.hpp>

namespace managers {
  class DARwInOPVisionManager;
}

namespace webots {
  class Motor;
  class LED;
  class Camera;
};

class VisualTracking : public webots::Robot {
  public:
                                     VisualTracking();
    virtual                         ~VisualTracking();
    void                             run();
    
  private:
    int                              mTimeStep;
    
    void                             myStep();
    
    webots::Motor                   *mMotors[NMOTORS];
    webots::LED                     *mEyeLED;
    webots::LED                     *mHeadLED;
    webots::Camera                  *mCamera;
    
    managers::DARwInOPVisionManager *mVisionManager;
};

#endif
