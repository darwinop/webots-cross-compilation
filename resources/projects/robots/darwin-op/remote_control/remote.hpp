// File:          remote.hpp
// Date:          January 2013
// Description:   Remote serveur for DARwIn-OP remote-control
// Author:        david.mansolino@epfl.ch

#ifndef REMOTE_HPP
#define REMOTE_HPP

#define NSERVOS 20

#include <webots/Robot.hpp>

namespace webots {
  class Servo;
  class LED;
  class Camera;
  class Accelerometer;
  class Gyro;
  
  class Remote : public Robot {
    public:
                                       Remote();
      virtual                         ~Remote();
      void                             remoteStep();
      const double *                   getRemoteAccelerometer() const;
      const double *                   getRemoteGyro() const;
      const unsigned char *            getRemoteImage() const;
      void                             setRemoteLED(int index, int value); 
      void                             setRemoteServoPosition(int index, int value);
      void                             setRemoteServoVelocity(int index, int value);
      void                             setRemoteServoAcceleration(int index, int value);
      void                             setRemoteServoMotorForce(int index, int value);
      void                             setRemoteServoForce(int index, int value);
      void                             setRemoteServoControlP(int index, int value);
      double                           getRemoteServoPosition(int index);
      double                           getRemoteServoForce(int index);
      double                           getRemoteTime() const;

    private:
      int                              mTimeStep;

      void                             wait(int ms);
      void                             myStep();
      
      Servo                           *mServos[NSERVOS];
      LED                             *mEyeLed;
      LED                             *mHeadLed;
      LED                             *mBackLedRed;
      LED                             *mBackLedGreen;
      LED                             *mBackLedBlue;
      Camera                          *mCamera;
      Accelerometer                   *mAccelerometer;
      Gyro                            *mGyro;
  };
};

#endif
