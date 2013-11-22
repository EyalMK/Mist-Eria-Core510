/* # Script de Tydrheal & Sungis : Liu Flameheart # */

/*
	Notes :
	What is missing ? :	- Pending
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Liu Flameheart */
	SPELL_JADE_ESSENCE			= 106797,
	SPELL_SERPENT_STRIKE		= 106823,
	SPELL_JADE_STRIKE			= 106841,
	SPELL_SHA_MASK				= 117691,
	SPELL_SHA_CORRUPTION		= 128240,
	SPELL_MEDITATE				= 124416,

	/* Yu'lon */ 
	SPELL_JADE_FIRE				= 107045,
};

enum Events
{
	/* Liu Flameheart */
	EVENT_SERPENT_STRIKE	= 1,
	EVENT_JADE_STRIKE		= 2,
	EVENT_SUMMON_YU_LON		= 3,

	/* Yu'lon */
	EVENT_JADE_FIRE = 4,
};

enum Texts
{
	SAY_AGGRO							= 0,
	SAY_DEATH							= 1,
	SAY_PHASE_LIU_JADE_SERPENT_DANCE	= 2,
	SAY_PHASE_YU_LON					= 3,
	SAY_INTRO							= 4,
	SAY_SLAY_1							= 5,
	SAY_SLAY_2							= 6
};

enum Phases
{
	PHASE_NULL						= 0,
	PHASE_LIU_SERPENT_DANCE			= 1,
	PHASE_LIU_JADE_SERPENT_DANCE	= 2,
	PHASE_YU_LON					= 3
};

enum Npcs
{
	NPC_LIU_FLAMEHEART	= 56732,
	NPC_MINION_OF_DOUBT	= 57109,
	NPC_YU_LON			= 56762,
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
		bool intro;
		bool thirdPhaseHome; // When Liu comes to the center of the "room" in the third phase
		bool yulonSummoned; // When Yu'Lon is summoned
		
		void Reset()
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, NOT_STARTED);
				intro = false;
				thirdPhaseHome = false;
				yulonSummoned = false;
				me->setActive(false);

				me->HandleEmoteCommand(44); // Ready hands
				me->CastSpell(me, SPELL_SHA_MASK);
				me->CastSpell(me, SPELL_SHA_CORRUPTION);
			}
		}

		void JustDied(Unit *pWho)
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, DONE);
				Talk(SAY_DEATH);
			}
		}

		void KilledUnit(Unit *pWho)
		{
			if (instance)
				Talk(irand(SAY_SLAY_1, SAY_SLAY_2));
		}
		
		void EnterEvadeMode()
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, FAIL);
				intro = false;
				thirdPhaseHome = false;
				yulonSummoned = false;
				me->setActive(false);
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, IN_PROGRESS);
				me->SetInCombatWithZone();
				Talk(SAY_AGGRO);
				me->setActive(true);
			}

			events.SetPhase(PHASE_LIU_SERPENT_DANCE);
			events.ScheduleEvent(EVENT_SERPENT_STRIKE, 12*IN_MILLISECONDS, 0, PHASE_LIU_SERPENT_DANCE);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!me->IsWithinDistInMap(who, 30.0f) || intro)
				return;

			if (!who || !who->IsInWorld())
				return;

			if (who && who->GetTypeId() == TYPEID_PLAYER && !intro && !me->IsValidAttackTarget(who))
			{
				Talk(SAY_INTRO);
				intro = true;
			}
		}

		void UpdateAI(uint32 diff)
		{
			if	(!UpdateVictim())
				return;

			events.Update(diff);

			if (events.IsInPhase(PHASE_LIU_SERPENT_DANCE) && HealthBelowPct(70))
			{
				Talk(SAY_PHASE_LIU_JADE_SERPENT_DANCE);
				me->CastSpell(me->getVictim(), SPELL_JADE_ESSENCE);
				events.ScheduleEvent(EVENT_JADE_STRIKE, 12*IN_MILLISECONDS, 0, PHASE_LIU_JADE_SERPENT_DANCE);
				events.SetPhase(PHASE_LIU_JADE_SERPENT_DANCE);
			}

			if (events.IsInPhase(PHASE_LIU_JADE_SERPENT_DANCE) && HealthBelowPct(30))
			{
				Talk(SAY_PHASE_YU_LON);
				me->setActive(false);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->GetMotionMaster()->MovePoint(0, 929.684998f, -2560.610107f, 180.070007f);
				events.SetPhase(PHASE_YU_LON);
			}

			if (events.IsInPhase(PHASE_YU_LON) && !thirdPhaseHome)
				if (me->GetPositionX() == 929.684998f && me->GetPositionY() == -2560.610107f && me->GetPositionZ() == 180.070007f)
				{
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->SetFacingTo(4.410300f);
					me->CastSpell(me, SPELL_MEDITATE);
					events.ScheduleEvent(EVENT_SUMMON_YU_LON, 0, 0, PHASE_YU_LON);
					thirdPhaseHome = true;
				}

			if (events.IsInPhase(PHASE_YU_LON) && yulonSummoned)
				if (Creature* yulon = me->FindNearestCreature(NPC_YU_LON, 500, false))
				{
					me->DealDamage(me, me->GetHealth());
					yulon->DespawnOrUnsummon();
				}

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_SERPENT_STRIKE:
							me->CastSpell(me->getVictim(), SPELL_SERPENT_STRIKE);

							events.ScheduleEvent(EVENT_SERPENT_STRIKE, 12*IN_MILLISECONDS, 0, PHASE_LIU_SERPENT_DANCE);
							break;

						case EVENT_JADE_STRIKE:
							me->CastSpell(me->getVictim(), SPELL_JADE_STRIKE);

							events.ScheduleEvent(EVENT_JADE_STRIKE, 12*IN_MILLISECONDS, 0, PHASE_LIU_JADE_SERPENT_DANCE);
							break;

						case EVENT_SUMMON_YU_LON:
							float x, y, z, o;
							x = me->GetPositionX();
							y = me->GetPositionY();
							z = me->GetPositionZ();
							o = 1.256324f;

							me->SummonCreature(NPC_YU_LON, x, y, z, o, TEMPSUMMON_MANUAL_DESPAWN);
							yulonSummoned = true;

							events.CancelEvent(EVENT_SUMMON_YU_LON);
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

		}

		void JustDied(Unit *pWho)
		{
			
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
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

			me->SetObjectScale(1.0f);
			if (Creature* liu = me->FindNearestCreature(NPC_LIU_FLAMEHEART, 500, true))
				me->SetHealth(liu->GetHealth());
		}

		void JustDied(Unit *pWho) {	}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				me->SetInCombatWithZone();
		}

		void EnterEvadeMode()
		{
			if (instance)
				me->DespawnOrUnsummon();
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
									me->CastSpell(target, SPELL_JADE_FIRE);

							events.ScheduleEvent(EVENT_JADE_FIRE, 15*IN_MILLISECONDS);
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