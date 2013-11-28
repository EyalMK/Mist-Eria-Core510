/* # Script de Tydrheal & Sungis : Lorewalker Stonestep # */

/* Notes : What is missing ? - Pending ...
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Lorewalker Stonestep */
	SPELL_LOREWALKER_S_ALACRITY	= 122714,

	/* Corrupted Scroll */
	SPELL_FLOOR_SCROLL			= 107350,
	SPELL_CAMERA_SHAKE			= 106346,

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
	NPC_LOREWALKER_STONESTEP	= 56843,
	NPC_CORRUPTED_SCROLL		= 57080,
	NPC_STRIFE					= 59051,
	NPC_PERIL					= 59726,
	NPC_OSONG					= 56872,
	NPC_SUN						= 56915, // Missing template
};

class boss_lorewalker_stonestep : public CreatureScript
{
public:
	boss_lorewalker_stonestep() : CreatureScript("boss_lorewalker_stonestep") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_lorewalker_stonestepAI(creature);
	}

	struct boss_lorewalker_stonestepAI : public ScriptedAI
	{
		boss_lorewalker_stonestepAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		void Reset()
		{
			events.Reset();

			if (instance)
				if (Creature* scroll = me->FindNearestCreature(NPC_CORRUPTED_SCROLL, 500, true))
					me->SetInCombatWith(scroll);
		}

		void JustDied(Unit *pWho) 
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, DONE);	
		}

		void KilledUnit(Unit *pWho) 
		{

		}
		
		void EnterEvadeMode() 
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, FAIL);	
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, IN_PROGRESS);

				if (Creature* scroll = me->FindNearestCreature(NPC_CORRUPTED_SCROLL, 500, true))
					me->SetInCombatWith(scroll);
			}
		}

		void UpdateAI(uint32 diff) 
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

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
		}
	};
};

class npc_corrupted_scroll : public CreatureScript 
{
public:
	npc_corrupted_scroll() : CreatureScript("npc_corrupted_scroll") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_corrupted_scrollAI(creature);
	}

	struct npc_corrupted_scrollAI : public ScriptedAI
	{
		npc_corrupted_scrollAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		void Reset() 
		{
			me->setActive(false);
			me->CastSpell(me, SPELL_FLOOR_SCROLL);
		}

		void JustDied(Unit *pWho) 
		{
			if (instance)
			{
				instance->DoCastSpellOnPlayers(SPELL_CAMERA_SHAKE);

				me->SummonCreature(NPC_OSONG, 848.020325f, 2449.538818f, 174.961197f, 4.385465f, TEMPSUMMON_MANUAL_DESPAWN);
				//me->SummonCreature(NPC_PERIL, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
				//me->SummonCreature(NPC_STRIFE, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
			}
		}
	};
};

class npc_zao_sunseeker : public CreatureScript 
{
public:
	npc_zao_sunseeker() : CreatureScript("npc_zao_sunseeker") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_zao_sunseekerAI(creature);
	}

	struct npc_zao_sunseekerAI : public ScriptedAI
	{
		npc_zao_sunseekerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		void Reset() 
		{
			events.Reset();
		}

		void JustDied(Unit *pWho) 
		{
			
		}

		void EnterCombat(Unit* /*who*/) 
		{
			me->SetInCombatWithZone();
		}

		void EnterEvadeMode() 
		{
			
		}

		void UpdateAI(uint32 diff) 
		{	
			if(!UpdateVictim())
				return;

			events.Update(diff);

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

class npc_sun : public CreatureScript 
{
public:
	npc_sun() : CreatureScript("npc_sun") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_sunAI(creature);
	}

	struct npc_sunAI : public ScriptedAI
	{
		npc_sunAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset() 
		{
			events.Reset();
		}

		void JustDied(Unit *pWho)
		{

		}

		void EnterCombat(Unit* /*who*/) 
		{

		}

		void EnterEvadeMode() 
		{

		}

		void UpdateAI(uint32 diff) 
		{	
			if(!UpdateVictim())
				return;

			events.Update(diff);

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
					
		}
	};
};

class npc_haunting_sha : public CreatureScript 
{
public:
	npc_haunting_sha() : CreatureScript("npc_haunting_sha") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_haunting_shaAI(creature);
	}

	struct npc_haunting_shaAI : public ScriptedAI
	{
		npc_haunting_shaAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		int checkEvadeMode;

		void Reset() 
		{
			events.Reset();
		}

		void JustSummoned(Creature* creature) 
		{
			events.Reset();
		}

		void JustDied(Unit *pWho)
		{

		}

		void EnterCombat(Unit* /*who*/) 
		{

		}

		void EnterEvadeMode() 
		{

		}

		void UpdateAI(uint32 diff) 
		{	
			if(!UpdateVictim())
				return;

			events.Update(diff);

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

void AddSC_boss_lorewalker_stonestep()
{
	new npc_corrupted_scroll();
	new boss_lorewalker_stonestep();
	new npc_zao_sunseeker();
	new npc_sun();
	new npc_haunting_sha();
}