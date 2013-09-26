// File:          MotionPlayer.hpp
// Date:          19th of September 2013
// Description:   Sample showing how to use a Webots motion on the real DARwIn-OP
// Author:        fabien.rohrer@cyberbotics.com

#ifndef MOTION_PLAYER_HPP
#define MOTION_PLAYER_HPP

#include <webots/Robot.hpp>

class MotionPlayer : public webots::Robot {
  public:
             MotionPlayer();
    virtual ~MotionPlayer();
    void     run();
    
  private:
    int      mTimeStep;
    
    void     myStep();
    void     wait(int ms);
};

#endif
