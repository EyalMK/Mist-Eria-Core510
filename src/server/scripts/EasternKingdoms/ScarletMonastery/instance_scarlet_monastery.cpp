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
    SPELL_HEAL                  = 111024
};

enum Texts
{
    SAY_CRANE       = 0
};

enum Actions
{
    ACTION_CRANE    = 1
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
                NpcTriggerCraneGUID                 = 0;
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

                    case NPC_TRIGGER_CRANE:
                        NpcTriggerCraneGUID = creature->GetGUID();

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
                        break;

                    case GO_WHITEMANE_GATE:
                        WhitemaneDoorGUID = go->GetGUID();
                        if (GetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE) == IN_PROGRESS)
                            HandleGameObject(0, true, go);
                        if (GetBossState(DATA_BOSS_COMMANDER_DURAND) == NOT_STARTED)
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
                        if(state == IN_PROGRESS)
                            HandleGameObject(WhitemaneDoorGUID, true);
                        break;
                    case DATA_BOSS_COMMANDER_DURAND:
                        if(state == NOT_STARTED)
                            HandleGameObject(WhitemaneDoorGUID, false);
                        if(state == IN_PROGRESS)
                            HandleGameObject(KorloffDoorGUID, false);
                        break;
                    default:
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

                    case DATA_NPC_TRIGGER_CRANE:
                        return NpcTriggerCraneGUID;

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
                uint64 NpcTriggerCraneGUID ;
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

class npc_traqueur_crane : public CreatureScript
{
public:
    npc_traqueur_crane() : CreatureScript("npc_traqueur_crane") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_traqueur_craneAI(creature);
    }

    struct npc_traqueur_craneAI : public ScriptedAI
    {
            npc_traqueur_craneAI(Creature* creature) : ScriptedAI(creature)
            {
                Crane = false;
            }

            bool Crane;

            void DoAction(int32 action)
            {
                switch (action)
                {
                    case ACTION_CRANE:
                        if (!Crane)
                        {
                            Talk(SAY_CRANE);
                            Crane = true;
                        }
                        break;
                }
            }
    };
};

class at_crane_monastery : public AreaTriggerScript
{
    public:
        at_crane_monastery () : AreaTriggerScript("at_crane_monastery ") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* crane = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_NPC_TRIGGER_CRANE)))
                    crane->AI()->DoAction(ACTION_CRANE);
            return true;
        }
};


void AddSC_instance_scarlet_monastery()
{
   new instance_scarlet_monastery();
   new npc_spirit_of_redemption();
   new npc_traqueur_crane();
   new at_crane_monastery();
}
