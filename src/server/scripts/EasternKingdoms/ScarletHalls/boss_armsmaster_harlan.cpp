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
    SPELL_FIRESTORM_KICK        = 113764

};

enum Events
{
    EVENT_FIRESTORM_KICK    = 1

};

enum Texts
{
    SAY_AGGRO                       = 0,
    SAY_DEATH                       = 1,
    SAY_KILL                        = 2
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

        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();

            if (instance)
                instance->SetBossState(DATA_BOSS_ARMSMASTER_HARLAN, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetBossState(DATA_BOSS_ARMSMASTER_HARLAN, IN_PROGRESS);
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

        }

        void UpdateAI(uint32 diff)
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

                        default:
                            break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_armsmaster_harlan()
{
    new boss_armsmaster_harlan();
}
