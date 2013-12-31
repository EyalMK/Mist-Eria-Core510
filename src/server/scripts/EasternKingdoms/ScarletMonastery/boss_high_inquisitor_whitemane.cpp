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
    SPELL_RAISE_FALLEN_CRUSADER     = 115139,
    SPELL_SPIRIT_GALE               = 115289,
    SPELL_EVICT_SOUL                = 115297,
    SPELL_SUMMON_EMPOWERING_SPIRITS = 115147
};


enum Events
{
    EVENT_RAISE_FALLEN_CRUSADER         = 1,
    EVENT_SPIRIT_GALE                   = 2,
    EVENT_EVICT_SOUL                    = 3,
    EVENT_SUMMON_EMPOWERING_SPIRITS     = 4
};


enum Texts
{
    SAY_AGGRO                       = 1, // Vous allez partager mon agonie sans fin !
    SAY_DEATH                       = 2, // Je vois le bout du tunnel enfin…
    SAY_RAISE_FALLEN_CRUSADER       = 3, // Pas de repos... pour les morts en colère !
    SAY_EVICT_SOUL                  = 4, // Cherchez un réceptacle… et revenez !
    SAY_SUMMON_EMPOWERING_SPIRITS   = 5  // Revendiquez un corps et exercez votre terrible vengeance !
};


class boss_high_inquisitor_whitemane : public CreatureScript
{
public:
    boss_high_inquisitor_whitemane() : CreatureScript("boss_high_inquisitor_whitemane") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_inquisitor_whitemaneAI(creature);
    }

    struct boss_high_inquisitor_whitemaneAI : public ScriptedAI
    {
        boss_high_inquisitor_whitemaneAI(Creature* creature) : ScriptedAI(creature), Summons(me)
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
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, NOT_STARTED);
        }


        void EnterCombat(Unit* /*who*/)
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, IN_PROGRESS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, FAIL);
        }


        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);

            if (instance)
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, DONE);
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
                        case EVENT_RAISE_FALLEN_CRUSADER:
                            break;

                        case EVENT_SPIRIT_GALE:
                            break;

                        case EVENT_EVICT_SOUL:
                            break;

                        case EVENT_SUMMON_EMPOWERING_SPIRITS:
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

void AddSC_boss_high_inquisitor_whitemane()
{
    new boss_high_inquisitor_whitemane();
}
