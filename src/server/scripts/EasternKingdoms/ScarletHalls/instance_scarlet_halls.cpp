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
#include "ScriptedCreature.h"
#include "scarlet_halls.h"

class instance_scarlet_halls : public InstanceMapScript
{
    public:
        instance_scarlet_halls() : InstanceMapScript("instance_scarlet_halls", 1001) { }

        struct instance_scarlet_halls_InstanceMapScript : public InstanceScript
        {
            instance_scarlet_halls_InstanceMapScript(Map* map) : InstanceScript(map){}

            void Initialize()
            {
                SetBossNumber(EncounterCount);
                BossHoundmasterBraunGUID        = 0;
                BossArmsmasterHarlanGUID        = 0;
                BossFlameweaverKoeglerGUID      = 0;

                BraunDoorGUID                   = 0;
                HarlanDoorGUID                  = 0;
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_HOUNDMASTER_BRAUN:
                        BossHoundmasterBraunGUID = creature->GetGUID();
                        break;

                    case NPC_ARMSMASTER_HARLAN:
                        BossArmsmasterHarlanGUID = creature->GetGUID();
                        break;

                    case NPC_FLAMEWEAVER_KOEGLER:
                        BossFlameweaverKoeglerGUID	= creature->GetGUID();
                        break;

                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_BRAUN_GATE:
                        BraunDoorGUID = go->GetGUID();
                        break;

                    case GO_HARLAN_GATE:
                        HarlanDoorGUID = go->GetGUID();
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_BOSS_HOUNDMASTER_BRAUN:
                        break;
                    case DATA_BOSS_ARMSMASTER_HARLAN:
                        break;
                    case DATA_BOSS_FLAMEWEAVER_KOEGLER:
                        break;
                }
                return true;
            }

            uint64 GetData64(uint32 id) const
            {
                switch (id)
                {
                    case DATA_BOSS_HOUNDMASTER_BRAUN:
                        return BossHoundmasterBraunGUID;
                        break;

                    case DATA_BOSS_ARMSMASTER_HARLAN:
                        return BossArmsmasterHarlanGUID;
                        break;

                    case DATA_BOSS_FLAMEWEAVER_KOEGLER:
                        return BossFlameweaverKoeglerGUID;
                        break;
                }
                return 0;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S M " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* strIn)
            {
                if (!strIn)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(strIn);

                char dataHead1, dataHead2;

                std::istringstream loadStream(strIn);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'S' && dataHead2 == 'H')
                {
                    for (uint8 i = 0; i < EncounterCount; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                            SetBossState(i, EncounterState(tmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

            protected:
                uint64 BossHoundmasterBraunGUID;
                uint64 BossArmsmasterHarlanGUID;
                uint64 BossFlameweaverKoeglerGUID;

                uint64 BraunDoorGUID;
                uint64 HarlanDoorGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_scarlet_halls_InstanceMapScript(map);
        }
};

void AddSC_instance_scarlet_halls()
{
    new instance_scarlet_halls();
}
