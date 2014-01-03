/* Vale of Eternal Blossoms */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"

/* Taijing the Cyclone & Jinho the Windbreaker */

enum JinhoTaijingSpells
{
    SPELL_SPINNING_CRANE_KICK	= 129003,
    SPELL_CYCLONIC_INSPIRATION	= 128943
};

enum JinhoTaijingEvents
{
	EVENT_SPINNING_CRANE_KICK	= 1
};

class npc_jinho_and_taijing : public CreatureScript
{
public:
    npc_jinho_and_taijing() : CreatureScript("npc_jinho_and_taijing") { }

    struct npc_jinho_and_taijingAI : public ScriptedAI
    {
        npc_jinho_and_taijingAI(Creature* creature) : ScriptedAI(creature)
        {	}

		EventMap events;

        void Reset()
		{
			events.Reset();
			events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 1*IN_MILLISECONDS);
		}

        void MoveInLineOfSight(Unit* who)
        {
<<<<<<< HEAD
<<<<<<< HEAD
            if (!me->IsWithinDistInMap(who->ToPlayer(), 40.0f) || who->ToPlayer()->isGameMaster() || who->ToPlayer()->HasAura(SPELL_CYCLONIC_INSPIRATION))
=======
            if (!me->IsWithinDistInMap(who, 40.0f) || (who && who->ToPlayer() && who->ToPlayer()->isGameMaster()) || who->HasAura(SPELL_CYCLONIC_INSPIRATION))
>>>>>>> b755d3e0aca79bff61baa81e67a4a4cdbb1e1aea
=======
            if (!me->IsWithinDistInMap(who, 40.0f) || who->ToPlayer()->isGameMaster() || who->HasAura(SPELL_CYCLONIC_INSPIRATION))
>>>>>>> parent of 347f6fe... Fix crash
                return;

			if (!who || !who->IsInWorld() || !me->HasAura(SPELL_SPINNING_CRANE_KICK))
                return;

			if (who && who->ToPlayer() && !me->IsValidAttackTarget(who))
                who->CastSpell(who, SPELL_CYCLONIC_INSPIRATION);
        }

        void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					case EVENT_SPINNING_CRANE_KICK:
						DoCast(me, SPELL_SPINNING_CRANE_KICK);

						events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 45*IN_MILLISECONDS);
						break;

					default:
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jinho_and_taijingAI(creature);
    }
};

void AddSC_vale_of_eternal_blossoms()
{
    new npc_jinho_and_taijing;
}
