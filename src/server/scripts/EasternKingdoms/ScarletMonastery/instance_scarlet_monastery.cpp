/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "scarlet_monastery.h"


DoorData const doorData[] =
{
    {GO_THALNOS_THE_SOULRENDER_GATE,        DATA_BOSS_THALNOS_THE_SOULRENDER,   DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_BROTHER_KORLOFF_GATE,               DATA_BOSS_BROTHER_KORLOFF,          DOOR_TYPE_PASSAGE,      BOUNDARY_NONE},
    {GO_BROTHER_KORLOFF_GATE,               DATA_BOSS_COMMANDER_DURAND,         DOOR_TYPE_ROOM,         BOUNDARY_N},
    {0,                                     0,                                  DOOR_TYPE_ROOM,         BOUNDARY_NONE}, // END
};

class instance_scarlet_monastery : public InstanceMapScript
{
    public:
        instance_scarlet_monastery() : InstanceMapScript("instance_scarlet_monastery", 1004) { }

        struct instance_scarlet_monastery_InstanceMapScript : public InstanceScript
        {
            instance_scarlet_monastery_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                BossThalnosTheSoulrenderGUID        = 0;
                BossBrotherKorloffGUID              = 0;
                BossHighInquisitorWhitemaneGUID		= 0;
                BossCommanderDurandGUID             = 0;
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case BOSS_THALNOS_THE_SOULRENDER:
                        BossThalnosTheSoulrenderGUID = creature->GetGUID();
                        break;

                    case BOSS_BROTHER_KORLOFF:
                        BossBrotherKorloffGUID = creature->GetGUID();
                        break;

                    case BOSS_HIGH_INQUISITOR_WHITEMANE:
                        BossHighInquisitorWhitemaneGUID	 = creature->GetGUID();
                        break;

                    case BOSS_COMMANDER_DURAND:
                        BossCommanderDurandGUID	 = creature->GetGUID();
                        break;

                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_THALNOS_THE_SOULRENDER_GATE:
                    case GO_BROTHER_KORLOFF_GATE:
                    case GO_HIGH_INQUISITOR_WHITEMANE_GATE:
                        AddDoor(go, true);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_THALNOS_THE_SOULRENDER_GATE:
                    case GO_BROTHER_KORLOFF_GATE:
                    case GO_HIGH_INQUISITOR_WHITEMANE_GATE:
                        AddDoor(go, false);
                        break;

                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 id) const
            {
                switch (id)
                {
                    case DATA_BOSS_THALNOS_THE_SOULRENDER:
                        return BossThalnosTheSoulrenderGUID;

                    case DATA_BOSS_BROTHER_KORLOFF:
                        return BossBrotherKorloffGUID;

                    case DATA_BOSS_HIGH_INQUISITOR_WHITEMANE:
                        return BossHighInquisitorWhitemaneGUID;

                    case DATA_BOSS_COMMANDER_DURAND:
                        return BossCommanderDurandGUID;

                    default:
                        break;
                }

                return 0;
            }

            protected:
                uint64 BossThalnosTheSoulrenderGUID;
                uint64 BossBrotherKorloffGUID;
                uint64 BossHighInquisitorWhitemaneGUID;
                uint64 BossCommanderDurandGUID;

        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_scarlet_monastery_InstanceMapScript(map);
        }
};

void AddSC_instance_scarlet_monastery()
{
   new instance_scarlet_monastery();
}
