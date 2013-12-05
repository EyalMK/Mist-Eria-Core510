/* # Script de Tydrheal & Sungis : Sha of Doubt # */

/*
	Notes : What is missing ? - Pending ...
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Sha Of Doubt */
	SPELL_WITHER_WILL				= 106736,
	SPELL_BOUNDS_OF_REALITY			= 117665,
	SPELL_TOUCH_OF_NOTHINGNESS		= 106113,
	SPELL_FIGMENT_OF_DOUBT_CLONE	= 106935,
	SPELL_FIGMENT_OF_DOUBT			= 106936,

	/* Figment of Doubt */
	SPELL_SHADOWFORM				= 107903,
	SPELL_RELEASE_DOUBT				= 106112,
	SPELL_GATHERING_DOUBT			= 117570,
};

enum Events
{
	/* Sha Of Doubt */
	EVENT_WITHER_WILL				= 1,
	EVENT_TOUCH_OF_NOTHINGNESS		= 2,
	EVENT_BOUNDS_OF_REALITY			= 3,
	EVENT_SUMMON_FIGMENT_OF_DOUBT	= 4,

	/* Figment of Doubt */
	EVENT_ATTACK_PLAYERS			= 1,
	EVENT_RELEASE_DOUBT				= 2,
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
	NPC_FIGMENT_OF_DOUBT	= 56792,
	NPC_SHA_TRIGGER			= 400453,
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
		bool boundsOfReality;
		bool seventyFivePct;
		bool fiftyPct;
		uint32 boundsCount;

		void Reset()
		{		
			events.Reset();

			if (instance)
			{
				boundsOfReality = false;
				seventyFivePct = false;
				fiftyPct = false;
				boundsCount = 0;

				events.SetPhase(PHASE_NULL);
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, NOT_STARTED);
			}
		}
		
		void JustDied(Unit *pWho)
		{
			if (instance)
					instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, DONE);
		}

		void KilledUnit(Unit *pWho) {	}
		
		void EnterEvadeMode()
		{
			if (instance)
			{
				boundsOfReality = false;
				seventyFivePct = false;
				fiftyPct = false;
				boundsCount = 0;

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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HealthBelowPct(75) && events.IsInPhase(PHASE_COMBAT) && !seventyFivePct)
			{
				boundsCount = 1;
				events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, 1*IN_MILLISECONDS, 0, PHASE_BOUNDS_OF_REALITY);
				events.SetPhase(PHASE_BOUNDS_OF_REALITY);
			}

			if (HealthBelowPct(50) && events.IsInPhase(PHASE_COMBAT) && !fiftyPct)
			{
				boundsCount = 2;
				events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, 1*IN_MILLISECONDS, 0, PHASE_BOUNDS_OF_REALITY);
				events.SetPhase(PHASE_BOUNDS_OF_REALITY);
			}

			if (events.IsInPhase(PHASE_BOUNDS_OF_REALITY) && !boundsOfReality)
				if (Creature* trigger = me->FindNearestCreature(NPC_SHA_TRIGGER, 0.1f, true))
				{
					me->Relocate(trigger->GetHomePosition());
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
					me->SetFacingTo(4.410300f);
					me->CastSpell(me, SPELL_BOUNDS_OF_REALITY);
					events.ScheduleEvent(EVENT_SUMMON_FIGMENT_OF_DOUBT, 0, 0, PHASE_BOUNDS_OF_REALITY);
					boundsOfReality = true;
				}

			if (boundsOfReality && !me->HasAura(SPELL_BOUNDS_OF_REALITY))
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
				events.SetPhase(PHASE_COMBAT);
				boundsOfReality = false;
			}

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

						case EVENT_BOUNDS_OF_REALITY:
							if (Creature* trigger = me->FindNearestCreature(NPC_SHA_TRIGGER, 99999.0f, true))
								me->GetMotionMaster()->MovePoint(0, trigger->GetHomePosition());

							if (boundsCount == 1)
								seventyFivePct = true;

							if (boundsCount == 2)
								fiftyPct = true;

							events.CancelEvent(EVENT_BOUNDS_OF_REALITY);
							break;

						case EVENT_SUMMON_FIGMENT_OF_DOUBT:
							instance->DoCastSpellOnPlayers(SPELL_FIGMENT_OF_DOUBT);

							events.CancelEvent(EVENT_SUMMON_FIGMENT_OF_DOUBT);
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

class npc_figment_of_doubt : public CreatureScript 
{
public:
    npc_figment_of_doubt() : CreatureScript("npc_figment_of_doubt") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_figment_of_doubtAI(creature);
    }

    struct npc_figment_of_doubtAI : public ScriptedAI
    {
        npc_figment_of_doubtAI(Creature *creature) : ScriptedAI(creature)
        {
        }

		InstanceScript* instance;
        EventMap events;
		Player* player;
		bool emote;

        void Reset()
		{
			events.Reset();

			emote = false;

			me->setActive(false);
			me->SetDisableGravity(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->CastSpell(me, SPELL_SHADOWFORM);

			events.ScheduleEvent(EVENT_ATTACK_PLAYERS, 4*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_RELEASE_DOUBT, 30*IN_MILLISECONDS);

			if (instance)
			{
				Map* map = me->GetMap();

				if (map && map->IsDungeon())
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
					{
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
						{
							if (player = i->getSource())
							{
								if (Creature* figment = player->FindNearestCreature(NPC_FIGMENT_OF_DOUBT, 99999.0f, true))
								{
									float x, y, z, o;
									x = player->GetPositionX();
									y = player->GetPositionY();
									z = player->GetPositionZ();
									o = player->GetOrientation();
									figment->Relocate(x, y, z + 5.0f, o);
									player->CastSpell(figment, SPELL_FIGMENT_OF_DOUBT_CLONE);
									figment->SetDisplayId(player->GetDisplayId());
								}
							}
						}
					}
				}
			}
		}

		void JustSummoned(Creature* summoned)
        {
			events.Reset();

			emote = false;

			me->setActive(false);
			me->SetDisableGravity(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->CastSpell(me, SPELL_SHADOWFORM);

			events.ScheduleEvent(EVENT_ATTACK_PLAYERS, 4*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_RELEASE_DOUBT, 30*IN_MILLISECONDS);

			if (instance)
			{
				Map* map = me->GetMap();

				if (map && map->IsDungeon())
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
					{
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
						{
							if (player = i->getSource())
							{
								if (Creature* figment = player->FindNearestCreature(NPC_FIGMENT_OF_DOUBT, 99999.0f, true))
								{
									float x, y, z, o;
									x = player->GetPositionX();
									y = player->GetPositionY();
									z = player->GetPositionZ();
									o = player->GetOrientation();
									figment->Relocate(x, y, z + 5.0f, o);
									player->CastSpell(figment, SPELL_FIGMENT_OF_DOUBT_CLONE);
									figment->SetDisplayId(player->GetDisplayId());
								}
							}
						}
					}
				}
			}
        }

		void EnterEvadeMode()
		{
			if (instance)
				me->DespawnOrUnsummon();
		}

        void JustDied(Unit *pWho)
        {
			if (instance)
			{
				if (me->FindNearestCreature(NPC_FIGMENT_OF_DOUBT, 99999.0f, true))
					me->DespawnOrUnsummon();

				else
				{
					if (Creature* sha = me->FindNearestCreature(BOSS_SHA_OF_DOUBT, 99999.0f, true))
					{
						sha->RemoveAurasDueToSpell(SPELL_BOUNDS_OF_REALITY, sha->GetGUID());
						me->DespawnOrUnsummon();
					}
				}
			}
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);

			if (!emote)
			{
				me->HandleEmoteCommand(EMOTE_STATE_DROWNED);
				emote = true;
			}

			if (!UpdateVictim())
			{
				while(uint32 eventId = events.ExecuteEvent())
				{
					switch(eventId)
					{
						if (instance)
						{
							case EVENT_ATTACK_PLAYERS:
								me->setActive(true);
								me->SetDisableGravity(true);
								me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
								me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
								me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
								me->CastSpell(me, SPELL_GATHERING_DOUBT);
								me->setFaction(14);
								me->SetInCombatWith(player);
								me->AddThreat(player, 99999.0f);
							
								events.CancelEvent(EVENT_ATTACK_PLAYERS);
								break;

							case EVENT_RELEASE_DOUBT:
								me->CastSpell(me, SPELL_RELEASE_DOUBT);
								me->Kill(me);

								events.CancelEvent(EVENT_RELEASE_DOUBT);
								break;

							default:
								break;
						}
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
	new npc_figment_of_doubt();
}