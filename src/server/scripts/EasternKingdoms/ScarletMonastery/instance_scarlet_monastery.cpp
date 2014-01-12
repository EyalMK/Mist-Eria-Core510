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
#include "scarlet_monastery.h"
#include "Player.h"

enum Spells
{
    SPELL_HEAL                  = 111024,
    SPELL_MASS_RESURRECTION     = 113134
};


class instance_scarlet_monastery : public InstanceMapScript
{
    public:
        instance_scarlet_monastery() : InstanceMapScript("instance_scarlet_monastery", 1004) { }

        struct instance_scarlet_monastery_InstanceMapScript : public InstanceScript
        {
            instance_scarlet_monastery_InstanceMapScript(Map* map) : InstanceScript(map){}

            void Initialize()
            {
                SetBossNumber(EncounterCount);
                BossThalnosTheSoulrenderGUID        = 0;
                BossBrotherKorloffGUID              = 0;
                BossHighInquisitorWhitemaneGUID		= 0;
                BossCommanderDurandGUID             = 0;

                ThalnosDoorGUID                     = 0;
                KorloffDoorGUID                     = 0;
                WhitemaneDoorGUID                   = 0;
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_THALNOS_THE_SOULRENDER:
                        BossThalnosTheSoulrenderGUID = creature->GetGUID();
                        break;

                    case NPC_BROTHER_KORLOFF:
                        BossBrotherKorloffGUID = creature->GetGUID();
                        break;

                    case NPC_HIGH_INQUISITOR_WHITEMANE:
                        BossHighInquisitorWhitemaneGUID	 = creature->GetGUID();
                        break;

                    case NPC_COMMANDER_DURAND:
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
                    case GO_THALNOS_GATE:
                        ThalnosDoorGUID = go->GetGUID();
                        if (GetBossState(DATA_BOSS_THALNOS_THE_SOULRENDER) == DONE)
                            HandleGameObject(0, true, go);
                        break;

                    case GO_KORLOFF_GATE:
                        KorloffDoorGUID = go->GetGUID();
                        if (GetBossState(DATA_BOSS_BROTHER_KORLOFF) == DONE)
                            HandleGameObject(0, true, go);
                        if (GetBossState(DATA_BOSS_COMMANDER_DURAND) == IN_PROGRESS)
                            HandleGameObject(0, false, go);
                        if (GetBossState(DATA_BOSS_COMMANDER_DURAND) == NOT_STARTED && GetBossState(DATA_BOSS_BROTHER_KORLOFF) == DONE)
                            HandleGameObject(0, true, go);
                        if (GetBossState(DATA_BOSS_COMMANDER_DURAND) == DONE)
                            HandleGameObject(0, true, go);
                        break;

                    case GO_WHITEMANE_GATE:
                        WhitemaneDoorGUID = go->GetGUID();
                        if (GetBossState(DATA_BOSS_COMMANDER_DURAND) == SPECIAL)
                            HandleGameObject(0, true, go);
                        if (GetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE) == NOT_STARTED)
                            HandleGameObject(0, false, go);
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_BOSS_THALNOS_THE_SOULRENDER:
                        if(state == DONE)
                            HandleGameObject(ThalnosDoorGUID, true);
                        break;
                    case DATA_BOSS_BROTHER_KORLOFF:
                        if(state == DONE)
                            HandleGameObject(KorloffDoorGUID, true);
                        break;
                    case DATA_BOSS_HIGH_INQUISITOR_WHITEMANE:
                        if(state == NOT_STARTED)
                            HandleGameObject(WhitemaneDoorGUID, false);
                        break;
                    case DATA_BOSS_COMMANDER_DURAND:
                        if(state == NOT_STARTED && GetBossState(DATA_BOSS_BROTHER_KORLOFF) == DONE)
                            HandleGameObject(KorloffDoorGUID, true);
                        if(state == SPECIAL)
                            HandleGameObject(WhitemaneDoorGUID, true);
                        if(state == IN_PROGRESS)
                            HandleGameObject(KorloffDoorGUID, false);
                        if(state == DONE)
                            HandleGameObject(KorloffDoorGUID, true);
                        break;
                }
                return true;
            }

            uint64 GetData64(uint32 id) const
            {
                switch (id)
                {
                    case DATA_BOSS_THALNOS_THE_SOULRENDER:
                        return BossThalnosTheSoulrenderGUID;
                        break;

                    case DATA_BOSS_BROTHER_KORLOFF:
                        return BossBrotherKorloffGUID;
                        break;

                    case DATA_BOSS_HIGH_INQUISITOR_WHITEMANE:
                        return BossHighInquisitorWhitemaneGUID;
                        break;

                    case DATA_BOSS_COMMANDER_DURAND:
                        return BossCommanderDurandGUID;
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

                if (dataHead1 == 'S' && dataHead2 == 'M')
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
                uint64 BossThalnosTheSoulrenderGUID;
                uint64 BossBrotherKorloffGUID;
                uint64 BossHighInquisitorWhitemaneGUID;
                uint64 BossCommanderDurandGUID;

                uint64 ThalnosDoorGUID;
                uint64 KorloffDoorGUID;
                uint64 WhitemaneDoorGUID;

        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_scarlet_monastery_InstanceMapScript(map);
        }
};


class npc_spirit_of_redemption : public CreatureScript
{
public:
    npc_spirit_of_redemption() : CreatureScript("npc_spirit_of_redemption") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spirit_of_redemptionAI(creature);
    }

    struct npc_spirit_of_redemptionAI : public ScriptedAI
    {
            npc_spirit_of_redemptionAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 m_uiCheckTimer;
            uint32 m_uiDespawnTimer;

            void Reset()
            {
                m_uiCheckTimer = 1000;
                m_uiDespawnTimer = 20000;
            }

            void UpdateAI(uint32 diff)
            {
                if(m_uiCheckTimer <= diff)
                {
                    if(Unit* target = DoSelectLowestHpFriendly(30, 1000))
                    {
                        me->CastSpell(target, SPELL_HEAL);
                        m_uiCheckTimer = 6000;
                        return;
                    }
                    m_uiCheckTimer = 1000;
                }
                else m_uiCheckTimer -= diff;

                if(m_uiDespawnTimer <= diff)
                {
                    me->DespawnOrUnsummon();
                }
                else m_uiDespawnTimer -= diff;
            }
    };
};


class npc_scarlet_judicator : public CreatureScript
{
public:
    npc_scarlet_judicator() : CreatureScript("npc_scarlet_judicator") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_judicatorAI(creature);
    }

    struct npc_scarlet_judicatorAI : public ScriptedAI
    {
            npc_scarlet_judicatorAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->SetStandState(UNIT_STAND_STATE_DEAD);
            }

            void SpellHit(Unit* /*who*/, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_MASS_RESURRECTION)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->SetReactState(REACT_AGGRESSIVE);

                    if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 1000.0f, true))
                        if(target && target->GetTypeId() == TYPEID_PLAYER)
                            me->GetMotionMaster()->MoveChase(target);
                }
            }

            void UpdateAI(uint32 diff)
            {
                DoMeleeAttackIfReady();
            }

    };
};

void AddSC_instance_scarlet_monastery()
{
    new instance_scarlet_monastery();
    new npc_spirit_of_redemption();
    new npc_scarlet_judicator();
}
