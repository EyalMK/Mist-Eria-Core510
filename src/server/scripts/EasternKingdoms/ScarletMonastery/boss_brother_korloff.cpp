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
    SPELL_BLAZING_FISTS     = 114807
};


enum Events
{
    EVENT_FIRESTORM_KICK    = 1,
    EVENT_RISING_FLAME      = 2,
    EVENT_BLAZING_FISTS     = 3
};


enum Texts
{
    SAY_AGGRO                       = 1, // Je vais vous briser…
    SAY_DEATH                       = 2, // Vous êtes… des pièces de métal…
    SAY_FIRESTORM_KICK              = 3, // Quand on est mort, on est mort.
    SAY_BLAZING_FISTS               = 4 // Mes poings sont rouge sang… votre sang !
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

        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();

            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, IN_PROGRESS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, FAIL);
        }


        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);

            if (instance)
                instance->SetBossState(DATA_BOSS_BROTHER_KORLOFF, DONE);
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
                        case EVENT_FIRESTORM_KICK:
                            break;

                        case EVENT_RISING_FLAME:
                            break;

                        case EVENT_BLAZING_FISTS:
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
