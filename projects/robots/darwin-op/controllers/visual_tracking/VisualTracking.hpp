// File:          VisualTracking.hpp
// Date:          7th of December 2011
// Description:   Simple controller showing how to use vision manager
// Author:        david.mansolino@epfl.ch

#ifndef VISUALTRACKING_HPP
#define VISUALTRACKING_HPP

#define NSERVOS 20

#include <webots/Robot.hpp>

namespace managers {
  class DARwInOPVisionManager;
}

namespace webots {
  class Servo;
  class LED;
  class Camera;

  
  class VisualTracking : public Robot {
    public:
                                       VisualTracking();
      virtual                         ~VisualTracking();
      void                             run();
      
    private:
      int                              mTimeStep;
      
      void                             myStep();
      
      Servo                           *mServos[NSERVOS];
      LED                             *mEyeLED;
      LED                             *mHeadLED;
      Camera                          *mCamera;
      
      managers::DARwInOPVisionManager *mVisionManager;
  };
};

#endif