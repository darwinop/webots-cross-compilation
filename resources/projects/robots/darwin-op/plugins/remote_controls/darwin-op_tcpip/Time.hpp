/*
 * File:         Time.hpp
 * Date:         June 2012
 * Description:  Helper function to return real time
 * Author:       fabien.rohrer@cyberbotics.com
 * Modifications:
 */

#ifndef TIME_HPP
#define TIME_HPP

class Time {
  public:
                 Time();
    virtual     ~Time();

    int          currentSimulationTime(); // returns milliseconds

    static void  wait(int duration); // duration in milliseconds

  private:
    unsigned int currentTime();
    unsigned int mInitTime;
};

#endif
