#include <webots/Accelerometer.hpp>
#include <webots/Robot.hpp>

#include <CM730.h>

using namespace webots;
using namespace Robot;

static double values[3];

Accelerometer::Accelerometer(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
}

Accelerometer::~Accelerometer() {
}

void Accelerometer::enable(int ms) {
}

void Accelerometer::disable() {
}

const double *Accelerometer::getValues() const {
  CM730 *cm730 = getRobot()->getCM730();
  
  int integerValues[3];
  cm730->ReadWord(CM730::P_ACCEL_X_L, &integerValues[0], 0);
  cm730->ReadWord(CM730::P_ACCEL_Y_L, &integerValues[1], 0);
  cm730->ReadWord(CM730::P_ACCEL_Z_L, &integerValues[2], 0);
  
  for (int i=0; i<3; i++)
    values[i] = integerValues[i];
  
  return values;
}
