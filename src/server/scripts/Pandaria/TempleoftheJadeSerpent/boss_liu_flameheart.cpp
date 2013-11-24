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
	SPELL_JADE_ESSENCE				= 106797,
	SPELL_SERPENT_STRIKE			= 106823,
	SPELL_JADE_STRIKE				= 106841,
	SPELL_SHA_MASK					= 117691,
	SPELL_SHA_CORRUPTION			= 128240,
	SPELL_MEDITATE					= 124416,

	/* Yu'lon */ 
	SPELL_JADE_FIRE_MISSILE			= 107045,

	/* Jade fire */
	SPELL_JADE_FIRE					= 107108,

	/* Serpent Wave */
	SPELL_SERPENT_WAVE				= 106938,
	SPELL_SERPENT_WAVE_VISUAL		= 106939,

	/* Jade Serpent Wave */
	SPELL_JADE_SERPENT_WAVE			= 107053,
	SPELL_JADE_SERPENT_WAVE_VISUAL	= 107002,
	SPELL_JADE_FIRE_SUMMON			= 107103
};

enum Events
{
	/* Liu Flameheart */
	EVENT_SERPENT_STRIKE			= 1,
	EVENT_JADE_STRIKE				= 2,
	EVENT_SUMMON_SERPENT_WAVE		= 3,
	EVENT_SUMMON_JADE_SERPENT_WAVE	= 4,
	EVENT_SERPENT_WAVE_MOVE			= 5,
	EVENT_JADE_SERPENT_WAVE_MOVE	= 6,
	EVENT_SUMMON_YU_LON				= 7,

	/* Yu'lon */
	EVENT_JADE_FIRE					= 1,

	/* Serpent Wave */
	EVENT_SERPENT_WAVE				= 1,

	/* Jade Serpent Wave */
	EVENT_JADE_SERPENT_WAVE			= 1,
	EVENT_JADE_FIRE_SUMMON	= 2
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
	NPC_LIU_FLAMEHEART				= 56732,
	NPC_MINION_OF_DOUBT				= 57109,
	NPC_YU_LON						= 56762,
	NPC_JADE_FIRE					= 56893,
	NPC_LIU_TRIGGER					= 400445,
	NPC_SERPENT_WAVE_TRIGGER		= 400446,
	NPC_JADE_SERPENT_WAVE_TRIGGER	= 400447,
	NPC_WAVE_TRACKER				= 400448
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
		float x, y, z, o;
		Creature* firstWave;
		Creature* secondWave;
		Creature* thirdWave;
		Creature* fourthWave;

		void Reset()
		{
			events.Reset();

			if (instance)
				instance->SetBossState(DATA_BOSS_LIU_FLAMEHEART, NOT_STARTED);

			intro = false;
			thirdPhaseHome = false;

			me->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
			me->setActive(false);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
			me->CastSpell(me, SPELL_SHA_MASK);
			me->CastSpell(me, SPELL_SHA_CORRUPTION);

			if (me->HasAura(SPELL_JADE_ESSENCE))
				me->RemoveAurasDueToSpell(SPELL_JADE_ESSENCE, me->GetGUID());
			if (me->HasAura(SPELL_MEDITATE))
				me->RemoveAurasDueToSpell(SPELL_MEDITATE, me->GetGUID());
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
				intro = false;
				thirdPhaseHome = false;
				me->CombatStop();
				me->DeleteThreatList();
				me->setActive(false);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
				me->Relocate(929.684998f, -2560.610107f, 180.070007f, 4.410300f);
				me->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
				me->SetFacingTo(1.250660f);
				if (me->HasAura(SPELL_JADE_ESSENCE))
					me->RemoveAurasDueToSpell(SPELL_JADE_ESSENCE, me->GetGUID());
				if (me->HasAura(SPELL_MEDITATE))
					me->RemoveAurasDueToSpell(SPELL_MEDITATE, me->GetGUID());
				me->CastSpell(me, SPELL_SHA_MASK);
				me->CastSpell(me, SPELL_SHA_CORRUPTION);

				std::list<Creature*> jadeFires;
				me->GetCreatureListWithEntryInGrid(jadeFires, NPC_JADE_FIRE, 500.0f);
				if (!jadeFires.empty())
				{
					for (std::list<Creature*>::iterator itr = jadeFires.begin(); itr != jadeFires.end(); ++itr)
						(*itr)->DespawnOrUnsummon();
				}
			}
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
			events.ScheduleEvent(EVENT_SUMMON_SERPENT_WAVE, 14*IN_MILLISECONDS, 0, PHASE_LIU_SERPENT_DANCE);
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
				events.CancelEvent(EVENT_SERPENT_STRIKE);
				events.CancelEvent(EVENT_SUMMON_SERPENT_WAVE);
				events.ScheduleEvent(EVENT_JADE_STRIKE, 12*IN_MILLISECONDS, 0, PHASE_LIU_JADE_SERPENT_DANCE);
				events.ScheduleEvent(EVENT_SUMMON_JADE_SERPENT_WAVE, 14*IN_MILLISECONDS, 0, PHASE_LIU_JADE_SERPENT_DANCE);
				events.SetPhase(PHASE_LIU_JADE_SERPENT_DANCE);
			}

			if (events.IsInPhase(PHASE_LIU_JADE_SERPENT_DANCE) && HealthBelowPct(30))
			{
				Talk(SAY_PHASE_YU_LON);
				me->setActive(false);
				events.CancelEvent(EVENT_JADE_STRIKE);
				events.CancelEvent(EVENT_SUMMON_JADE_SERPENT_WAVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->GetMotionMaster()->MovePoint(0, 929.684998f, -2560.610107f, 180.070007f);
				events.SetPhase(PHASE_YU_LON);
			}

			if (events.IsInPhase(PHASE_YU_LON) && !thirdPhaseHome)
				if (Creature* liu = me->FindNearestCreature(NPC_LIU_TRIGGER, 0.1f, true))
				{
					me->Relocate(929.684998f, -2560.610107f, 180.070007f, 4.410300f);
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
						{
							x = me->GetPositionX();
							y = me->GetPositionY();
							z = me->GetPositionZ();
							o = 1.256324f;

							me->SummonCreature(NPC_YU_LON, x, y, z, o, TEMPSUMMON_MANUAL_DESPAWN);

							events.CancelEvent(EVENT_SUMMON_YU_LON);
							break;
						}

						case EVENT_SUMMON_SERPENT_WAVE:
						{
							x = me->GetPositionX();
							y = me->GetPositionY();
							z = 179.821503;

							if (Creature* firstTracker = me->SummonCreature(NPC_WAVE_TRACKER, x + 100.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (firstWave = me->SummonCreature(NPC_SERPENT_WAVE_TRIGGER, x + 10.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									firstWave->SetFacingToObject(firstTracker);

							if (Creature* secondTracker = me->SummonCreature(NPC_WAVE_TRACKER, x, y + 100.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (secondWave = me->SummonCreature(NPC_SERPENT_WAVE_TRIGGER, x, y + 10.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									secondWave->SetFacingToObject(secondTracker);
							
							if (Creature* thirdTracker = me->SummonCreature(NPC_WAVE_TRACKER, x - 100.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (thirdWave = me->SummonCreature(NPC_SERPENT_WAVE_TRIGGER, x - 10.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									thirdWave->SetFacingToObject(thirdTracker);

							if (Creature* fourthTracker = me->SummonCreature(NPC_WAVE_TRACKER, x, y - 100.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (fourthWave = me->SummonCreature(NPC_SERPENT_WAVE_TRIGGER, x, y - 10.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									fourthWave->SetFacingToObject(fourthTracker);

							events.ScheduleEvent(EVENT_SERPENT_WAVE_MOVE, 3*IN_MILLISECONDS, 0, PHASE_LIU_SERPENT_DANCE);
							events.ScheduleEvent(EVENT_SUMMON_SERPENT_WAVE, 12*IN_MILLISECONDS, 0, PHASE_LIU_SERPENT_DANCE);
							break;
						}

						case EVENT_SERPENT_WAVE_MOVE:
							if (firstWave)
								firstWave->GetMotionMaster()->MovePoint(0, x + 100.0f, y, z);
							if (secondWave)
								secondWave->GetMotionMaster()->MovePoint(0, x, y + 100.0f, z);
							if (thirdWave)
								thirdWave->GetMotionMaster()->MovePoint(0, x - 100.0f, y, z);
							if (fourthWave)
								fourthWave->GetMotionMaster()->MovePoint(0, x, y - 100.0f, z);
							events.CancelEvent(EVENT_SERPENT_WAVE_MOVE);
							break;

						case EVENT_SUMMON_JADE_SERPENT_WAVE:
						{
							x = me->GetPositionX();
							y = me->GetPositionY();
							z = 179.821503;

							if (Creature* firstTracker = me->SummonCreature(NPC_WAVE_TRACKER, x + 100.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (firstWave = me->SummonCreature(NPC_JADE_SERPENT_WAVE_TRIGGER, x + 10.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									firstWave->SetFacingToObject(firstTracker);

							if (Creature* secondTracker = me->SummonCreature(NPC_WAVE_TRACKER, x, y + 100.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (secondWave = me->SummonCreature(NPC_JADE_SERPENT_WAVE_TRIGGER, x, y + 10.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									secondWave->SetFacingToObject(secondTracker);
							
							if (Creature* thirdTracker = me->SummonCreature(NPC_WAVE_TRACKER, x - 100.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (thirdWave = me->SummonCreature(NPC_JADE_SERPENT_WAVE_TRIGGER, x - 10.0f, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									thirdWave->SetFacingToObject(thirdTracker);

							if (Creature* fourthTracker = me->SummonCreature(NPC_WAVE_TRACKER, x, y - 100.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
								if (fourthWave = me->SummonCreature(NPC_JADE_SERPENT_WAVE_TRIGGER, x, y - 10.0f, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
									fourthWave->SetFacingToObject(fourthTracker);
							
							events.ScheduleEvent(EVENT_JADE_SERPENT_WAVE_MOVE, 3*IN_MILLISECONDS, 0, PHASE_LIU_JADE_SERPENT_DANCE);
							events.ScheduleEvent(EVENT_SUMMON_JADE_SERPENT_WAVE, 12*IN_MILLISECONDS, 0, PHASE_LIU_JADE_SERPENT_DANCE);
							break;
						}

						case EVENT_JADE_SERPENT_WAVE_MOVE:
							if (firstWave)
								firstWave->GetMotionMaster()->MovePoint(0, x + 100.0f, y, z);
							if (secondWave)
								secondWave->GetMotionMaster()->MovePoint(0, x, y + 100.0f, z);
							if (thirdWave)
								thirdWave->GetMotionMaster()->MovePoint(0, x - 100.0f, y, z);
							if (fourthWave)
								fourthWave->GetMotionMaster()->MovePoint(0, x, y - 100.0f, z);

							events.CancelEvent(EVENT_JADE_SERPENT_WAVE_MOVE);
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
		bool healthApplied;

		void Reset()
		{
			events.Reset();
            
			healthApplied = false;

			if (instance)
			{
				if (Creature* liu = me->FindNearestCreature(NPC_LIU_FLAMEHEART, 500, true))
					if (!healthApplied)
					{
						me->SetHealth(liu->GetMaxHealth() * 0.3f);
						healthApplied = true;
					}

				me->SetObjectScale(0.1f); // Spawn animation
				me->SetObjectScale(1.0f); // Spawn animation
				events.ScheduleEvent(EVENT_JADE_FIRE, 15*IN_MILLISECONDS);
				me->SetInCombatWithZone();
			}
		}

		void JustDied(Unit *pWho)
		{
			if (instance)
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
							me->CastSpell(me->getVictim(), SPELL_JADE_FIRE_MISSILE);

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
			me->CastSpell(me, SPELL_JADE_FIRE);
		}

		void JustSummoned(Creature* summoned)
        {
			me->setActive(false);
			me->CastSpell(me, SPELL_JADE_FIRE);
        }
	};
};

class npc_serpent_wave_trigger: public CreatureScript
{
public:
	npc_serpent_wave_trigger() : CreatureScript("npc_serpent_wave_trigger") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_serpent_wave_triggerAI(creature);
	}

	struct npc_serpent_wave_triggerAI : public ScriptedAI
	{
		npc_serpent_wave_triggerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset()
		{
			me->setActive(false);
			me->CastSpell(me, SPELL_SERPENT_WAVE_VISUAL);
			events.ScheduleEvent(EVENT_SERPENT_WAVE, 1*IN_MILLISECONDS);
		}

		void JustSummoned(Creature* summoned)
        {
			me->setActive(false);
			me->CastSpell(me, SPELL_SERPENT_WAVE_VISUAL);
			events.ScheduleEvent(EVENT_SERPENT_WAVE, 1*IN_MILLISECONDS);
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
						case EVENT_SERPENT_WAVE:
							me->CastSpell(me, SPELL_SERPENT_WAVE);

							events.ScheduleEvent(EVENT_SERPENT_WAVE, 500);
							break;
						default:
							break;
					}
				}
			}
		}
	};
};

class npc_jade_serpent_wave_trigger: public CreatureScript
{
public:
	npc_jade_serpent_wave_trigger() : CreatureScript("npc_jade_serpent_wave_trigger") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_jade_serpent_wave_triggerAI(creature);
	}

	struct npc_jade_serpent_wave_triggerAI : public ScriptedAI
	{
		npc_jade_serpent_wave_triggerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		int32 maxJadeFires;

		void Reset()
		{
			maxJadeFires = 0;
			me->setActive(false);
			me->CastSpell(me, SPELL_JADE_SERPENT_WAVE_VISUAL);
			events.ScheduleEvent(EVENT_JADE_SERPENT_WAVE, 1*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_JADE_FIRE_SUMMON, 3700);
		}

		void JustSummoned(Creature* summoned)
        {
			maxJadeFires = 0;
			me->setActive(false);
			me->CastSpell(me, SPELL_JADE_SERPENT_WAVE_VISUAL);
			events.ScheduleEvent(EVENT_JADE_SERPENT_WAVE, 1*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_JADE_FIRE_SUMMON, 3700);
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
						case EVENT_JADE_FIRE_SUMMON:
						{
							if (maxJadeFires <= 5)
							{
								me->CastSpell(me, SPELL_JADE_FIRE_SUMMON);
								maxJadeFires++;
							}

							events.ScheduleEvent(EVENT_JADE_FIRE_SUMMON, 500);
							break;
						}

						case EVENT_JADE_SERPENT_WAVE:
							me->CastSpell(me, SPELL_JADE_SERPENT_WAVE);

							events.ScheduleEvent(EVENT_JADE_SERPENT_WAVE, 500);
							break;

						default:
							break;
					}
				}
			}
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

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetEffectValue(); // 107098 dbc EffectBasePoints
                GetCaster()->CastSpell(GetHitUnit(), bp0);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_yulon_jade_fire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
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
	new npc_serpent_wave_trigger();
	new npc_jade_serpent_wave_trigger();
	new spell_yulon_jade_fire();
}