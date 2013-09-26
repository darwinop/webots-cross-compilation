// File:          Symmetry.hpp
// Date:          29th of October 2012
// Description:   Sample showing how to use the motors and to cross-compile
//                for the darwin-op
// Author:        david.mansolino@epfl.ch

#ifndef SYMMETRY_HPP
#define SYMMETRY_HPP

#define NMOTORS 20

#include <webots/Robot.hpp>

namespace webots {
  class Motor;
  class LED;
};

class Symmetry : public webots::Robot {
  public:
                  Symmetry();
    virtual      ~Symmetry();
    void          run();
    
  private:
    int           mTimeStep;
    
    void          myStep();
    void          wait(int ms);
    
    webots::Motor *mMotors[NMOTORS];
    webots::LED   *mEyeLED;
    webots::LED   *mHeadLED;
};

#endif
