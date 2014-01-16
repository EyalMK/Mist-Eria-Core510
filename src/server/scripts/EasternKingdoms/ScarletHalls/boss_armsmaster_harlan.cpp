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
#include "ScriptedCreature.h"
#include "scarlet_halls.h"
#include "ScriptedEscortAI.h"

enum Spells
{
    /* Harlan */
    SPELL_HEROIC_LEAP           = 111218,
    SPELL_DRAGONS_REACH         = 111217,
    SPELL_BLADES_OF_LIGHT       = 111216,
    SPELL_BERSERKER_RAGE        = 111221,
    //SPELL_CALL_REINFORCEMENTS   = 111755,

    /* Autres */
    SPELL_HEAVY_ARMOR           = 113959,
    SPELL_UNARMORED_1           = 113969,
    SPELL_UNARMORED_2           = 113970
};

enum Events
{
    EVENT_HEROIC_LEAP           = 1,
    EVENT_DRAGONS_REACH         = 2,
    EVENT_BLADES_OF_LIGHT       = 3,
    EVENT_BERSERKER_RAGE        = 4,
    EVENT_CALL_REINFORCEMENTS   = 5,
    EVENT_JUMP_LEAP             = 6,
    EVENT_WAYPOINT_BLADES       = 7
};

enum Texts
{
    SAY_AGGRO               = 0,
    SAY_DEATH               = 1,
    SAY_KILL                = 2
};


enum Creatures
{
    NPC_SCARLET_DEFENDER    = 58998
};


class boss_armsmaster_harlan : public CreatureScript
{
public:
    boss_armsmaster_harlan () : CreatureScript("boss_armsmaster_harlan") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_armsmaster_harlanAI(creature);
    }

    struct boss_armsmaster_harlanAI : public ScriptedAI
    {
        boss_armsmaster_harlanAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        bool berserk;
        bool waypoint;
        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();
            berserk = true;
            waypoint = false;

            if (instance)
                instance->SetBossState(DATA_BOSS_ARMSMASTER_HARLAN, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetBossState(DATA_BOSS_ARMSMASTER_HARLAN, IN_PROGRESS);

            events.ScheduleEvent(EVENT_DRAGONS_REACH, 10*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_CALL_REINFORCEMENTS, 15*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_JUMP_LEAP, 40*IN_MILLISECONDS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_ARMSMASTER_HARLAN, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void KilledUnit(Unit* /*pWho*/)
        {
            Talk(SAY_KILL);
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);
            Summons.DespawnAll();

            if (instance)
                instance->SetBossState(DATA_BOSS_ARMSMASTER_HARLAN, DONE);

        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

            if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 500.0f, true))
                if(target && target->GetTypeId() == TYPEID_PLAYER)
                    Summoned->AI()->AttackStart(target);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            switch (id)
            {
                case EVENT_JUMP:
                    events.ScheduleEvent(EVENT_HEROIC_LEAP, 100);
                    break;
            }

            if (type == POINT_MOTION_TYPE)
                switch (id)
                {
                    case 1:
                        me->GetMotionMaster()->MovePoint(2, 1199.28f, 455.25f, 1.2f);
                        break;
                    case 2:
                        me->GetMotionMaster()->MovePoint(3, 1212.00f, 456.04f, 3.00f);
                        break;
                    case 3:
                        me->GetMotionMaster()->MovePoint(4, 1220.20f, 446.20f, 6.20f);
                        break;
                    case 4:
                        me->GetMotionMaster()->MovePoint(5, 1224.79f, 436.81f, 6.20f);
                        break;
                    case 5:
                        me->GetMotionMaster()->MovePoint(6, 1214.36f, 426.06f, 6.20f);
                        break;
                    case 6:
                        me->GetMotionMaster()->MovePoint(7, 1198.93f, 425.60f, 7.90f);
                        break;
                    case 7:
                        me->GetMotionMaster()->MovePoint(8, 1187.82f, 436.83f, 10.90f);
                        break;
                    case 8:
                        me->GetMotionMaster()->MovePoint(9, 1188.49f, 451.41f, 10.90f);
                        break;
                    case 9:
                        me->GetMotionMaster()->MovePoint(10, 1197.87f, 462.36f, 8.10f);
                        break;
                    case 10:
                        me->GetMotionMaster()->MovePoint(11, 1212.04f, 462.46f, 6.20f);
                        break;
                    case 11:
                        me->GetMotionMaster()->MovePoint(12, 1224.22f, 453.38f, 6.20f);
                        break;
                    case 12:
                        me->GetMotionMaster()->MovePoint(13, 1217.59f, 437.31f, 4.50f);
                        break;
                    case 13:
                        me->GetMotionMaster()->MovePoint(14, 1208.86f, 430.74f, 2.10f);
                        break;
                    case 14:
                        me->GetMotionMaster()->MovePoint(15, 1193.94f, 444.15f, 1.2f);
                        break;
                    case 15:
                        me->RemoveAurasDueToSpell(SPELL_BLADES_OF_LIGHT);
                        events.ScheduleEvent(EVENT_DRAGONS_REACH, 10*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_CALL_REINFORCEMENTS, 15*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_JUMP_LEAP, 40*IN_MILLISECONDS);
                        break;
                }
        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HealthBelowPct(50) && berserk)
            {
                events.ScheduleEvent(EVENT_BERSERKER_RAGE, 1*IN_MILLISECONDS);
                berserk = false;
            }


            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    if (instance)
                    {
                        case EVENT_JUMP_LEAP:
                            me->GetMotionMaster()->MoveJump(1206.44f, 444.01f, 1.1f, 10, 10, EVENT_JUMP);
                            events.CancelEvent(EVENT_DRAGONS_REACH);
                            events.CancelEvent(EVENT_CALL_REINFORCEMENTS);
                            break;

                        case EVENT_HEROIC_LEAP:
                            DoCast(SPELL_HEROIC_LEAP);
                            events.ScheduleEvent(EVENT_BLADES_OF_LIGHT, 100);
                            break;

                        case EVENT_DRAGONS_REACH:
                            DoCast(SPELL_DRAGONS_REACH);
                            events.ScheduleEvent(EVENT_DRAGONS_REACH, 16*IN_MILLISECONDS);
                            break;

                        case EVENT_BLADES_OF_LIGHT:
                            DoCast(SPELL_BLADES_OF_LIGHT);
                            events.ScheduleEvent(EVENT_WAYPOINT_BLADES, 6*IN_MILLISECONDS);
                            break;

                        case EVENT_WAYPOINT_BLADES:
                            me->GetMotionMaster()->MovePoint(1, 1193.94f, 444.15f, 1.2f);
                            break;

                        case EVENT_BERSERKER_RAGE:
                            DoCast(SPELL_BERSERKER_RAGE);
                            events.CancelEvent(EVENT_BERSERKER_RAGE);
                            break;

                        case EVENT_CALL_REINFORCEMENTS:
                            me->SummonCreature(NPC_SCARLET_DEFENDER, 1187.17f, 443.87f, 11.3f, 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
                            me->SummonCreature(NPC_SCARLET_DEFENDER, 1187.17f, 443.87f, 11.3f, 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
                            events.ScheduleEvent(EVENT_CALL_REINFORCEMENTS, 20*IN_MILLISECONDS);
                            break;

                        default:
                            break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

};

class npc_scarlet_defender : public CreatureScript
{
public:
    npc_scarlet_defender() : CreatureScript("npc_scarlet_defender") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_defenderAI(creature);
    }

    struct npc_scarlet_defenderAI : public ScriptedAI
    {
            npc_scarlet_defenderAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
            }

            void EnterCombat(Unit* /*who*/)
            {
            }

            void UpdateAI(uint32 diff)
            {
                if(!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
    };
};

void AddSC_boss_armsmaster_harlan()
{
    new boss_armsmaster_harlan();
    new npc_scarlet_defender();
}
