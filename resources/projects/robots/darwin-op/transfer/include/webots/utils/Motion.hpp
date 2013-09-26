/*******************************************************************************************************/
/* File:         Motion.hpp                                                                            */
/* Date:         September 2013                                                                        */
/* Description:  Wrapper of the Motion Webots API for the DARwIn-OP real robot                         */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2013 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef MOTION_HPP
#define MOTION_HPP

#include <string>
#include <vector>

struct Pose;

namespace webots {
  class Motor;
  
  class Motion {
  public:
                               Motion(const std::string &fileName);
    virtual                   ~Motion();
    
    bool                       isValid() const { return mValid; }
    
    virtual void               play();
    virtual void               stop() { mPlaying = false; }
    
    int                        getDuration() const { return mDuration; }
    int                        getTime() const { return mElapsed; }
    bool                       isOver() const;
    
    virtual void               setTime(int time);
    virtual void               setReverse(bool reverse) { mReverse = reverse; }
    virtual void               setLoop(bool loop) { mLoop = loop; }
    
    // not member(s) of the Webots API function: please don't use
    static void                playMotions();

  private:
    static int                 timeFromString(const std::string &time); // e.g. time = "01:03:024": return 1*60000 + 3*1000 + 24 = 63024
    void                       clearInternalStructure();
    void                       playStep();
    
    bool                       mValid;
    int                        mDuration;
    bool                       mReverse;
    bool                       mLoop;
    bool                       mPlaying;
    int                        mElapsed;
    int                        mPreviousTime;
    std::vector<Pose *>        mPoses;
    std::vector<Motor *>       mMotors;

    static std::vector<Motion *> cMotions;
  };
}

#endif //MOTION_HPP
