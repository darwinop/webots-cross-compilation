#include <webots/Gyro.hpp>
#include <webots/Robot.hpp>

#include <CM730.h>

using namespace webots;
using namespace Robot;

Gyro::Gyro(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  for (int i=0; i<3; i++)
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
  for (int i=0; i<3; i++)
    mValues[i] = integerValues[i];
}
