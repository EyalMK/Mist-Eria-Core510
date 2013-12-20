/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __BATTLEGROUNDTA_H
#define __BATTLEGROUNDTA_H

#include "Battleground.h"

enum BattlegroundTAObjectTypes
{
    BG_TA_OBJECT_DOOR_1         = 0,
    BG_TA_OBJECT_DOOR_2         = 1,
    BG_TA_OBJECT_BUFF_1         = 2,
    BG_TA_OBJECT_BUFF_2         = 3,
    BG_TA_OBJECT_MAX            = 4
};

enum BattlegroundTAObjects
{
    BG_TA_OBJECT_TYPE_DOOR_1    = 0,
    BG_TA_OBJECT_TYPE_DOOR_2    = 0,
    BG_TA_OBJECT_TYPE_BUFF_1    = 0,
    BG_TA_OBJECT_TYPE_BUFF_2    = 0
};

class BattlegroundTA : public Battleground
{
    public:
        BattlegroundTA();
        ~BattlegroundTA();

        /* inherited from BattlegroundClass */
        void AddPlayer(Player* player);
        void Reset();
        void FillInitialWorldStates(WorldPacket &d);
        void StartingEventCloseDoors();
        void StartingEventOpenDoors();

        void RemovePlayer(Player* player, uint64 guid, uint32 team);
        void HandleAreaTrigger(Player* Source, uint32 Trigger);
        bool SetupBattleground();
        void HandleKillPlayer(Player* player, Player* killer);
        bool HandlePlayerUnderMap(Player* player);
};
#endif
