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
	EVENT_MOVE_TO_THE_CENTER		= 3,
	EVENT_BOUNDS_OF_REALITY			= 4,
	EVENT_SUMMON_FIGMENT_OF_DOUBT	= 5,

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
	NPC_FIGMENT_OF_DOUBT	= 56792, // displayid = 15435
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
		Map* map;
		Unit* player;
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
			{
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, DONE);

				map = me->GetMap();

				if (map && map->IsDungeon())
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (player = i->getSource()->ToPlayer())
								if (player->HasAura(SPELL_TOUCH_OF_NOTHINGNESS))
									player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_NOTHINGNESS, player->GetGUID());
				}
			}
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

				me->CombatStop();
				me->DeleteThreatList();

				if (me->HasAura(SPELL_BOUNDS_OF_REALITY))
					me->RemoveAurasDueToSpell(SPELL_BOUNDS_OF_REALITY, me->GetGUID());

				if (GameObject* go = me->FindNearestGameObject(GO_SHA_OF_DOUBT_GATE, 9999.0f))
					go->UseDoorOrButton();

				std::list<Creature*> figments;
				me->GetCreatureListWithEntryInGrid(figments, NPC_FIGMENT_OF_DOUBT, 99999.0f);
				if (!figments.empty())
				{
					for (std::list<Creature*>::iterator itr = figments.begin(); itr != figments.end(); ++itr)
						(*itr)->DespawnOrUnsummon();
				}
			}
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, IN_PROGRESS);
				me->SetInCombatWithZone();
			}

			events.SetPhase(PHASE_COMBAT);
			events.ScheduleEvent(EVENT_WITHER_WILL, 2*IN_MILLISECONDS, 0, PHASE_COMBAT);
			events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 8*IN_MILLISECONDS, 0, PHASE_COMBAT);

			if (GameObject* go = me->FindNearestGameObject(GO_SHA_OF_DOUBT_GATE, 9999.0f))
					go->UseDoorOrButton();
		}
		
		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (HealthBelowPct(75) && events.IsInPhase(PHASE_COMBAT) && !seventyFivePct)
			{
				boundsCount = 1;
				events.ScheduleEvent(EVENT_MOVE_TO_THE_CENTER, 1*IN_MILLISECONDS, 0, PHASE_BOUNDS_OF_REALITY);
				events.SetPhase(PHASE_BOUNDS_OF_REALITY);
			}

			if (HealthBelowPct(50) && events.IsInPhase(PHASE_COMBAT) && !fiftyPct)
			{
				boundsCount = 2;
				events.ScheduleEvent(EVENT_MOVE_TO_THE_CENTER, 1*IN_MILLISECONDS, 0, PHASE_BOUNDS_OF_REALITY);
				events.SetPhase(PHASE_BOUNDS_OF_REALITY);
			}

			if (events.IsInPhase(PHASE_BOUNDS_OF_REALITY) && !boundsOfReality && !me->HasAura(SPELL_BOUNDS_OF_REALITY))
				if (me->FindNearestCreature(NPC_SHA_TRIGGER, 0.1f, true))
				{
					me->CastSpell(me, SPELL_BOUNDS_OF_REALITY);

					events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, 0, 0, PHASE_BOUNDS_OF_REALITY);
					events.ScheduleEvent(EVENT_SUMMON_FIGMENT_OF_DOUBT, 0, 0, PHASE_BOUNDS_OF_REALITY);
				}

			if (boundsOfReality && !me->HasAura(SPELL_BOUNDS_OF_REALITY))
			{
				me->InterruptSpell(CURRENT_CHANNELED_SPELL);
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

							events.ScheduleEvent(EVENT_WITHER_WILL, 7*IN_MILLISECONDS, 0, PHASE_COMBAT);
							break;

						case EVENT_TOUCH_OF_NOTHINGNESS:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if (target && target->GetTypeId() == TYPEID_PLAYER)
									target->CastSpell(target, SPELL_TOUCH_OF_NOTHINGNESS);

							events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 18*IN_MILLISECONDS, 0, PHASE_COMBAT);
							break;

						case EVENT_MOVE_TO_THE_CENTER:
							if (Creature* trigger = me->FindNearestCreature(NPC_SHA_TRIGGER, 99999.0f, true))
								me->GetMotionMaster()->MovePoint(0, trigger->GetHomePosition());

							events.CancelEvent(EVENT_MOVE_TO_THE_CENTER);
							break;

						case EVENT_BOUNDS_OF_REALITY:
						{
							if (Creature* trigger = me->FindNearestCreature(NPC_SHA_TRIGGER, 99999.0f, true))
							{
								me->Relocate(trigger->GetHomePosition());
								me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
								me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
								me->SetOrientation(4.410300f);
								//me->CastSpell(me, SPELL_BOUNDS_OF_REALITY);
							}

							if (boundsCount == 1)
								seventyFivePct = true;

							if (boundsCount == 2)
								fiftyPct = true;

							events.CancelEvent(EVENT_BOUNDS_OF_REALITY);
							break;
						}

						case EVENT_SUMMON_FIGMENT_OF_DOUBT:
						{
							boundsOfReality = true;

							map = me->GetMap();

							if (map && map->IsDungeon())
							{
								Map::PlayerList const &PlayerList = map->GetPlayers();

								if (!PlayerList.isEmpty())
									for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
										if (player = i->getSource()->ToPlayer())
											player->CastSpell(player, SPELL_FIGMENT_OF_DOUBT);
							}

							events.CancelEvent(EVENT_SUMMON_FIGMENT_OF_DOUBT);
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
		Map* map;
		Unit* player;
		bool emote;

        void Reset()
		{
			events.Reset();

			emote = false;

			me->setActive(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->CastSpell(me, SPELL_SHADOWFORM);
			me->CastSpell(me, SPELL_GATHERING_DOUBT);

			events.ScheduleEvent(EVENT_ATTACK_PLAYERS, 4*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_RELEASE_DOUBT, 30*IN_MILLISECONDS);

			if (instance)
			{
				map = me->GetMap();

				if (map && map->IsDungeon())
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (player = i->getSource()->ToPlayer())
								if (Creature* figment = player->FindNearestCreature(NPC_FIGMENT_OF_DOUBT, 99999.0f, true))
								{
									float x, y, z, o;
									x = player->GetPositionX();
									y = player->GetPositionY();
									z = player->GetPositionZ();
									o = player->GetOrientation();
									figment->Relocate(x, y, z, o);
									//player->CastSpell(figment, SPELL_FIGMENT_OF_DOUBT_CLONE);
									figment->SetDisplayId(player->GetDisplayId());
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
					me->DespawnOrUnsummon(2*IN_MILLISECONDS);

				if (!me->FindNearestCreature(NPC_FIGMENT_OF_DOUBT, 99999.0f, true))
					if (Creature* sha = me->FindNearestCreature(BOSS_SHA_OF_DOUBT, 99999.0f, true))
					{
						sha->InterruptSpell(CURRENT_CHANNELED_SPELL);
						sha->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
						sha->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
						sha->RemoveAurasDueToSpell(SPELL_BOUNDS_OF_REALITY, sha->GetGUID());
						me->DespawnOrUnsummon(2*IN_MILLISECONDS);
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

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					if (instance)
					{
						case EVENT_ATTACK_PLAYERS:
							me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
							me->setFaction(14);
							me->setActive(true);
							me->SetInCombatWithZone();
							me->Attack(player, true);
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

			DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_sha_of_doubt()
{
	new boss_sha_of_doubt();
	new npc_figment_of_doubt();
}