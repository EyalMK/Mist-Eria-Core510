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
#include "scarlet_monastery.h"


enum Spells
{
    SPELL_FIRESTORM_KICK    = 113764,
    SPELL_RISING_FLAME      = 114410,
    SPELL_BLAZING_FISTS     = 114807,
    SPELL_SCORCHED_EARTH    = 114460
};


enum Events
{
    EVENT_FIRESTORM_KICK    = 1,
    EVENT_BLAZING_FISTS     = 2,
    EVENT_SCORCHED_EARTH    = 3,
    EVENT_JUMP_FIRESTORM    = 4
};


enum Texts
{
    SAY_AGGRO                       = 0,
    SAY_DEATH                       = 1,
    SAY_KILL                        = 2
};


class boss_brother_korloff : public CreatureScript
{
public:
    boss_brother_korloff () : CreatureScript("boss_brother_korloff") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_brother_korloffAI(creature);
    }

    struct boss_brother_korloffAI : public ScriptedAI
    {
        boss_brother_korloffAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        bool scorchedearth;
        uint32 m_uiNextCastPercent;
        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();
            scorchedearth = true;
            m_uiNextCastPercent = 90 ;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, IN_PROGRESS);

            events.ScheduleEvent(EVENT_FIRESTORM_KICK, 10*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BLAZING_FISTS, 20*IN_MILLISECONDS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, FAIL);

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
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, DONE);
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

            Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if(target && target->GetTypeId() == TYPEID_PLAYER)
                    Summoned->AI()->AttackStart(target);
        }


        void DamageTaken(Unit* doneBy, uint32 &amount)
        {
            if(me->GetHealthPct() <= m_uiNextCastPercent)
            {
                DoCast(SPELL_RISING_FLAME);
                m_uiNextCastPercent -= 10 ;
            }
        }


        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HealthBelowPct(50) && scorchedearth)
            {
                events.ScheduleEvent(EVENT_SCORCHED_EARTH, 1*IN_MILLISECONDS);
                scorchedearth = false;
            }

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    if (instance)
                    {
                        case EVENT_JUMP_FIRESTORM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                me->GetMotionMaster()->MoveJump(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 20, 20, EVENT_JUMP);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                events.ScheduleEvent(EVENT_FIRESTORM_KICK, 1*IN_MILLISECONDS);
                            }
                            events.ScheduleEvent(EVENT_JUMP_FIRESTORM, 30*IN_MILLISECONDS);
                            break;

                        case EVENT_FIRESTORM_KICK:
                            DoCast(SPELL_FIRESTORM_KICK);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            break;

                        case EVENT_SCORCHED_EARTH:
                            DoCast(SPELL_SCORCHED_EARTH);
                            break;

                        case EVENT_BLAZING_FISTS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                DoCast(target, SPELL_BLAZING_FISTS);
                            }
                            events.ScheduleEvent(EVENT_BLAZING_FISTS, 30*IN_MILLISECONDS);
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

void AddSC_boss_brother_korloff()
{
    new boss_brother_korloff();
}
