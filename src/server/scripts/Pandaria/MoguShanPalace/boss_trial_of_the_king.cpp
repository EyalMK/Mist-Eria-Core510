/* # Script de Sungis : Trial of the King # */

#include "ScriptPCH.h"
#include "SpellScript.h"
#include "mogushan_palace.h"

enum Spells
{
	/* Kuai the Brute */
	SPELL_SHOCKWAVE						= 119922,

	/* Mu'Shiba */
	SPELL_LEAP							= 134729,
	SPELL_RAVAGE						= 119946,
	
	/* Ming the Cunning */
	SPELL_LIGHTNING_BOLT				= 123654,
	SPELL_MAGNETIC_FIELD				= 120100,
	SPELL_SUMMON_WHIRLING_DERVISH		= 119981,
	SPELL_WHIRLING_DERVISH_VISUAL		= 119982,

	/* Haiyan the Unstoppable */
	SPELL_CONFLAGRATE					= 120160,
	SPELL_CONFLAGRATE_TRIGGER			= 120201,
	SPELL_CONFLAGRATE_INSTANT			= 120167,
	SPELL_METEOR						= 120195,
	SPELL_TRAUMATIC_BLOW				= 123655,
};

enum Actions
{
	ACTION_KUAI_THE_BRUTE_RESET,
	ACTION_KUAI_THE_BRUTE_DIED,
	ACTION_MING_THE_CUNNING_RESET,
	ACTION_MING_THE_CUNNING_DIED,
	ACTION_HAIYAN_THE_UNSTOPPABLE_RESET,
	ACTION_HAIYAN_THE_UNSTOPPABLE_DIED,
	ACTION_BOSS_ENTER_COMBAT,
};

enum Events
{
	/* Xin the Weaponmaster */
	EVENT_INTRO_1		= 1,
	EVENT_INTRO_2		= 2,
	EVENT_JUMP_BACK		= 3,
	EVENT_CHOOSE_BOSS	= 4,

	/* Mu'Shiba */
	EVENT_RAVAGE		= 1,
	EVENT_LEAP			= 2,

	/* Kuai the Brute */
	EVENT_SHOCKWAVE		= 1,
};

enum Texts
{
	/* Xin the Weaponmaster */
	SAY_INTRO_1,
	SAY_INTRO_2,
};

enum Phases
{
	PHASE_NULL			= 0,
	PHASE_XIN_COMBAT	= 1,
	PHASE_BOSS_WAITING	= 1,
	PHASE_BOSS_ATTACK	= 2,
	PHASE_BOSS_DONE		= 3,
};

static Position XinJumpPosition = { -4297.225098f, -2613.670898f, 22.324942f };

class npc_xin_the_weaponmaster : public CreatureScript
{
public:
	npc_xin_the_weaponmaster() : CreatureScript("npc_xin_the_weaponmaster") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_xin_the_weaponmasterAI (pCreature);
	}

	struct npc_xin_the_weaponmasterAI : public ScriptedAI
	{
		npc_xin_the_weaponmasterAI(Creature *c) : ScriptedAI(c)
		{
			instance = c->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		bool intro;

		void Reset()
		{
			events.Reset();
			intro = false;

			if (instance)
				instance->SetData(DATA_NPC_XIN_THE_WEAPONMASTER, NOT_STARTED);

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

			events.SetPhase(PHASE_NULL);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!me->IsWithinDistInMap(who, 50.0f) || intro)
				return;

			if (!who || !who->IsInWorld())
				return;

			if (who && who->GetTypeId() == TYPEID_PLAYER && !intro && !me->IsValidAttackTarget(who) && who->isAlive())
			{
				events.SetPhase(PHASE_XIN_COMBAT);
				events.ScheduleEvent(EVENT_INTRO_1, 2*IN_MILLISECONDS, 0, PHASE_XIN_COMBAT);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				DoZoneInCombat();

				if (instance)
					instance->SetData(DATA_NPC_XIN_THE_WEAPONMASTER, IN_PROGRESS);

				intro = true;
			}
		}

		void EnterEvadeMode()
		{
			if (instance)
				instance->SetData(DATA_NPC_XIN_THE_WEAPONMASTER, FAIL);

			ScriptedAI::EnterEvadeMode();
		}

		void UpdateAI(const uint32 diff)
		{
			events.Update(diff);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_INTRO_1:
							Talk(SAY_INTRO_1);
							me->HandleEmoteCommand(EMOTE_STATE_ROAR);

							events.ScheduleEvent(EVENT_INTRO_2, 12*IN_MILLISECONDS);
							events.CancelEvent(EVENT_INTRO_1);
							break;

						case EVENT_INTRO_2:
							Talk(SAY_INTRO_2);

							events.ScheduleEvent(EVENT_JUMP_BACK, 12*IN_MILLISECONDS);
							events.CancelEvent(EVENT_INTRO_2);
							break;

						case EVENT_JUMP_BACK:
							me->GetMotionMaster()->MoveJump(XinJumpPosition, 5.0f, 7.5f);

							events.ScheduleEvent(EVENT_CHOOSE_BOSS, 3*IN_MILLISECONDS);
							events.CancelEvent(EVENT_JUMP_BACK);
							break;

						case EVENT_CHOOSE_BOSS:
							if (Creature* boss = /*RAND(*/me->GetCreature(*me, instance->GetData64(DATA_KUAI_THE_BRUTE)))/*,
							                          me->GetCreature(*me, instance->GetData64(DATA_MING_THE_CUNNING)),
													  me->GetCreature(*me, instance->GetData64(DATA_HAIYAN_THE_UNSTOPPABLE)))*/
													  boss->AI()->DoAction(ACTION_BOSS_ENTER_COMBAT);

							events.CancelEvent(EVENT_CHOOSE_BOSS);
							break;

						default:
							break;
					}
				}
			}
		}
	};
};

class boss_kuai_the_brute : public CreatureScript
{
public:
	boss_kuai_the_brute() : CreatureScript("boss_kuai_the_brute") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new boss_kuai_the_bruteAI (pCreature);
	}

	struct boss_kuai_the_bruteAI : public ScriptedAI
	{
		boss_kuai_the_bruteAI(Creature *c) : ScriptedAI(c)
		{
			instance = c->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		bool checkKuaiTheBruteAlive;
		bool checkMingTheCunningAlive;
        bool checkHaiyanTheUnstoppableAlive;

		void Reset()
		{
			events.Reset();
			checkKuaiTheBruteAlive = true;
			checkMingTheCunningAlive = true;
			checkHaiyanTheUnstoppableAlive = true;

			me->SetReactState(REACT_PASSIVE);

			events.SetPhase(PHASE_BOSS_WAITING);
		}

		void DoAction(int32 action)
        {
            switch (action)
            {
				case ACTION_BOSS_ENTER_COMBAT:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
					me->SetInCombatWithZone();
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (Creature* mushiba = me->GetCreature(*me, instance->GetData64(DATA_MUSHIBA)))
			{
				mushiba->SetReactState(REACT_AGGRESSIVE);
				mushiba->SetInCombatWithZone();
			}

			events.SetPhase(PHASE_BOSS_ATTACK);
			events.ScheduleEvent(EVENT_SHOCKWAVE, 17*IN_MILLISECONDS, 0);
		}

		void KilledUnit(Unit *pWho)
		{

		}
		
		void EnterEvadeMode()
		{
			if (instance)
				instance->SetData(DATA_KUAI_THE_BRUTE, FAIL);

			ScriptedAI::EnterEvadeMode();
		}

		void DamageTaken(Unit* who, uint32& damage)
		{
			if (damage >= me->GetHealth())
			{
				damage = 0;
				JustDied(who);
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			if (Creature* mushiba = me->GetCreature(*me, instance->GetData64(DATA_MUSHIBA)))
			{
				mushiba->SetReactState(REACT_PASSIVE);
				mushiba->AttackStop();
				mushiba->DeleteThreatList();
				mushiba->CombatStop(true);
				mushiba->AI()->EnterEvadeMode();
			}

			me->SetReactState(REACT_PASSIVE);
			me->AttackStop();
			me->DeleteThreatList();
			me->CombatStop(true);
		}

		void UpdateAI(const uint32 diff)
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
						case EVENT_SHOCKWAVE:
							me->CastSpell(me->getVictim(), SPELL_SHOCKWAVE);

							events.ScheduleEvent(EVENT_SHOCKWAVE, 17*IN_MILLISECONDS, 0);
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

class boss_mushiba : public CreatureScript
{
public:
	boss_mushiba() : CreatureScript("boss_mushiba") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new boss_mushibaAI (pCreature);
	}

	struct boss_mushibaAI : public ScriptedAI
	{
		boss_mushibaAI(Creature *c) : ScriptedAI(c)
		{
			instance = c->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		Unit* ravageVictim;

		void Reset()
		{
			events.Reset();
			ravageVictim = NULL;

			me->SetReactState(REACT_PASSIVE);
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->SetReactState(REACT_AGGRESSIVE);
			events.ScheduleEvent(EVENT_RAVAGE, 30*IN_MILLISECONDS);
		}
		
		void EnterEvadeMode()
		{
			ScriptedAI::EnterEvadeMode();
		}

		void JustDied(Unit* /*killer*/)
		{
			if (instance)
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RAVAGE);
		}

		void UpdateAI(const uint32 diff)
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
						case EVENT_LEAP:
						{
							ravageVictim = SelectTarget(SELECT_TARGET_RANDOM, 1, 500, true);

							if (ravageVictim)
								me->CastSpell(ravageVictim, SPELL_LEAP);

							events.ScheduleEvent(EVENT_RAVAGE, 1*IN_MILLISECONDS);
							events.CancelEvent(EVENT_LEAP);
							break;
						}

						case EVENT_RAVAGE:
							if (ravageVictim && ravageVictim->isAlive())
								me->CastSpell(ravageVictim, SPELL_RAVAGE);

							events.ScheduleEvent(EVENT_LEAP, 30*IN_MILLISECONDS);
							events.CancelEvent(EVENT_RAVAGE);
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

void AddSC_trial_of_the_king()
{
	new npc_xin_the_weaponmaster();
	new boss_kuai_the_brute();
	new boss_mushiba();
}