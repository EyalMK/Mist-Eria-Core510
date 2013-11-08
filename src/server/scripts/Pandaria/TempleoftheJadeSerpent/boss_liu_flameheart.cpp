/* # Script de Tydrheal : Liu Flameheart # */

/* Notes : Tester -- voir spells -- Ajouter SoundID
Liu Flameheart : Script 95% (terminé -- voir spells)	
Minon of doubt : Script 100%
Yulon : Script 95% (voir spells)
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Liu Flameheart */
	SPELL_SERPENT_STRIKE = 106823,
	SPELL_SERPENT_KICK = 106856,
	SPELL_SERPENT_WAVE = 106938,
	SPELL_JADE_SERPENT_STRIKE = 106841,
	SPELL_JADE_SERPENT_KICK = 106864,
	SPELL_JADE_SERPENT_WAVE = 107053,
	
	/* Yu'lon */ 
	SPELL_JADE_FIRE = 107045

};

enum Events
{
	EVENT_BEGIN,
	EVENT_SERPENT_STRIKE,
	EVENT_SERPENT_KICK,
	EVENT_SERPENT_WAVE,
	EVENT_SAY_LEAVE,
	EVENT_SAY_AFTER_PHASE_2,
	EVENT_JADE_SERPENT_STRIKE,
	EVENT_JADE_SERPENT_KICK,
	EVENT_JADE_SERPENT_WAVE,

	/* Yu'lon */
	EVENT_JADE_FIRE
};

enum Actions
{
	ACTION_BOSS_LIU_FLAMEHEART_RESET,
	ACTION_BOSS_LIU_FLAMEHEART_DIED,
	ACTION_MINION_DEATH,
	ACTION_END
};

enum Texts
{
	SAY_BEGIN = 0,
	SAY_COMBAT = 1,
	SAY_LEAVE = 2,
	SAY_PHASE_2 = 3,
	SAY_AFTER_PHASE_2 = 4,
	SAY_PHASE_3 = 5,
	SAY_DEATH = 6,
	SAY_SLAY_1 = 7,
	SAY_SLAY_2 = 8
};

enum Phases
{
	PHASE_BEGIN,
	PHASE_COMBAT,
	PHASE_2,
	PHASE_3,
	PHASE_END
};

enum IdsCreatures
{
	MINION_OF_DOUBT = 57109,
	YULON = 56762
};


class boss_liu_flameheart: public CreatureScript
{
public:
	boss_liu_flameheart() : CreatureScript("boss_liu_flameheart") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_liu_flameheartAI(creature);
	}

	struct boss_liu_flameheartAI : public ScriptedAI
	{
		boss_liu_flameheartAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkLiuFlameheartAlive;
		int counterMinionDeath;
		std::list<Creature*> listMinion;
		Position position;
		
		void Reset() 
		{
			me->GetPosition(&position);
			counterMinionDeath = 0;
			checkLiuFlameheartAlive = true;
			checkLiuFlameheartAlive = me->IsAlive();
			
			events.Reset();

			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, NOT_STARTED);
				me->AI()->DoAction(ACTION_BOSS_LIU_FLAMEHEART_RESET);
				GetCreatureListWithEntryInGrid(listMinion, me, MINION_OF_DOUBT, 50000.0f);
				for(std::list<Creature*>::const_iterator i = listMinion.begin() ; i != listMinion.end() ; ++i)
				{
					(*i)->RemoveCorpse();
					(*i)->Respawn();
				}
			}

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetVisible(false);
			
		}

		void DoAction(int32 action) 
        {
            switch (action)
            {
				case ACTION_BOSS_LIU_FLAMEHEART_RESET:
					checkLiuFlameheartAlive = true;
					break;
				case ACTION_BOSS_LIU_FLAMEHEART_DIED:
					checkLiuFlameheartAlive = false;
					break;
				case ACTION_MINION_DEATH:
					counterMinionDeath++;
					break;
				case ACTION_END:
					Talk(SAY_DEATH);
					me->Kill(me);
					break;
			}
        }

		void JustDied(Unit *pWho)
		{
			if (instance)
			{
				me->AI()->DoAction(ACTION_BOSS_LIU_FLAMEHEART_DIED);

				if (!checkLiuFlameheartAlive)
					instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, DONE);

			}
			
		}

		void KilledUnit(Unit *pWho) 
		{	
			Talk(urand(SAY_SLAY_1, SAY_SLAY_2));
		}
		
		void EnterEvadeMode()
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, FAIL);			
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, IN_PROGRESS);

			me->SetInCombatWithZone();
			events.SetPhase(PHASE_COMBAT);
			Talk(SAY_COMBAT);
			events.ScheduleEvent(EVENT_SERPENT_STRIKE, 8*IN_MILLISECONDS, 0, PHASE_COMBAT);
			events.ScheduleEvent(EVENT_SERPENT_KICK, 2*IN_MILLISECONDS, 0, PHASE_COMBAT);
			events.ScheduleEvent(EVENT_SAY_LEAVE, 10*IN_MILLISECONDS, 0, PHASE_COMBAT);
			events.ScheduleEvent(EVENT_SERPENT_WAVE, 5*IN_MILLISECONDS, 0, PHASE_COMBAT);
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
						case EVENT_BEGIN:
							me->SetVisible(true);
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
							Talk(SAY_BEGIN);
							events.CancelEvent(EVENT_BEGIN);
							break;

						case EVENT_SERPENT_STRIKE:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_SERPENT_STRIKE);
								}
							events.ScheduleEvent(EVENT_SERPENT_STRIKE, 10*IN_MILLISECONDS, 0, PHASE_COMBAT);
							break;

						case EVENT_SERPENT_KICK:
							DoCast(me, SPELL_SERPENT_KICK);
							events.ScheduleEvent(EVENT_SERPENT_KICK, 8*IN_MILLISECONDS, 0, PHASE_COMBAT);
							break;

						case EVENT_SERPENT_WAVE:
							DoCast(me, SPELL_SERPENT_WAVE);
							events.ScheduleEvent(EVENT_SERPENT_WAVE, 8*IN_MILLISECONDS, 0, PHASE_COMBAT);
							break;

						case EVENT_SAY_LEAVE:
							Talk(SAY_LEAVE);
							events.CancelEvent(EVENT_SAY_LEAVE);
							break;

						case EVENT_SAY_AFTER_PHASE_2:
							Talk(SAY_AFTER_PHASE_2);
							events.CancelEvent(EVENT_SAY_AFTER_PHASE_2);
							break;

						case EVENT_JADE_SERPENT_STRIKE:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_JADE_SERPENT_STRIKE);
								}
							events.ScheduleEvent(EVENT_JADE_SERPENT_STRIKE, 10*IN_MILLISECONDS, 0, PHASE_2);
							break;

						case EVENT_JADE_SERPENT_KICK:
							DoCast(me, SPELL_JADE_SERPENT_KICK);
							events.ScheduleEvent(EVENT_JADE_SERPENT_KICK, 8*IN_MILLISECONDS, 0, PHASE_2);
							break;

						case EVENT_JADE_SERPENT_WAVE:
							DoCast(me, SPELL_JADE_SERPENT_WAVE);
							events.ScheduleEvent(EVENT_JADE_SERPENT_WAVE, 8*IN_MILLISECONDS, 0, PHASE_2);
							break;

						default:
							break;
					}
				}
			}

			if(counterMinionDeath = 3 && !events.IsInPhase(PHASE_BEGIN))
			{
				events.ScheduleEvent(EVENT_BEGIN, 0, 0, PHASE_BEGIN);
				events.SetPhase(PHASE_BEGIN);
			}

			if(me->HealthBelowPct(70) && !events.IsInPhase(PHASE_COMBAT))
			{
				Talk(PHASE_2);
				events.ScheduleEvent(EVENT_SAY_AFTER_PHASE_2, 5*IN_MILLISECONDS, 0, PHASE_2);
				events.ScheduleEvent(EVENT_JADE_SERPENT_STRIKE, 1*IN_MILLISECONDS, 0, PHASE_2);
				events.ScheduleEvent(EVENT_JADE_SERPENT_KICK, 2*IN_MILLISECONDS, 0, PHASE_2);
				events.ScheduleEvent(EVENT_JADE_SERPENT_WAVE, 4*IN_MILLISECONDS, 0, PHASE_2);
				events.SetPhase(PHASE_2);
			}

			if(me->HealthBelowPct(30) && !events.IsInPhase(PHASE_2))
			{
				Talk(PHASE_3);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
				me->GetMotionMaster()->MovePoint(0, position);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->SummonCreature(YULON, position, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360*IN_MILLISECONDS);
				events.SetPhase(PHASE_3);
			}
			
			if(!events.IsInPhase(PHASE_3))
			{
				DoMeleeAttackIfReady();
			}
		}
	};
};

class npc_minion_of_doubt: public CreatureScript
{
public:
	npc_minion_of_doubt() : CreatureScript("npc_minion_of_doubt") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_minion_of_doubtAI(creature);
	}

	struct npc_minion_of_doubtAI : public ScriptedAI
	{
		npc_minion_of_doubtAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		void Reset() 
		{
			if(instance)
				if (Creature* liu = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LIU_FLAMEHEART)))
					if (liu->AI())
					{
						liu->AI()->Reset();
					}
		}

		void DoAction(int32 action) 
        {
        }

		void JustDied(Unit *pWho) 
		{	
			if(instance)
				if (Creature* liu = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LIU_FLAMEHEART)))
					if (liu->AI())
					{
						liu->AI()->DoAction(ACTION_MINION_DEATH);
					}
		}

		void KilledUnit(Unit *pWho)
		{	
		}
		
		void EnterEvadeMode()
		{	
		}

		void EnterCombat(Unit* /*who*/) 
		{
			me->SetInCombatWithZone();
		}

		void UpdateAI(uint32 diff) 
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
		}
	};
};

class npc_yulon : public CreatureScript 
{
public:
	npc_yulon() : CreatureScript("npc_yulon") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_yulonAI(creature);
	}

	struct npc_yulonAI : public ScriptedAI
	{
		npc_yulonAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		int health;

		void Reset() 
		{
			events.Reset();

			if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
				health = lorewalker->GetHealth();
			me->SetHealth(health);
		}

		void JustSummoned(Creature* creature) 
		{
			events.Reset();

			if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
				health = lorewalker->GetHealth();
			me->SetHealth(health);
		}

		void DoAction(int32 action) 
		{
		}

		void JustDied(Unit *pWho) 
		{
			events.SetPhase(PHASE_END);
			if(instance)
				if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
					if(lorewalker->AI())
						lorewalker->AI()->DoAction(ACTION_END);
		}

		void EnterCombat(Unit* /*who*/) 
		{
			me->SetInCombatWithZone();
			events.SetPhase(PHASE_3);
			events.ScheduleEvent(EVENT_JADE_FIRE, 6*IN_MILLISECONDS, 0, PHASE_3);
		}

		void EnterEvadeMode()
		{
			me->DespawnOrUnsummon();
			if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
				if(lorewalker->AI())
					lorewalker->AI()->Reset();
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
						case EVENT_JADE_FIRE:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_JADE_FIRE);
								}
							events.ScheduleEvent(EVENT_JADE_FIRE, 15*IN_MILLISECONDS, 0, PHASE_3);
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


void AddSC_boss_liu_flameheart()
{
	new boss_liu_flameheart();
	new npc_minion_of_doubt();
	new npc_yulon();
}