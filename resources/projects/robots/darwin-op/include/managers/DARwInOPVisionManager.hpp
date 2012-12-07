// File:          DARwInOPVisionManager.hpp
// Date:          7th of December 2012
// Description:   Facade between webots and the darwin-op framework
//                allowing to used the main image processing tools
// Author:        david.mansolino@epfl.ch

#include <ImgProcess.h>
#include <ColorFinder.h>
#include <Image.h>
#include <Point.h>

#ifndef DARWINOP_VISION_MANAGER_HPP
#define DARWINOP_VISION_MANAGER_HPP


//namespace webots {
  //class Robot;
  //class Servo;
//}

namespace Robot {
  class ColorFinder;
  class FrameBuffer;
}

namespace managers {
  using namespace Robot;
  class DARwInOPVisionManager {
    public:
                       DARwInOPVisionManager(int width, int height, int hue, int hueTolerance, int minSaturation, int minValue, int minPercent, int maxPercent);
      virtual         ~DARwInOPVisionManager();
      bool             getBallCenter(double &x, double &y, const unsigned char * image);

    private:
      ColorFinder                     *mfinder;
      FrameBuffer                     *mbuffer;

  };
}

#endif
