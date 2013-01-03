//---------------------------------------------------------------------------------------
//  File:         darwin-op_team_1.cpp (to be used in a Webots controllers)
//  Description:  Instantiates the correct player type: FieldPlayer or GoalKeeper
//  Project:      Robotstadium, the online robot soccer competition
//  Author:       Yvan Bourquin - www.cyberbotics.com
//  Date:         May 11, 2009
//  Changes:      May 3, 2010: Changed how teamID and playerID are determined
//---------------------------------------------------------------------------------------

#include "FieldPlayer.hpp"
#include "GoalKeeper.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

int main(int argc, const char *argv[]) {

  if (argc < 3) {
    cout << "Error: could not find teamID and playerID in controllerArgs" << endl;
    return 0;
  }

  int playerID = atoi(argv[1]);
  int teamID = atoi(argv[2]);

  // choose GoalKepper/FieldPlayer role acording to playerID
  Player *player = NULL;
  if (playerID == 0)
    player = new GoalKeeper(playerID, teamID);
  else
    player = new FieldPlayer(playerID, teamID);

  player->run();
  delete player;
}
