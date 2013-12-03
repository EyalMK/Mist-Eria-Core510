/* # Script de Tydrheal & Sungis : Sha of Doubt # */

/* Notes : What is missing ? - Pending ...
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Sha Of Doubt */
	SPELL_WITHER_WILL			= 106736,
	SPELL_BOUNDS_OF_REALITY		= 117665,
	SPELL_TOUCH_OF_NOTHINGNESS	= 106113
};

enum Events
{
	EVENT_WITHER_WILL			= 1,
	EVENT_TOUCH_OF_NOTHINGNESS	= 2,
	EVENT_BOUNDS_OF_REALITY		= 3
};

enum Texts
{

};

enum Phases
{
	PHASE_NULL				= 0,
	PHASE_COMBAT			= 1,
	PHASE_BOUNDS_OF_REALITY	= 2
};

enum Npcs
{

};

class boss_sha_of_doubt : public CreatureScript
{
public:
	boss_sha_of_doubt() : CreatureScript("boss_sha_of_doubt") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new boss_sha_of_doubtAI(creature);
	}

	struct boss_sha_of_doubtAI : public ScriptedAI
	{
		boss_sha_of_doubtAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		bool seventyFivePct;

		void Reset()
		{		
			events.Reset();

			if (instance)
			{
				events.SetPhase(PHASE_NULL);
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, NOT_STARTED);
				seventyFivePct = false;
			}
		}
		
		void JustDied(Unit *pWho)
		{
			if (instance)
					instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, DONE);
		}

		void KilledUnit(Unit *pWho)
		{

		}
		
		void EnterEvadeMode()
		{
			if (instance)
			{
				events.SetPhase(PHASE_NULL);
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, FAIL);
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, IN_PROGRESS);

			me->SetInCombatWithZone();

			events.SetPhase(PHASE_COMBAT);
			events.ScheduleEvent(EVENT_WITHER_WILL, 2*IN_MILLISECONDS, 0, PHASE_COMBAT);
			events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 8*IN_MILLISECONDS, 0, PHASE_COMBAT);
		}
		
		void UpdateAI(uint32 diff)
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (HealthBelowPct(75) && !seventyFivePct)
			{
				events.SetPhase(PHASE_BOUNDS_OF_REALITY);
				events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, 0, 0, PHASE_BOUNDS_OF_REALITY);
				seventyFivePct = true;
			}


			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_WITHER_WILL:
							me->CastSpell(me, SPELL_WITHER_WILL);

							events.ScheduleEvent(EVENT_WITHER_WILL, 7*IN_MILLISECONDS);
							break;

						case EVENT_TOUCH_OF_NOTHINGNESS:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
								if (target && target->GetTypeId() == TYPEID_PLAYER)
									me->CastSpell(target, SPELL_TOUCH_OF_NOTHINGNESS);

							events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 18*IN_MILLISECONDS);
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

void AddSC_boss_sha_of_doubt()
{
	new boss_sha_of_doubt();
}