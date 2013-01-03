#ifndef FIELD_PLAYER_HPP
#define FIELD_PLAYER_HPP

//-----------------------------------------------------------------------------
//  File:         FieldPlayer C++ class (to be used in a Webots controllers)
//  Description:  Field player (not a goalkeeper !)
//  Project:      Robotstadium, the online robot soccer competition
//  Author:       Yvan Bourquin - www.cyberbotics.com
//  Date:         May 8, 2009
//  Changes:      Sep 27, 2011: Updated for the DARwIn-OP (Fabien Rohrer)
//-----------------------------------------------------------------------------

#include "Player.hpp"

class FieldPlayer : public Player {
public:
  FieldPlayer(int playerID, int teamID);
  virtual ~FieldPlayer();

  // overridden function
  virtual void run();

private:
  bool getBallCenter(double &x, double &y);
};

#endif
