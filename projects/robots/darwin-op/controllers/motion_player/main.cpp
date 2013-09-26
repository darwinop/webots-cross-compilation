// File:          main.cpp
// Date:          1st of June 2011
// Description:   Manage the entree point function
// Author:        fabien.rohrer@cyberbotics.com

#include "MotionPlayer.hpp"

#include <cstdlib>

using namespace webots;

int main(int argc, char **argv)
{
  MotionPlayer *controller = new MotionPlayer();
  controller->run();
  delete controller;
  return EXIT_FAILURE;
}

