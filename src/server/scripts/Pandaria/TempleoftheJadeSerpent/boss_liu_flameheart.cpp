/* # Script de Tydrheal & Sungis : Liu Flameheart # */

/*
	Notes :
	What is missing ? :	- Flame waves system
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
	SPELL_JADE_FIRE_MISSILE		= 107045,

	/* Jade fire */
	SPELL_JADE_FIRE				= 107108
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
	NPC_JADE_FIRE		= 56893,
	NPC_LIU_TRIGGER		= 400445
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
		
		void Reset()
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, NOT_STARTED);
				intro = false;
				thirdPhaseHome = false;
				me->setActive(false);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
				me->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
				me->RemoveAurasDueToSpell(SPELL_MEDITATE);
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

				std::list<Creature*> jadeFires;
				me->GetCreatureListWithEntryInGrid(jadeFires, NPC_JADE_FIRE, 500.0f);
				if (!jadeFires.empty())
				{
					for (std::list<Creature*>::iterator itr = jadeFires.begin(); itr != jadeFires.end(); ++itr)
						(*itr)->DespawnOrUnsummon();
				}
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
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
				me->RemoveAurasDueToSpell(SPELL_MEDITATE);
				me->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
				intro = false;
				thirdPhaseHome = false;
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
				if (Creature* liu = me->FindNearestCreature(NPC_LIU_TRIGGER, 0.1f, true))
				{
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
					me->SetFacingTo(4.410300f);
					me->CastSpell(me, SPELL_MEDITATE);
					events.ScheduleEvent(EVENT_SUMMON_YU_LON, 0, 0, PHASE_YU_LON);
					thirdPhaseHome = true;
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

		void JustSummoned(Creature* summoned)
        {
            me->SetObjectScale(1.0f);
			me->SetInCombatWithZone();
			if (Creature* liu = me->FindNearestCreature(NPC_LIU_FLAMEHEART, 500, true))
				me->SetHealth(liu->GetHealth());

			events.ScheduleEvent(EVENT_JADE_FIRE, 15*IN_MILLISECONDS);
        }

		void JustDied(Unit *pWho)
		{
			if (Creature* liu = me->FindNearestCreature(NPC_LIU_FLAMEHEART, 500, true))
				liu->DealDamage(liu, liu->GetHealth());

			me->DespawnOrUnsummon();
		}

		void EnterCombat()
		{
			if (instance)
				events.ScheduleEvent(EVENT_JADE_FIRE, 15*IN_MILLISECONDS);
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

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_JADE_FIRE:
						{
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
								if (target && target->GetTypeId() == TYPEID_PLAYER)
									me->CastSpell(target, SPELL_JADE_FIRE_MISSILE);

							events.ScheduleEvent(EVENT_JADE_FIRE, 15*IN_MILLISECONDS);
							break;
						}

						default:
							break;
					}
				}
			}

			DoMeleeAttackIfReady();	
		}
	};
};

class npc_jade_fire_trigger: public CreatureScript
{
public:
	npc_jade_fire_trigger() : CreatureScript("npc_jade_fire_trigger") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_jade_fire_triggerAI(creature);
	}

	struct npc_jade_fire_triggerAI : public ScriptedAI
	{
		npc_jade_fire_triggerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		
		void Reset()
		{
			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->CastSpell(me, SPELL_JADE_FIRE);
		}

		void JustSummoned(Creature* summoned)
        {
			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->CastSpell(me, SPELL_JADE_FIRE);
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

class spell_yulon_jade_fire : public SpellScriptLoader
{
    public:
        spell_yulon_jade_fire() : SpellScriptLoader("spell_yulon_jade_fire") { }

        class spell_yulon_jade_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yulon_jade_fire_SpellScript);

			SpellCastResult CheckCast()
            {
				if (GetHitUnit() == GetCaster())
					return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetEffectValue(); // 107098 dbc EffectBasePoints
                GetCaster()->CastSpell(GetHitUnit(), bp0);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_yulon_jade_fire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
				OnCheckCast += SpellCheckCastFn(spell_yulon_jade_fire_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_yulon_jade_fire_SpellScript();
        }
};

void AddSC_boss_liu_flameheart()
{
	new boss_liu_flameheart();
	new npc_yulon();
	new npc_jade_fire_trigger();
	new npc_minion_of_doubt();
	new spell_yulon_jade_fire();
}