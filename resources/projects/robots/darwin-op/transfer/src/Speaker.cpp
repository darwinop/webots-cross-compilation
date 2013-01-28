#include <webots/Speaker.hpp>
#include <webots/Robot.hpp>

#include <LinuxDARwIn.h>

using namespace webots;
using namespace Robot;

Speaker::Speaker(const std::string &name, const Robot *robot) :
  Device(name, robot)
{
  speak_pid = -1;
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

void Speaker::speak(const char * text) {
  if(speak_pid != -1)
    kill(speak_pid, SIGKILL);

  speak_pid = fork();

  switch(speak_pid) {
  case -1:
    fprintf(stderr, "Fork failed!! \n");
    break;
  case 0:
    fprintf(stderr, "Saying \"%s\" ...\n", text);
    char buffer[sizeof(text) + 10];
    sprintf(buffer, "\"%s\"", text);
    execl("/usr/bin/espeak", "espeak", buffer, (char *)NULL);
    fprintf(stderr, "exec failed!! \n");
    break;
  default:
    break;
  }
}

void Speaker::speakFile(const char * filename) {
  if(speak_pid != -1)
    kill(speak_pid, SIGKILL);

  speak_pid = fork();

  switch(speak_pid) {
  case -1:
    fprintf(stderr, "Fork failed!! \n");
    break;
  case 0:
    fprintf(stderr, "Saying text from file \"%s\" ...\n", filename);
    execl("/usr/bin/espeak", "espeak", "-f", filename, (char *)NULL);
    fprintf(stderr, "exec failed!! \n");
    break;
  default:
    break;
  }
}
