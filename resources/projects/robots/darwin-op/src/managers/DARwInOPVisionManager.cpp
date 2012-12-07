#include <managers/DARwInOPVisionManager.hpp>

//#include <webots/Robot.hpp>


//#include <iostream>
//#include <cmath>

using namespace Robot;
using namespace managers;
//using namespace webots;
using namespace std;


DARwInOPVisionManager::DARwInOPVisionManager(int width, int height, int hue, int hueTolerance, int minSaturation, int minValue, int minPercent, int maxPercent) {
  mfinder = new ColorFinder(hue, hueTolerance, minSaturation, minSaturation, minPercent, maxPercent);
  mbuffer = new FrameBuffer(width, height);
}

DARwInOPVisionManager::~DARwInOPVisionManager() {
  delete mfinder;
  delete mbuffer;
}

bool DARwInOPVisionManager::getBallCenter(double &x, double &y, const unsigned char * image)
{
  
  Point2D pos;
  
  // Put the image in mbuffer
  mbuffer->m_BGRAFrame->m_ImageData = (unsigned char *)image;
  // Convert the image from BGRA format to HSV format
  ImgProcess::BGRAtoHSV(mbuffer);
  // Extract position of the ball from HSV verson of the image
  pos = mfinder->GetPosition(mbuffer->m_HSVFrame);
  
  if (pos.X==-1 && pos.Y==-1) {
    x = 0.0;
    y = 0.0;
    return false;
  } else {
    x = pos.X; //2.0 * pos.X / mCamera->getWidth()  - 1.0;
    y = pos.Y; //2.0 * pos.Y / mCamera->getHeight() - 1.0;
    return true;
  }
}
