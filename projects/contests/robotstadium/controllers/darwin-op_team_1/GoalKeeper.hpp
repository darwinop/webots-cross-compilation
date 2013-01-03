#ifndef GOAL_KEEPER_HPP
#define GOAL_KEEPER_HPP

//-----------------------------------------------------------------------------
//  File:         GoalKeeper class (to be used in a Webots controllers)
//  Description:  Goal keeper (not a field player !)
//  Project:      Robotstadium, the online robot soccer competition
//  Author:       Yvan Bourquin - www.cyberbotics.com
//  Date:         May 8, 2009
//  Changes:      Sep 27, 2011: Updated for the DARwIn-OP (Fabien Rohrer)
//-----------------------------------------------------------------------------

#include "Player.hpp"

class GoalKeeper : public Player {
public:
  GoalKeeper(int playerID, int teamID);
  virtual ~GoalKeeper();

  // overridden function
  virtual void run();
};

#endif
