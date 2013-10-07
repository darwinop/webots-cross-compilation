/*******************************************************************************************************/
/* File:         Speaker.hpp                                                                           */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Speaker Webots API for the DARwIn-OP real robot                        */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include <webots/Device.hpp>

#include <unistd.h>
#include <sys/wait.h>

namespace webots {
  class Speaker: public Device  {
    public:
                    Speaker(const std::string &name); //Use Robot::getSpeaker() instead
      virtual      ~Speaker();

      virtual void  enable(int ms);
      virtual void  disable();
      virtual void  playFile(const char *filename);
      virtual void  playFileWait(const char *filename);
      virtual void  speak(const char *text, const char *voice = "en", int speed = 175);
      virtual void  speakFile(const char *filename, const char *voice = "en", int speed = 175);

    private:
      pid_t mSpeakPID;
  };
}

#endif // SPEAKER_HPP
