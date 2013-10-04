/* # Script de Sungis : Gekkan # */

/* Notes :
Gekkan : Script 90%					A faire : vérifier si les sorts fonctionnent.
Glintrok Oracle : Script 95%		A faire : vérifier si les sorts fonctionnent.
Glintrok Hexxer : Script 95%		A faire : vérifier si les sorts fonctionnent.
Glintrok Hexxer (mob) : Script 95%	A faire : vérifier si les sorts fonctionnent.
Glintrok Skulker : Script 95%		A faire : vérifier si les sorts fonctionnent.
Glintrok Ironhide : Script 95%		A faire : vérifier si les sorts fonctionnent.

UPDATE creature_template SET ScriptName = 'boss_gekkan' WHERE entry = 61243;
UPDATE creature_template SET ScriptName = 'npc_glintrok_oracle' WHERE entry = 61339;
UPDATE creature_template SET ScriptName = 'npc_glintrok_hexxer' WHERE entry = 61340;
UPDATE creature_template SET ScriptName = 'mob_glintrok_hexxer' WHERE entry = 61216;
UPDATE creature_template SET ScriptName = 'npc_glintrok_skulker' WHERE entry = 61338;
UPDATE creature_template SET ScriptName = 'npc_glintrok_ironhide' WHERE entry = 61337;

INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(61243, 0, 0, "J'entends quelque chose...", 14, 0, 100, 0, 0, 32495, "Gekkan - Intro"),
(61243, 1, 0, "Stoppez-les !", 14, 0, 100, 0, 0, 32488, "Gekkan - Aggro"),
(61243, 2, 0, "Fouillez ce cadavre !", 14, 0, 100, 0, 0, 32496, "Gekkan - Slay"),
(61243, 3, 0, "Quel gaspillage...", 14, 0, 100, 0, 0, 32494, "Gekkan - Death"),
(61243, 4, 0, "Tuez-les tous !", 14, 0, 100, 0, 0, 32497, "Gekkan - Reckless inspiration");
*/

#include "ScriptPCH.h"
#include "mogushan_palace.h"

enum Spells
{
	/* Gekkan */
	SPELL_RECKLESS_INSPIRATION			= /*118988*/88325,
	SPELL_RECKLESS_INSPIRATION_INSTANT	= /*129262*/34971,

	/* Glintrok Oracle */
	SPELL_CLEANSING_FLAME				= /*118940*/77066,
	SPELL_FIRE_BOLT						= /*118936*/83794,

	/* Glintrok Hexxer */
	SPELL_HEX_OF_LETHARGY				= /*118903*/86582,
	SPELL_DARK_BOLT						= /*118917*/90668,
	
	/* Glintrok Skulker */
	SPELL_SHANK							= /*118963*/91998,
	SPELL_STEALTH						= /*118963*/102921,

	/* Glintrok Ironhide */
	SPELL_IRON_PROTECTOR				= /*118958*/465
};

enum Events
{
	/* Gekkan */
	EVENT_RECKLESS_INSPIRATION_GLINTROK	= 1,
	EVENT_RECKLESS_INSPIRATION_GEKKAN	= 2,

	/* Glintrok Oracle */
	EVENT_CLEANSING_FLAME				= 1,
	EVENT_FIRE_BOLT						= 2,
	
	/* Glintrok hexxer */
	EVENT_HEX_OF_LETHARGY				= 1,
	EVENT_DARK_BOLT						= 2,

	/* Glintrok skulker */
	EVENT_SHANK							= 1
};

enum Phases
{
	PHASE_NO_RECKLESS_INSPIRATION_DONE		= 1,
	PHASE_FIRST_RECKLESS_INSPIRATION_DONE	= 2,
	PHASE_SECOND_RECKLESS_INSPIRATION_DONE	= 3,
	PHASE_THIRD_RECKLESS_INSPIRATION_DONE	= 4,
	PHASE_LAST_RECKLESS_INSPIRATION_DONE	= 5
};

enum Actions
{
	ACTION_GEKKAN_RESET,
	ACTION_GEKKAN_DIED,
	ACTION_ORACLE_RESET,
	ACTION_ORACLE_DIED,
	ACTION_HEXXER_RESET,
	ACTION_HEXXER_DIED,
	ACTION_SKULKER_RESET,
	ACTION_SKULKER_DIED,
	ACTION_IRONHIDE_RESET,
	ACTION_IRONHIDE_DIED
};

enum Texts
{
	SAY_INTRO					= 0,
	SAY_AGGRO					= 1,
	SAY_SLAY					= 2,
	SAY_DEATH					= 3,
	SAY_RECKLESS_INSPIRATION	= 4
};

class boss_gekkan : public CreatureScript
{
public:
	boss_gekkan() : CreatureScript("boss_gekkan") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new boss_gekkanAI(creature);
	}

	struct boss_gekkanAI : public ScriptedAI
	{
		boss_gekkanAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		bool checkGekkanAlive;
		bool checkGlintrokOracleAlive;
        bool checkGlintrokHexxerAlive;
		bool checkGlintrokSkulkerAlive;
		bool checkGlintrokIronhideAlive;
		bool lastPhase;

		void Reset() OVERRIDE
		{
			checkGekkanAlive = true;
			checkGekkanAlive = me->IsAlive();

			checkGlintrokOracleAlive = true;
            if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				checkGlintrokOracleAlive = oracle->IsAlive();

			checkGlintrokHexxerAlive = true;
            if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				checkGlintrokHexxerAlive = hexxer->IsAlive();

			checkGlintrokSkulkerAlive = true;
            if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				checkGlintrokSkulkerAlive = skulker->IsAlive();

			checkGlintrokIronhideAlive = true;
            if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				checkGlintrokIronhideAlive = ironhide->IsAlive();

			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				if (!oracle->IsAlive())
				{
					oracle->RemoveCorpse();
					oracle->Respawn();
				}

			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				if (!hexxer->IsAlive())
				{
					hexxer->RemoveCorpse();
					hexxer->Respawn();
				}

			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				if (!skulker->IsAlive())
				{
					skulker->RemoveCorpse();
					skulker->Respawn();
				}

			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				if (!ironhide->IsAlive())
				{
					ironhide->RemoveCorpse();
					ironhide->Respawn();
				}

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION_INSTANT))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION_INSTANT);

			events.Reset();

			if (instance)
			{
				lastPhase = false;
				instance->SetBossState(DATA_GEKKAN, NOT_STARTED);
				me->AI()->DoAction(ACTION_GEKKAN_RESET);
			}
		}

		void DoAction(int32 action) OVERRIDE
        {
            switch (action)
            {
				case ACTION_GEKKAN_RESET:
					checkGekkanAlive = true;
					break;
				case ACTION_GEKKAN_DIED:
					checkGekkanAlive = false;
					break;
				case ACTION_ORACLE_RESET:
					checkGlintrokOracleAlive = true;
					break;
				case ACTION_ORACLE_DIED:
					checkGlintrokOracleAlive = false;
					break;
				case ACTION_HEXXER_RESET:
					checkGlintrokHexxerAlive = true;
					break;
				case ACTION_HEXXER_DIED:
					checkGlintrokHexxerAlive = false;
					break;
				case ACTION_SKULKER_RESET:
					checkGlintrokSkulkerAlive = true;
					break;
				case ACTION_SKULKER_DIED:
					checkGlintrokSkulkerAlive = false;
					break;
				case ACTION_IRONHIDE_RESET:
					checkGlintrokIronhideAlive = true;
					break;
				case ACTION_IRONHIDE_DIED:
					checkGlintrokIronhideAlive = false;
					break;
            }
        }

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
			{
				me->AI()->DoAction(ACTION_GEKKAN_DIED);

				if (!checkGlintrokOracleAlive && !checkGlintrokHexxerAlive && !checkGlintrokSkulkerAlive && !checkGlintrokIronhideAlive)
				{
					instance->SetBossState(DATA_GEKKAN, DONE);
					me->SummonGameObject(GO_ANCIENT_MOGU_TREASURE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), 0, 0, 0, 0, 90000000);
				}

				if (checkGlintrokOracleAlive == true)
					if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
						oracle->AddAura(SPELL_RECKLESS_INSPIRATION_INSTANT, oracle);

				if (checkGlintrokHexxerAlive == true)
					if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
						hexxer->AddAura(SPELL_RECKLESS_INSPIRATION_INSTANT, hexxer);

				if (checkGlintrokSkulkerAlive == true)
					if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
						skulker->AddAura(SPELL_RECKLESS_INSPIRATION_INSTANT, skulker);

				if (checkGlintrokIronhideAlive == true)
					if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
						ironhide->AddAura(SPELL_RECKLESS_INSPIRATION_INSTANT, ironhide);
			}

			Talk(SAY_DEATH);
		}

		void KilledUnit(Unit *pWho) OVERRIDE
		{
			Talk(SAY_SLAY);
		}
		
		void EnterEvadeMode() OVERRIDE
		{
			if (instance)
				instance->SetBossState(DATA_GEKKAN, FAIL);
		}
		
		void CheckIfMobHexxerKilled()
		{
			if (instance->GetBossState(DATA_MOB_GLINTROK_HEXXER) == DONE)
				Talk(SAY_INTRO);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			if (instance)
				instance->SetBossState(DATA_GEKKAN, IN_PROGRESS);

			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				oracle->SetInCombatWithZone();
			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				hexxer->SetInCombatWithZone();
			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				skulker->SetInCombatWithZone();
			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				ironhide->SetInCombatWithZone();

			me->SetInCombatWithZone();
			Talk(SAY_AGGRO);

			events.SetPhase(PHASE_NO_RECKLESS_INSPIRATION_DONE);
			events.ScheduleEvent(EVENT_RECKLESS_INSPIRATION_GLINTROK, 16*IN_MILLISECONDS, 0, PHASE_NO_RECKLESS_INSPIRATION_DONE);
		}

		void UpdateAI(uint32 diff) OVERRIDE
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
						case EVENT_RECKLESS_INSPIRATION_GLINTROK:
							if (Creature* glintrok = RAND(
							me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
							me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)),
							me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)),
							me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
								DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);

							Talk(SAY_RECKLESS_INSPIRATION);
							events.DelayEvents(2*IN_MILLISECONDS);
							events.SetPhase(PHASE_FIRST_RECKLESS_INSPIRATION_DONE);
							break;

						default:
							break;
					}
				}
			}

			if (events.IsInPhase(PHASE_FIRST_RECKLESS_INSPIRATION_DONE))
			{
				if (!checkGlintrokOracleAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_SECOND_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokHexxerAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_SECOND_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokSkulkerAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_SECOND_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_SECOND_RECKLESS_INSPIRATION_DONE);
					}
				}
			}

			if (events.IsInPhase(PHASE_SECOND_RECKLESS_INSPIRATION_DONE))
			{
				if (!checkGlintrokOracleAlive && !checkGlintrokHexxerAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokOracleAlive && !checkGlintrokSkulkerAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE);
					}
				}
				if (!checkGlintrokOracleAlive && !checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokHexxerAlive && !checkGlintrokSkulkerAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokHexxerAlive && !checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokSkulkerAlive && !checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* glintrok = RAND(
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)),
					me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER))))
					{
						DoCast(glintrok, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE);
					}
				}
			}

			if (events.IsInPhase(PHASE_THIRD_RECKLESS_INSPIRATION_DONE))
			{
				if (!checkGlintrokOracleAlive && !checkGlintrokHexxerAlive && !checkGlintrokSkulkerAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
					{
						DoCast(ironhide, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_LAST_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokOracleAlive && !checkGlintrokHexxerAlive && !checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
					{
						DoCast(skulker, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_LAST_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokOracleAlive && !checkGlintrokSkulkerAlive && !checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
					{
						DoCast(hexxer, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_LAST_RECKLESS_INSPIRATION_DONE);
					}
				}

				if (!checkGlintrokHexxerAlive && !checkGlintrokSkulkerAlive && !checkGlintrokIronhideAlive)
				{
					DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);

					if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
					{
						DoCast(oracle, SPELL_RECKLESS_INSPIRATION);
						events.SetPhase(PHASE_LAST_RECKLESS_INSPIRATION_DONE);
					}
				}
			}

			if (events.IsInPhase(PHASE_LAST_RECKLESS_INSPIRATION_DONE) && !checkGlintrokOracleAlive && !checkGlintrokHexxerAlive && !checkGlintrokSkulkerAlive && !checkGlintrokIronhideAlive && lastPhase == false)
			{
				DoCast(me, SPELL_RECKLESS_INSPIRATION_INSTANT);
				lastPhase = true;
			}

			DoMeleeAttackIfReady();
		}
	};
};

class npc_glintrok_oracle : public CreatureScript
{
public:
	npc_glintrok_oracle() : CreatureScript("npc_glintrok_oracle") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_glintrok_oracleAI(creature);
	}

	struct npc_glintrok_oracleAI : public ScriptedAI
	{
		npc_glintrok_oracleAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		bool isWithinMeleeRange;

		void Reset() OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_ORACLE_RESET);

			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				if (!gekkan->IsAlive())
				{
					gekkan->RemoveCorpse();
					gekkan->Respawn();
					gekkan->AI()->EnterEvadeMode();
				}

			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				if (!hexxer->IsAlive())
				{
					hexxer->RemoveCorpse();
					hexxer->Respawn();
					hexxer->AI()->EnterEvadeMode();
				}

			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				if (!skulker->IsAlive())
				{
					skulker->RemoveCorpse();
					skulker->Respawn();
					skulker->AI()->EnterEvadeMode();
				}

			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				if (!ironhide->IsAlive())
				{
					ironhide->RemoveCorpse();
					ironhide->Respawn();
					ironhide->AI()->EnterEvadeMode();
				}

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION);

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION_INSTANT))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION_INSTANT);

			events.Reset();
			isWithinMeleeRange = false;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_ORACLE_DIED);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				if (!gekkan->IsAlive())
					gekkan->SetInCombatWithZone();
			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				if (!hexxer->IsAlive())
					hexxer->SetInCombatWithZone();
			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				if (!skulker->IsAlive())
					skulker->SetInCombatWithZone();
			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				if (!ironhide->IsAlive())
					ironhide->SetInCombatWithZone();

			me->SetInCombatWithZone();

			events.ScheduleEvent(EVENT_CLEANSING_FLAME, 8*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_FIRE_BOLT, 2*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 diff) OVERRIDE
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (me->IsWithinMeleeRange(me->GetVictim(), 2.5f) && isWithinMeleeRange == false)
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				isWithinMeleeRange = true;
			}

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_CLEANSING_FLAME:
							if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
								DoCast(gekkan, SPELL_CLEANSING_FLAME);

							events.ScheduleEvent(EVENT_CLEANSING_FLAME, 8*IN_MILLISECONDS);
							break;

						case EVENT_FIRE_BOLT:
							DoCast(me->GetVictim(), SPELL_FIRE_BOLT);

							events.ScheduleEvent(EVENT_FIRE_BOLT, 3*IN_MILLISECONDS);
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

class npc_glintrok_hexxer : public CreatureScript
{
public:
	npc_glintrok_hexxer() : CreatureScript("npc_glintrok_hexxer") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_glintrok_hexxerAI(creature);
	}

	struct npc_glintrok_hexxerAI : public ScriptedAI
	{
		npc_glintrok_hexxerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		bool isWithinMeleeRange;

		void Reset() OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_HEXXER_RESET);

			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				if (!gekkan->IsAlive())
				{
					gekkan->RemoveCorpse();
					gekkan->Respawn();
					gekkan->AI()->EnterEvadeMode();
				}

			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				if (!oracle->IsAlive())
				{
					oracle->RemoveCorpse();
					oracle->Respawn();
					oracle->AI()->EnterEvadeMode();
				}

			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				if (!skulker->IsAlive())
				{
					skulker->RemoveCorpse();
					skulker->Respawn();
					skulker->AI()->EnterEvadeMode();
				}

			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				if (!ironhide->IsAlive())
				{
					ironhide->RemoveCorpse();
					ironhide->Respawn();
					ironhide->AI()->EnterEvadeMode();
				}

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION);

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION_INSTANT))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION_INSTANT);

			isWithinMeleeRange = false;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			events.Reset();
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_HEXXER_DIED);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				gekkan->SetInCombatWithZone();
			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				oracle->SetInCombatWithZone();
			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				skulker->SetInCombatWithZone();
			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				ironhide->SetInCombatWithZone();

			me->SetInCombatWithZone();
			
			events.ScheduleEvent(EVENT_HEX_OF_LETHARGY, 3*IN_MILLISECONDS, 0);
			events.ScheduleEvent(EVENT_DARK_BOLT, 2*IN_MILLISECONDS, 0);
		}

		void UpdateAI(uint32 diff) OVERRIDE
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (me->IsWithinMeleeRange(me->GetVictim(), 2.5f) && isWithinMeleeRange == false)
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				isWithinMeleeRange = true;
			}

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_HEX_OF_LETHARGY:
							DoCast(me->GetVictim(), SPELL_HEX_OF_LETHARGY);

							events.ScheduleEvent(EVENT_HEX_OF_LETHARGY, 10*IN_MILLISECONDS, 0);
							break;

						case EVENT_DARK_BOLT:
							DoCast(me->GetVictim(), SPELL_DARK_BOLT);

							events.ScheduleEvent(EVENT_DARK_BOLT, 3*IN_MILLISECONDS, 0);
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

class npc_glintrok_skulker : public CreatureScript
{
public:
	npc_glintrok_skulker() : CreatureScript("npc_glintrok_skulker") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_glintrok_skulkerAI (creature);
	}

	struct npc_glintrok_skulkerAI : public ScriptedAI
	{
		npc_glintrok_skulkerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset() OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_SKULKER_RESET);

			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				if (!gekkan->IsAlive())
				{
					gekkan->RemoveCorpse();
					gekkan->Respawn();
					gekkan->AI()->EnterEvadeMode();
				}

			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				if (!oracle->IsAlive())
				{
					oracle->RemoveCorpse();
					oracle->Respawn();
					oracle->AI()->EnterEvadeMode();
				}

			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				if (!hexxer->IsAlive())
				{
					hexxer->RemoveCorpse();
					hexxer->Respawn();
					hexxer->AI()->EnterEvadeMode();
				}

			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				if (!ironhide->IsAlive())
				{
					ironhide->RemoveCorpse();
					ironhide->Respawn();
					ironhide->AI()->EnterEvadeMode();
				}

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION);

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION_INSTANT))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION_INSTANT);

			DoCast(me, SPELL_STEALTH);
			events.Reset();
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_SKULKER_DIED);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				gekkan->SetInCombatWithZone();
			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				oracle->SetInCombatWithZone();
			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				hexxer->SetInCombatWithZone();
			if (Creature* ironhide = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_IRONHIDE)))
				ironhide->SetInCombatWithZone();

			me->SetInCombatWithZone();

			events.ScheduleEvent(EVENT_SHANK, 3*IN_MILLISECONDS, 0);
		}

		void UpdateAI(uint32 diff) OVERRIDE
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (events.ExecuteEvent() == EVENT_SHANK)
			{
				DoCast(me->GetVictim(), SPELL_SHANK);
				events.ScheduleEvent(EVENT_SHANK, 11*IN_MILLISECONDS, 0);
			}

			DoMeleeAttackIfReady();
		}
	};
};

class npc_glintrok_ironhide : public CreatureScript
{
public:
	npc_glintrok_ironhide() : CreatureScript("npc_glintrok_ironhide") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_glintrok_ironhideAI(creature);
	}

	struct npc_glintrok_ironhideAI : public ScriptedAI
	{
		npc_glintrok_ironhideAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		void Reset() OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_IRONHIDE_RESET);

			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				if (!gekkan->IsAlive())
				{
					gekkan->RemoveCorpse();
					gekkan->Respawn();
					gekkan->AI()->EnterEvadeMode();
				}

			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				if (!oracle->IsAlive())
				{
					oracle->RemoveCorpse();
					oracle->Respawn();
					oracle->AI()->EnterEvadeMode();
				}

			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				if (!hexxer->IsAlive())
				{
					hexxer->RemoveCorpse();
					hexxer->Respawn();
					hexxer->AI()->EnterEvadeMode();
				}

			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				if (!skulker->IsAlive())
				{
					skulker->RemoveCorpse();
					skulker->Respawn();
					skulker->AI()->EnterEvadeMode();
				}

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION);

			if (me->HasAura(SPELL_RECKLESS_INSPIRATION_INSTANT))
				me->RemoveAurasDueToSpell(SPELL_RECKLESS_INSPIRATION_INSTANT);

			DoCast(me, SPELL_IRON_PROTECTOR);
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
				if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
					if (gekkan->AI())
					    gekkan->AI()->DoAction(ACTION_IRONHIDE_DIED);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			if (Creature* gekkan = me->GetCreature(*me, instance->GetData64(DATA_GEKKAN)))
				gekkan->SetInCombatWithZone();
			if (Creature* oracle = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_ORACLE)))
				oracle->SetInCombatWithZone();
			if (Creature* hexxer = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_HEXXER)))
				hexxer->SetInCombatWithZone();
			if (Creature* skulker = me->GetCreature(*me, instance->GetData64(DATA_GLINTROK_SKULKER)))
				skulker->SetInCombatWithZone();

			me->SetInCombatWithZone();
		}

		void UpdateAI(uint32 diff) OVERRIDE
		{
			if(!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
	};
};

class mob_glintrok_hexxer : public CreatureScript
{
public:
	mob_glintrok_hexxer() : CreatureScript("mob_glintrok_hexxer") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new mob_glintrok_hexxerAI(creature);
	}

	struct mob_glintrok_hexxerAI : public ScriptedAI
	{
		mob_glintrok_hexxerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		bool isWithinMeleeRange;

		void Reset() OVERRIDE
		{
			isWithinMeleeRange = false;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			events.Reset();
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
				instance->SetBossState(DATA_MOB_GLINTROK_HEXXER, DONE);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			events.ScheduleEvent(EVENT_HEX_OF_LETHARGY, 3*IN_MILLISECONDS, 0);
			events.ScheduleEvent(EVENT_DARK_BOLT, 2*IN_MILLISECONDS, 0);
		}

		void UpdateAI(uint32 diff) OVERRIDE
		{
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (me->IsWithinMeleeRange(me->GetVictim(), 2.5f) && isWithinMeleeRange == false)
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				isWithinMeleeRange = true;
			}

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_HEX_OF_LETHARGY:
							DoCast(me->GetVictim(), SPELL_HEX_OF_LETHARGY);

							events.ScheduleEvent(EVENT_HEX_OF_LETHARGY, 10*IN_MILLISECONDS, 0);
							break;

						case EVENT_DARK_BOLT:
							DoCast(me->GetVictim(), SPELL_DARK_BOLT);

							events.ScheduleEvent(EVENT_DARK_BOLT, 3*IN_MILLISECONDS, 0);
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

void AddSC_boss_gekkan()
{
	new boss_gekkan();
	new npc_glintrok_oracle();
	new npc_glintrok_hexxer();
	new npc_glintrok_skulker();
	new npc_glintrok_ironhide();
	new mob_glintrok_hexxer();
}