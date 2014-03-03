#include "Farm.h"
#include "Player.h"

Farm::Farm()
{
}

Farm::Farm(Player* p) {
    player = p;
}


void Farm::CheckZone(uint32 newzone) {
    if (!isPlayerInsideZone && newzone == 6039) {
        oldPhase = player->GetPhaseMask();
        player->SetPhaseMask(0, true);
        isPlayerInsideZone = true;
    }else if (isPlayerInsideZone & newzone != 6039) {
        player->SetPhaseMask(oldPhase, true);
        isPlayerInsideZone = false;
    }
}


bool Farm::canSeeOrDetect(WorldObject *obj) {
    if (obj->GetGUID() == MAKE_NEW_GUID(69, 0, HIGHGUID_PLAYER))
        return true;
    return true;
}
