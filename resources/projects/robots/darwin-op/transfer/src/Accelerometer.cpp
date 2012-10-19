#include <webots/Accelerometer.hpp>
#include <webots/Robot.hpp>

#include <CM730.h>

using namespace webots;
using namespace Robot;

Accelerometer::Accelerometer(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  for (int i=0; i<3; i++)
    mValues[i] = 512;
}

Accelerometer::~Accelerometer() {
}

void Accelerometer::enable(int ms) {
}

void Accelerometer::disable() {
}

const double *Accelerometer::getValues() const {
  return mValues;
}

void Accelerometer::setValues(const int *integerValues) {
  for (int i=0; i<3; i++)
    mValues[i] = integerValues[i];
}
