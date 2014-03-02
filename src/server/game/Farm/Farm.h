#ifndef FARM_H
#define FARM_H

#include "Player.h"

class Player;

class Farm
{
public:
    Farm();
    Farm(Player*);

    void CheckZone(uint32 newzone);
    bool canSeeOrDetect(WorldObject* obj);

    Player* player;
    uint32 oldPhase;
    bool isPlayerInsideZone;
};

#endif // FARM_H
