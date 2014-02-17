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
    /* Koegler */
    SPELL_BOOK_BURNER               = 113364,
    SPELL_QUICKENED_MIND            = 113682,
    SPELL_PYROBLAST                 = 113690,
    SPELL_GREATER_DRAGONS_BREATH    = 113641,
    SPELL_TELEPORT                  = 113626,
    SPELL_FIREBALL_VOLLEY           = 113691,

    /* Autres */
    SPELL_BURNING_BOOKS             = 113616
};

enum Events
{
    EVENT_BOOK_BURNER               = 1,
    EVENT_QUICKENED_MIND            = 2,
    EVENT_PYROBLAST                 = 3,
    EVENT_GREATER_DRAGONS_BREATH    = 4,
    EVENT_TELEPORT                  = 5,
    EVENT_FIREBALL_VOLLEY           = 6
};

enum Texts
{
    SAY_AGGRO                       = 0,
    SAY_DEATH                       = 1,
    SAY_KILL                        = 2,
    SAY_INTRO                       = 3,
    SAY_DRAGONS_BREATH              = 4,
    SAY_BOOK_BURNER                 = 5
};

enum Actions
{
    ACTION_INTRO    = 1
};

class at_flameweaver_koegler_intro : public AreaTriggerScript
{
    public:
        at_flameweaver_koegler_intro() : AreaTriggerScript("at_flameweaver_koegler_intro") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* koegler = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_BOSS_FLAMEWEAVER_KOEGLER)))
                    koegler->AI()->DoAction(ACTION_INTRO);
            return true;
        }
};

class boss_flameweaver_koegler : public CreatureScript
{
public:
    boss_flameweaver_koegler() : CreatureScript("boss_flameweaver_koegler") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_flameweaver_koeglerAI(creature);
    }

    struct boss_flameweaver_koeglerAI : public ScriptedAI
    {
        boss_flameweaver_koeglerAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            Intro = false;
        }

        bool Intro;
        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();

            if (instance)
                instance->SetBossState(DATA_BOSS_FLAMEWEAVER_KOEGLER, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetBossState(DATA_BOSS_FLAMEWEAVER_KOEGLER, IN_PROGRESS);

            events.ScheduleEvent(EVENT_PYROBLAST, 4*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_QUICKENED_MIND, 8*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_FIREBALL_VOLLEY, 15*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BOOK_BURNER, 25*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_TELEPORT, 30*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_GREATER_DRAGONS_BREATH, 31*IN_MILLISECONDS);
        }

        void DoAction(const int32 action)
        {
            switch (action)
            {
                case ACTION_INTRO:
                    if (!Intro)
                    {
                        Talk(SAY_INTRO);
                        Intro = true;
                    }
                    break;
            }
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_FLAMEWEAVER_KOEGLER, FAIL);

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
                instance->SetBossState(DATA_BOSS_FLAMEWEAVER_KOEGLER, DONE);

        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    if (instance)
                    {
                        case EVENT_PYROBLAST:
                            DoCast(SPELL_PYROBLAST);
                            events.ScheduleEvent(EVENT_PYROBLAST, 40*IN_MILLISECONDS);
                            break;
                        case EVENT_QUICKENED_MIND:
                            DoCast(me, SPELL_QUICKENED_MIND);
                            events.ScheduleEvent(EVENT_QUICKENED_MIND, 40*IN_MILLISECONDS);
                            break;
                        case EVENT_FIREBALL_VOLLEY:
                            DoCast(SPELL_FIREBALL_VOLLEY);
                            events.ScheduleEvent(EVENT_FIREBALL_VOLLEY, 40*IN_MILLISECONDS);
                            break;
                        case EVENT_BOOK_BURNER:
                            DoCast(SPELL_BOOK_BURNER);
                            events.ScheduleEvent(EVENT_BOOK_BURNER, 40*IN_MILLISECONDS);
                            break;
                        case EVENT_TELEPORT:
                            DoCast(me, SPELL_TELEPORT);
                            events.ScheduleEvent(EVENT_TELEPORT, 40*IN_MILLISECONDS);
                            break;
                        case EVENT_GREATER_DRAGONS_BREATH:
                            DoCast(SPELL_GREATER_DRAGONS_BREATH);
                            events.ScheduleEvent(EVENT_GREATER_DRAGONS_BREATH, 40*IN_MILLISECONDS);
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

class npc_book_case : public CreatureScript
{
public:
    npc_book_case() : CreatureScript("npc_book_case") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_book_caseAI(creature);
    }

    struct npc_book_caseAI : public ScriptedAI
    {
            npc_book_caseAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
            }

            void EnterCombat(Unit* /*who*/)
            {
            }

            void UpdateAI(const uint32 diff)
            {
                if(!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
    };
};

void AddSC_boss_flameweaver_koegler()
{
    new boss_flameweaver_koegler();
    new npc_book_case();
}
