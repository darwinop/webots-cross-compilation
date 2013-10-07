#include <webots/Gyro.hpp>
#include <webots/Robot.hpp>

using namespace webots;

Gyro::Gyro(const std::string &name) :
  Device(name)
{
  for (int i = 0; i < 3; i++)
    mValues[i] = 512;  // 512 = central value -> no rotation
}

Gyro::~Gyro() {
}

void Gyro::enable(int ms) {
}

void Gyro::disable() {
}

const double *Gyro::getValues() const {
  return mValues;
}

void Gyro::setValues(const int *integerValues) {
  for (int i = 0; i < 3; i++)
    mValues[i] = integerValues[i];
}

int Gyro::getSamplingPeriod() const {
  return Robot::getInstance()->getBasicTimeStep();
}
