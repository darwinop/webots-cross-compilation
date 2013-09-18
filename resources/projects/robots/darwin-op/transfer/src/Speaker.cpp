#include <webots/Speaker.hpp>
#include <webots/Robot.hpp>

#include <LinuxDARwIn.h>
#include <string.h>

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

void Speaker::playFile(const char *filename) {
  LinuxActionScript::PlayMP3(filename);
}

void Speaker::playFileWait(const char *filename) {
  LinuxActionScript::PlayMP3Wait(filename);
}

void Speaker::speak(const char *text, const char *voice, int speed) {
  char speedBuffer[20];
  sprintf(speedBuffer, "%d", speed);

  if (speak_pid != -1)
    kill(speak_pid, SIGKILL);

  speak_pid = fork();

  switch (speak_pid) {
    case -1:
      fprintf(stderr, "Fork failed!! \n");
      break;
    case 0: {
      fprintf(stderr, "Saying \"%s\" ...\n", text);
      char *buffer = (char *)malloc(strlen(text)+3);
      sprintf(buffer, "\"%s\"", text);
      execl("/usr/bin/espeak", "espeak", buffer, "-v", voice, "-s", speedBuffer, (char *)NULL);
      free(buffer);
      fprintf(stderr, "exec failed!! \n"); }
      break;
    default:
      break;
  }
}

void Speaker::speakFile(const char *filename, const char *voice, int speed) {
  char speedBuffer[20];
  sprintf(speedBuffer, "%d", speed);

  if (speak_pid != -1)
    kill(speak_pid, SIGKILL);

  speak_pid = fork();

  switch(speak_pid) {
    case -1:
      fprintf(stderr, "Fork failed!! \n");
      break;
    case 0:
      fprintf(stderr, "Saying text from file \"%s\" ...\n", filename);
      execl("/usr/bin/espeak", "espeak", "-f", filename, "-v", voice, "-s", speedBuffer, (char *)NULL);
      fprintf(stderr, "exec failed!! \n");
      break;
    default:
      break;
  }
}
