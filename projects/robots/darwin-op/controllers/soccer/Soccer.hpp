// File:          Soccer.hpp
// Date:          20th of September 2011
// Description:   Simple soccer player showing how to use the middleware between webots and
//                the darwin-op framework
// Author:        fabien.rohrer@cyberbotics.com

#ifndef SOCCER_HPP
#define SOCCER_HPP

#define NSERVOS 20

#include <webots/Robot.hpp>

namespace managers {
  class DARwInOPMotionManager;
  class DARwInOPGaitManager;
  class DARwInOPVisionManager;
}

namespace webots {
  class Servo;
  class LED;
  class Camera;
  class Accelerometer;
  class Gyro;
  class Speaker;
  
  class Soccer : public Robot {
    public:
                                       Soccer();
      virtual                         ~Soccer();
      void                             run();
      
    private:
      int                              mTimeStep;
      
      void                             myStep();
      void                             wait(int ms);
      bool                             getBallCenter(double &x, double &y);
      
      Servo                           *mServos[NSERVOS];
      LED                             *mEyeLED;
      LED                             *mHeadLED;
      LED                             *mBackLedRed;
      LED                             *mBackLedGreen;
      LED                             *mBackLedBlue;
      Camera                          *mCamera;
      Accelerometer                   *mAccelerometer;
      Gyro                            *mGyro;
      
      managers::DARwInOPMotionManager *mMotionManager;
      managers::DARwInOPGaitManager   *mGaitManager;
      managers::DARwInOPVisionManager *mVisionManager;
  };
};

#endif
