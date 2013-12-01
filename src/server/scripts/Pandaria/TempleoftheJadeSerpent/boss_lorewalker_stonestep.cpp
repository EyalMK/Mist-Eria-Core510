/* # Script de Tydrheal & Sungis : Lorewalker Stonestep # */

/* Notes : What is missing ? - Pending ...
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Lorewalker Stonestep */
	SPELL_LOREWALKER_S_ALACRITY	= 122714,
	SPELL_SPINNING_CRANE_KICK	= 129003,

	/* Corrupted Scroll */
	SPELL_FLOOR_SCROLL			= 107350,
	SPELL_CAMERA_SHAKE			= 106346,

	/* Strife & Peril */
	SPELL_AGONY					= 114571,
	SPELL_INTENSITY				= 113315,
	SPELL_DISSIPATION			= 113379,
	SPELL_ULTIMATE_POWER		= 113309,

};

enum Events
{
	/* Lorewalker Stonestep */
	EVENT_SPINNING_CRANE_KICK	= 1,

	/* Scroll */
	EVENT_SUMMON_BOSSES			= 1,

	/* Strife & Peril */
	EVENT_START_ATTACK			= 1,
	EVENT_AGONY					= 2,
	
	/* Osong */
	EVENT_AGGRO					= 1,
	EVENT_ATTACK_PERIL			= 2,
	EVENT_ATTACK_STRIFE			= 3
};

enum Texts
{

};

enum Phases
{
	PHASE_NULL			= 0,
	PHASE_ATTACK_SCROLL	= 1,
	PHASE_BOSSES		= 2,
};

enum Npcs
{
	NPC_LOREWALKER_STONESTEP	= 56843,
	NPC_CORRUPTED_SCROLL		= 57080,
	NPC_STRIFE					= 59051,
	NPC_PERIL					= 59726,
	NPC_OSONG					= 56872,
  //NPC_SUN						= 56915, Missing template
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
		bool emote;
		bool scrollAlive;
		
		void Reset()
		{
			emote = false;
			events.Reset();
			events.SetPhase(PHASE_ATTACK_SCROLL);
			events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 0, 0, PHASE_ATTACK_SCROLL);
		}

		void EnterEvadeMode() 
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, FAIL);
				emote = false;
				events.Reset();
			}
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, IN_PROGRESS);
		}

		void UpdateAI(uint32 diff) 
		{
			if (!emote)
			{
				me->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
				emote = true;
			}

			events.Update(diff);

			if (Creature* scroll = me->FindNearestCreature(NPC_CORRUPTED_SCROLL, 500.0f))
				if (!scroll->isAlive())
					events.SetPhase(PHASE_BOSSES);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_SPINNING_CRANE_KICK:
							me->CastSpell(me, SPELL_SPINNING_CRANE_KICK);

							events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 16*IN_MILLISECONDS, 0, PHASE_ATTACK_SCROLL);
							break;

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
		EventMap events;
		bool oneHp;

		void Reset() 
		{
			oneHp = false;
			me->setActive(false);
			me->CastSpell(me, SPELL_FLOOR_SCROLL);
		}

		void DamageTaken(Unit* who, uint32& damage)
		{
			if (damage >= me->GetHealth() && !oneHp)
			{
				damage = 0;
				me->SetHealth(1);
				instance->DoCastSpellOnPlayers(SPELL_CAMERA_SHAKE);
				events.ScheduleEvent(EVENT_SUMMON_BOSSES, 4*IN_MILLISECONDS);
				oneHp = true;
			}
		}

		void UpdateAI(uint32 diff) 
		{
			events.Update(diff);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_SUMMON_BOSSES:
							me->SummonCreature(NPC_OSONG, 848.020325f, -2449.538818f, 174.961197f, 4.385465f, TEMPSUMMON_MANUAL_DESPAWN);
							me->SummonCreature(NPC_PERIL, 835.478394f, -2466.505859f, 174.961578f, 0.935758f, TEMPSUMMON_MANUAL_DESPAWN);
							me->SummonCreature(NPC_STRIFE, 848.223511f, -2470.850586f, 174.961578f, 1.537897f, TEMPSUMMON_MANUAL_DESPAWN);
							
							if (Creature* lorewalker = me->FindNearestCreature(NPC_LOREWALKER_STONESTEP, 500.0f))
							{
								lorewalker->GetMotionMaster()->MovePoint(0, 824.674438f, -2453.281738f, 176.302979f);
								lorewalker->SetFacingTo(5.957958f);
							}

							me->DespawnOrUnsummon();
							events.CancelEvent(EVENT_SUMMON_BOSSES);
							break;

						default:
							break;
					}
				}
			}
		}
	};
};

class npc_strife : public CreatureScript 
{
public:
	npc_strife() : CreatureScript("npc_strife") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_strifeAI(creature);
	}

	struct npc_strifeAI : public ScriptedAI
	{
		npc_strifeAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset()
		{
			events.Reset();
			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			events.ScheduleEvent(EVENT_START_ATTACK, 5*IN_MILLISECONDS);
		}

		void JustSummoned(Creature* summoned)
        {
			events.Reset();
			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			events.ScheduleEvent(EVENT_START_ATTACK, 5*IN_MILLISECONDS);
        }

		void JustDied(Unit *pWho)
		{
			if (instance)
				if (Creature* peril = me->FindNearestCreature(NPC_STRIFE, 500.0f))
					if (!peril->isAlive())
					{
						instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, DONE);
						instance->DoCastSpellOnPlayers(SPELL_LOREWALKER_S_ALACRITY);
					}
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				if (Creature* peril = me->FindNearestCreature(NPC_PERIL, 500.0f))
					if (!peril->isInCombat())
						peril->SetInCombatWithZone();

			me->setActive(true);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetInCombatWithZone();
			events.ScheduleEvent(EVENT_AGONY, 0);
		}

		void EnterEvadeMode()
		{
			me->DespawnOrUnsummon();
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_START_ATTACK:
							EnterCombat(me);

							events.CancelEvent(EVENT_START_ATTACK);
							break;

						case EVENT_AGONY:
							me->CastSpell(me->getVictim(), SPELL_AGONY);

							events.ScheduleEvent(EVENT_AGONY, 2*IN_MILLISECONDS);
						default:
							break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}
	};
};

class npc_peril : public CreatureScript 
{
public:
	npc_peril() : CreatureScript("npc_peril") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_perilAI(creature);
	}

	struct npc_perilAI : public ScriptedAI
	{
		npc_perilAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset()
		{
			events.Reset();
			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			events.ScheduleEvent(EVENT_START_ATTACK, 5*IN_MILLISECONDS);
		}

		void JustSummoned(Creature* summoned)
        {
			events.Reset();
			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			events.ScheduleEvent(EVENT_START_ATTACK, 5*IN_MILLISECONDS);
        }

		void JustDied(Unit *pWho)
		{
			if (instance)
				if (Creature* strife = me->FindNearestCreature(NPC_STRIFE, 500.0f))
					if (!strife->isAlive())
					{
						instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, DONE);
						instance->DoCastSpellOnPlayers(SPELL_LOREWALKER_S_ALACRITY);
					}
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				if (Creature* strife = me->FindNearestCreature(NPC_STRIFE, 500.0f))
					if (!strife->isInCombat())
						strife->SetInCombatWithZone();

			me->setActive(true);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetInCombatWithZone();
			events.ScheduleEvent(EVENT_AGONY, 0);
		}

		void EnterEvadeMode()
		{
			me->DespawnOrUnsummon();
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_START_ATTACK:
							EnterCombat(me);

							events.CancelEvent(EVENT_START_ATTACK);
							break;

						case EVENT_AGONY:
							me->CastSpell(me->getVictim(), SPELL_AGONY);

							events.ScheduleEvent(EVENT_AGONY, 2*IN_MILLISECONDS);
						default:
							break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}
	};
};

class npc_osong : public CreatureScript 
{
public:
	npc_osong() : CreatureScript("npc_osong") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_osongAI(creature);
	}

	struct npc_osongAI : public ScriptedAI
	{
		npc_osongAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset()
		{
			events.Reset();
			events.ScheduleEvent(EVENT_AGGRO, 5*IN_MILLISECONDS);
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
				if (Creature* peril = me->FindNearestCreature(NPC_PERIL, 500.0f))
					if (!peril->isInCombat())
						peril->SetInCombatWithZone();

			//Talk(SAY_AGGRO);
			events.ScheduleEvent(EVENT_ATTACK_PERIL, 3*IN_MILLISECONDS);
		}

		void EnterEvadeMode()
		{
			me->DespawnOrUnsummon();
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
						case EVENT_ATTACK_STRIFE:
							if (Creature* strife = me->FindNearestCreature(NPC_STRIFE, 500.0f))
							{
								me->AddThreat(strife, 999.0f);
								me->Attack(strife, true);
							}

							events.ScheduleEvent(EVENT_ATTACK_PERIL, 8*IN_MILLISECONDS);
							events.CancelEvent(EVENT_ATTACK_STRIFE);
							break;

						case EVENT_ATTACK_PERIL:
							if (Creature* peril = me->FindNearestCreature(NPC_PERIL, 500.0f))
							{
								me->AddThreat(peril, 999.0f);
								me->Attack(peril, true);
							}

							events.ScheduleEvent(EVENT_ATTACK_STRIFE, 8*IN_MILLISECONDS);
							events.CancelEvent(EVENT_ATTACK_PERIL);
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

void AddSC_boss_lorewalker_stonestep()
{
	new npc_corrupted_scroll();
	new boss_lorewalker_stonestep();
	new npc_strife();
	new npc_peril();
	new npc_osong();
}