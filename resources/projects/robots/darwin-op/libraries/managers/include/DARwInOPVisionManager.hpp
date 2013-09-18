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

namespace Robot {
  class ColorFinder;
  class FrameBuffer;
}

namespace managers {
  using namespace Robot;
  class DARwInOPVisionManager {
    public:
                   DARwInOPVisionManager(int width, int height, int hue, int hueTolerance, int minSaturation, int minValue, int minPercent, int maxPercent);
      virtual     ~DARwInOPVisionManager();

      bool         getBallCenter(double &x, double &y, const unsigned char *image);
      bool         isDetected(int x, int y);
      void         setHue(int hue)                     { mfinder->m_hue = hue; }
      void         setHueTolerance(int hueTolerance)   { mfinder->m_hue_tolerance = hueTolerance; }
      void         setMinSaturation(int minSaturation) { mfinder->m_min_saturation = minSaturation; }
      void         setMinValue(int minValue)           { mfinder->m_min_value = minValue; }
      void         setMinPercent(int minPercent)       { mfinder->m_min_percent = minPercent; }
      void         setmaxPercent(int maxPercent)       { mfinder->m_max_percent = maxPercent; }

    private:
      ColorFinder *mfinder;
      FrameBuffer *mbuffer;

  };
}

#endif
