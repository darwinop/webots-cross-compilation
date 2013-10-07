/*******************************************************************************************************/
/* File:         Camera.hpp                                                                            */
/* Date:         Sep 11                                                                                */
/* Description:  Wrapper of the Camera Webots API for the DARwIn-OP real robot                         */
/* Author:       fabien.rohrer@cyberbotics.com                                                         */
/* Copyright (c) 2011 Cyberbotics - www.cyberbotics.com                                                */
/*******************************************************************************************************/

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <webots/Device.hpp>
#include <pthread.h>

namespace webots {
  class Camera: public Device  {
    public:
      enum {
        WB_CAMERA_COLOR = 99
      };

                              Camera(const std::string &name); //Use Robot::getCamera() instead
      virtual                ~Camera();

      virtual void            enable(int ms);
      virtual void            disable();

      const unsigned char    *getImage() const;
      int                     getWidth() const;
      int                     getHeight() const;
      double                  getFov() const;
      int                     getType() const;
      double                  getNear() const;
      int                     getSamplingPeriod() const;

      static unsigned char    imageGetRed(const unsigned char *image, int width, int x,int y);
      static unsigned char    imageGetGreen(const unsigned char *image, int width, int x,int y);
      static unsigned char    imageGetBlue(const unsigned char *image, int width, int x,int y);
      static unsigned char    imageGetGrey(const unsigned char *image, int width, int x,int y);
      static bool             checkResolution(int width, int height);
    
    protected:
      static void            *CameraTimerProc(void *param);// thread function
    
    private:
      static const int        NBRESOLUTION = 6;
      static const int        mResolution[NBRESOLUTION][2];
      static unsigned char   *mImage;

      pthread_t               mCameraThread;// thread structure
      bool                    mIsActive;
  };
}

#endif // CAMERA_HPP
