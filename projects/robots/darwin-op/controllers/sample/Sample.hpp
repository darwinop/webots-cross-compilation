// File:          Sample.hpp
// Date:          20th of September 2011
// Description:   Sample showing how to use the middleware between webots and
//                the darwin-op framework
// Author:        fabien.rohrer@cyberbotics.com

#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#define NSERVOS 20

#include <webots/Robot.hpp>

namespace managers {
  class DARwInOPMotionManager;
  class DARwInOPGaitManager;
}

namespace webots {
  class Servo;
  class LED;
  class Camera;
  
  class Sample : public Robot {
    public:
                                       Sample();
      virtual                         ~Sample();
      void                             run();
      
    private:
      int                              mTimeStep;
      
      void                             myStep();
      void                             wait(int ms);
      bool                             getBallCenter(double &x, double &y);
      
      Servo                           *mServos[NSERVOS];
      LED                             *mEyeLED;
      LED                             *mHeadLED;
      Camera                          *mCamera;
      
      managers::DARwInOPMotionManager *mMotionManager;
      managers::DARwInOPGaitManager   *mGaitManager;
  };
};

#endif
