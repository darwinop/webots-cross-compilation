/*
 * File:          entry_points.hpp
 * Date:          June 2012
 * Description:   Defines the entry point of the remote control library
 * Author:        fabien.rohrer@cyberbotics.com
 * Modifications: January 2013 adapted to DARwIn-OP robot by david.mansolino@epfl.ch
 */

#ifndef LIB_ROBOT_WINDOW_HPP
#define LIB_ROBOT_WINDOW_HPP

#include <webots/remote_control.h>

extern "C" {
  bool wbr_init(WbrInterface *ri);
  void wbr_cleanup();
}

#endif
