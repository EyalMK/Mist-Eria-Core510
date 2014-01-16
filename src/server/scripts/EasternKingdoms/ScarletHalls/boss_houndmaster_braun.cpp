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

enum Spells
{
    SPELL_DEATH_BLOSSOM_JUMP    = 114241,
    SPELL_DEATH_BLOSSOM         = 114242,
    SPELL_PIERCING_THROW        = 114004,
  //  SPELL_BLOODY_MESS           = 114056,
    SPELL_CALL_DOG              = 114259,
    SPELL_BLOODY_RAGE           = 116140
};

enum Events
{
    EVENT_DEATH_BLOSSOM_JUMP    = 1,
    EVENT_DEATH_BLOSSOM         = 2,
    EVENT_PIERCING_THROW        = 3,
    EVENT_BLOODY_RAGE           = 4,
    EVENT_DEATH                 = 5
};

enum Texts
{
    SAY_AGGRO                       = 0,
    SAY_DEATH                       = 1,
    SAY_KILL                        = 2,
    SAY_CALL_DOG                    = 3,
    SAY_PIERCING                    = 4,
    SAY_BLOSSOM                     = 5,
    SAY_FAIL_DOG                    = 6,
    SAY_PRE_DEATH_1                 = 7,
    SAY_PRE_DEATH_2                 = 8
};


class boss_houndmaster_braun: public CreatureScript
{
public:
    boss_houndmaster_braun() : CreatureScript("boss_houndmaster_braun") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_houndmaster_braunAI(creature);
    }

    struct boss_houndmaster_braunAI : public ScriptedAI
    {
        boss_houndmaster_braunAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        bool bloodyrage;
        bool predeath;
        uint32 m_uiNextCastPercent;
        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();
            bloodyrage = true;
            predeath = true;
            m_uiNextCastPercent = 90;

            if (instance)
                instance->SetBossState(DATA_BOSS_HOUNDMASTER_BRAUN, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetBossState(DATA_BOSS_HOUNDMASTER_BRAUN, IN_PROGRESS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_HOUNDMASTER_BRAUN, FAIL);

            ScriptedAI::EnterEvadeMode();

            events.ScheduleEvent(EVENT_DEATH_BLOSSOM_JUMP, 17*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_PIERCING_THROW, 7*IN_MILLISECONDS);
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
                instance->SetBossState(DATA_BOSS_HOUNDMASTER_BRAUN, DONE);

        }

        void DamageTaken(Unit* doneBy, uint32 &damage)
        {
            if(me->GetHealthPct() <= m_uiNextCastPercent && me->GetHealthPct() >= 55)
            {
                DoCast(SPELL_CALL_DOG);
                m_uiNextCastPercent -= 10 ;
            }

            if (damage < me->GetHealth())
                return;

            if (!instance)
                return;

            if (damage >= me->GetHealth())
            {
                damage = 0;

                events.CancelEvent(EVENT_PIERCING_THROW);
                events.CancelEvent(EVENT_DEATH_BLOSSOM_JUMP);

                me->InterruptNonMeleeSpells(false);
                me->StopMoving();
                me->ClearComboPointHolders();
                me->RemoveAllAurasOnDeath();
                me->ClearAllReactives();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                Talk(SAY_PRE_DEATH_2);
                events.ScheduleEvent(EVENT_DEATH, 10*IN_MILLISECONDS);
            }
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (me->HealthBelowPct(50) && bloodyrage)
            {
                events.ScheduleEvent(EVENT_BLOODY_RAGE, 1*IN_MILLISECONDS);
                bloodyrage = false;
            }

            if (me->HealthBelowPct(20) && predeath)
            {
                Talk(SAY_PRE_DEATH_1);
                predeath = false;
            }

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    if (instance)
                    {
                        case EVENT_DEATH_BLOSSOM_JUMP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                DoCast(target, SPELL_DEATH_BLOSSOM_JUMP);
                            }
                            events.ScheduleEvent(EVENT_DEATH_BLOSSOM_JUMP, 18*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_DEATH_BLOSSOM, 1*IN_MILLISECONDS);
                            break;

                        case EVENT_DEATH_BLOSSOM:
                            DoCast(SPELL_DEATH_BLOSSOM);
                            break;

                        case EVENT_PIERCING_THROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                DoCast(target, SPELL_PIERCING_THROW);
                            }
                            events.ScheduleEvent(EVENT_PIERCING_THROW, 7*IN_MILLISECONDS);
                            break;

                        case EVENT_BLOODY_RAGE:
                            DoCast(me, SPELL_BLOODY_RAGE);
                            break;

                        case EVENT_DEATH:
                            me->DisappearAndDie();
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

void AddSC_boss_houndmaster_braun()
{
    new boss_houndmaster_braun();
}
