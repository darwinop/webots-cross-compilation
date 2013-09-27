// File:          remote.hpp
// Date:          January 2013
// Description:   Remote serveur for DARwIn-OP remote-control
// Author:        david.mansolino@epfl.ch

#ifndef REMOTE_HPP
#define REMOTE_HPP

#define NMOTORS 20

#include <webots/Robot.hpp>

namespace webots {
  class Motor;
  class LED;
  class Camera;
  class Accelerometer;
  class Gyro;
  
  class Remote : public Robot {
    public:
                           Remote();
      virtual             ~Remote();

      void                 remoteStep();

      const double        *getRemoteAccelerometer() const;
      const double        *getRemoteGyro() const;
      const unsigned char *getRemoteImage() const;
      double               getRemoteMotorPosition(int index);
      double               getRemoteMotorTorque(int index);
      double               getRemoteTime() const;

      void                 setRemoteLED(int index, int value);
      void                 setRemoteMotorPosition(int index, int value);
      void                 setRemoteMotorVelocity(int index, int value);
      void                 setRemoteMotorAcceleration(int index, int value);
      void                 setRemoteMotorAvailableTorque(int index, int value);
      void                 setRemoteMotorTorque(int index, int value);
      void                 setRemoteMotorControlPID(int index, int p, int i, int d);

    private:
      void                 wait(int ms);
      void                 myStep();

      int                  mTimeStep;

      Motor               *mMotors[NMOTORS];
      LED                 *mEyeLed;
      LED                 *mHeadLed;
      LED                 *mBackLedRed;
      LED                 *mBackLedGreen;
      LED                 *mBackLedBlue;
      Camera              *mCamera;
      Accelerometer       *mAccelerometer;
      Gyro                *mGyro;
  };
};

#endif
