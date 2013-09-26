// File:          main.cpp
// Date:          January 2013
// Description:   Manage the entree point function
// Author:        david.mansolino@epfl.ch

#include "Walk.hpp"

#include <cstdlib>

using namespace webots;

int main(int argc, char **argv)
{
  Walk *controller = new Walk();
  controller->run();
  delete controller;
  return EXIT_FAILURE;
}

