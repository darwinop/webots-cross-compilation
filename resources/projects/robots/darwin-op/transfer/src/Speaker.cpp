#include <webots/Speaker.hpp>
#include <webots/Robot.hpp>

#include <LinuxDARwIn.h>

using namespace webots;
using namespace Robot;

Speaker::Speaker(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
}

Speaker::~Speaker() {
}

void Speaker::enable(int ms) {
}

void Speaker::disable() {
}

void Speaker::playFile(const char* filename) {
  LinuxActionScript::PlayMP3(filename);
}

void Speaker::playFileWait(const char* filename) {
  LinuxActionScript::PlayMP3Wait(filename);
}
