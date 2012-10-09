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
	
  CM730 *cm730 = getRobot()->getCM730();
  int actualState = 0;

  //RGB Led
  if(getName() == "EyeLed" || getName() == "HeadLed")
  {
    // convert RGB-24bits to RGB-15bits
    int r5 = ((value >> 16) & 0xFF) >> 3;
    int g5 = ((value >>  8) & 0xFF) >> 3;
    int b5 = ( value        & 0xFF) >> 3;
    int rgb15 = (b5 << 10) | (g5 << 5) | r5;
  
    // set the led value
    cm730->WriteWord(mNamesToIDs[getName()], rgb15, 0);
  }
  // BackPannel Led
  else if(getName() == "BackLedRed")
  {
	if(value == 1)   	// Switch selected led on without changing the two others
	{
		cm730->ReadWord(CM730::ID_CM, CM730::P_LED_PANNEL, &actualState, 0);
		cm730->WriteByte(CM730::P_LED_PANNEL, 0x01|actualState, NULL);
	}
	else 			// Switch selected led off without changing the two others
	{
		cm730->ReadWord(CM730::ID_CM, CM730::P_LED_PANNEL, &actualState, 0);
		cm730->WriteByte(CM730::P_LED_PANNEL, 0x06&actualState, NULL);
	}
  }
  else if(getName() == "BackLedBlue")
  {
	if(value == 1)   	// Switch selected led on without changing the two others
	{
		cm730->ReadWord(CM730::ID_CM, CM730::P_LED_PANNEL, &actualState, 0);
		cm730->WriteByte(CM730::P_LED_PANNEL, 0x02|actualState, NULL);
	}
	else 			// Switch selected led off without changing the two others
	{
		cm730->ReadWord(CM730::ID_CM, CM730::P_LED_PANNEL, &actualState, 0);
		cm730->WriteByte(CM730::P_LED_PANNEL, 0x05&actualState, NULL);
	}
  }
  else if(getName() == "BackLedGreen")
  {
	if(value == 1)   	// Switch selected led on without changing the two others
	{
		cm730->ReadWord(CM730::ID_CM, CM730::P_LED_PANNEL, &actualState, 0);
		cm730->WriteByte(CM730::P_LED_PANNEL, 0x04|actualState, NULL);
	}
	else 			// Switch selected led off without changing the two others
	{
		cm730->ReadWord(CM730::ID_CM, CM730::P_LED_PANNEL, &actualState, 0);
		cm730->WriteByte(CM730::P_LED_PANNEL, 0x03&actualState, NULL);
	}
  }
}

void LED::initStaticMap() {
  static bool firstCall=true;
  if (firstCall) {
    firstCall = false;

    mNamesToIDs["EyeLed"]  = CM730::P_LED_EYE_L;
    mNamesToIDs["HeadLed"] = CM730::P_LED_HEAD_L;
  }
}
