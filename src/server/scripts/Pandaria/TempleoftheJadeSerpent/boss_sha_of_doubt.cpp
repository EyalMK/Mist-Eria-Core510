/* # Script de Tydrheal & Sungis : Sha of Doubt # */

/* Notes : What is missing ? - Pending ...
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Sha Of Doubt */
};

enum Events
{

};

enum Texts
{

};

enum Phases
{

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

		void Reset()
		{		
			events.Reset();

			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, NOT_STARTED);
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
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, FAIL);	
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, IN_PROGRESS);

			me->SetInCombatWithZone();
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

void AddSC_boss_sha_of_doubt()
{
	new boss_sha_of_doubt();
}