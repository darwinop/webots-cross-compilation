#include <webots/Gyro.hpp>
#include <webots/Robot.hpp>

#include <CM730.h>

using namespace webots;
using namespace Robot;

static double values[3];

Gyro::Gyro(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
}

Gyro::~Gyro() {
}

void Gyro::enable(int ms) {
}

void Gyro::disable() {
}

const double *Gyro::getValues() const {
  CM730 *cm730 = getRobot()->getCM730();
  
  int integerValues[3];
  cm730->ReadWord(CM730::P_GYRO_X_L, &integerValues[0], 0);
  cm730->ReadWord(CM730::P_GYRO_Y_L, &integerValues[1], 0);
  cm730->ReadWord(CM730::P_GYRO_Z_L, &integerValues[2], 0);
  
  for (int i=0; i<3; i++)
    values[i] = integerValues[i];
  
  return values;
}
