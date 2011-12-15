#include <webots/LED.hpp>
#include <webots/Robot.hpp>

#include <CM730.h>

using namespace webots;
using namespace Robot;

std::map<const std::string, int> LED::mNamesToIDs;

#define LED_BOUND(x,a,b) (((x)<(a))?(a):((x)>(b))?(b):(x))

LED::LED(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  initStaticMap();
}

LED::~LED() {
}

void LED::set(int value) {
  // convert RGB-24bits to RGB-15bits
  int r5 = ((value >> 16) & 0xFF) >> 3;
  int g5 = ((value >>  8) & 0xFF) >> 3;
  int b5 = ( value        & 0xFF) >> 3;
  int rgb15 = (r5 << 10) | (g5 << 5) | b5;
  
  // set the led value
  CM730 *cm730 = getRobot()->getCM730();
  cm730->WriteWord(mNamesToIDs[getName()], rgb15, 0);
}

void LED::initStaticMap() {
  static bool firstCall=true;
  if (firstCall) {
    firstCall = false;

    mNamesToIDs["EyeLed"]  = CM730::P_LED_EYE_L;
    mNamesToIDs["HeadLed"] = CM730::P_LED_HEAD_L;
  }
}
