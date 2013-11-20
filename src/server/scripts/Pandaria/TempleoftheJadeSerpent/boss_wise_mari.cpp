/* # Script de Tydrheal & Sungis : Wise Mari # */

/*
	Notes :
	What is missing ? :	- Wash away (Need to be fixed)
						- Doors (need a test)
						- Water damage (Does not work)
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Wise Mari */
	SPELL_BLESSING_OF_THE_WATERSPEAKER	= 121483,
	SPELL_BUBBLE_BURST					= 106612,
	SPELL_CALL_WATER					= 106526,
	SPELL_HYDROLANCE					= 106055,
	SPELL_PURIFIED_WATER				= 118714,
	SPELL_WATER_BUBBLE					= 106062,
	SPELL_HYDROLANCE_PRECAST			= 115220,
	SPELL_HYDROLANCE_PULSE_BIG			= 106267,
	SPELL_HYDROLANCE_PULSE_SMALL		= 106319,
	SPELL_CORRUPTED_WATERS				= 115165,
	SPELL_WASH_AWAY						= 106331,
	SPELL_WASH_AWAY_VISUAL				= 115575,
	

	/* Corrupt living Water */
	SPELL_SHA_RESIDUE					= 106653
};

enum Npcs
{
	NPC_CORRUPT_LIVING_WATER		= 56511,
	NPC_FIRST_TRIGGER_WATER			= 400430,
	NPC_SECOND_TRIGGER_WATER		= 400431,
	NPC_THIRD_TRIGGER_WATER			= 400432,
	NPC_FOURTH_TRIGGER_WATER		= 400433,
	NPC_CORRUPT_DROPLET				= 56658,
	NPC_HYDROLANCE_TRIGGER_FRONT	= 400434,
	NPC_HYDROLANCE_TRIGGER_LEFT		= 400435,
	NPC_HYDROLANCE_TRIGGER_RIGHT	= 400436,
	NPC_HYDROTRIGGER_ONE_FRONT		= 400438,
	NPC_HYDROTRIGGER_TWO_FRONT		= 400439,
	NPC_HYDROTRIGGER_ONE_LEFT		= 400440,
	NPC_HYDROTRIGGER_TWO_LEFT		= 400441,
	NPC_HYDROTRIGGER_ONE_RIGHT		= 400442,
	NPC_HYDROTRIGGER_TWO_RIGHT		= 400443,
	NPC_WASH_AWAY_TRIGGER			= 400444
};

enum Events
{
	EVENT_HYDROLANCE				= 1,
	EVENT_HYDROLANCE_FRONT			= 2,
	EVENT_HYDROLANCE_LEFT			= 3,
	EVENT_HYDROLANCE_RIGHT			= 4,
	EVENT_CALL_FIRST_WATER			= 5,
	EVENT_CALL_SECOND_WATER			= 6,
	EVENT_CALL_THIRD_WATER			= 7,
	EVENT_CALL_FOURTH_WATER			= 8,
	EVENT_BUBBLE_BURST				= 9,
	EVENT_WASH_AWAY					= 10,
	EVENT_FIRST_TRIGGER_WATER_AURA	= 11,
	EVENT_SECOND_TRIGGER_WATER_AURA	= 12,
	EVENT_THIRD_TRIGGER_WATER_AURA	= 13,
	EVENT_FOURTH_TRIGGER_WATER_AURA	= 14,
	EVENT_HYDROTRIGGER_ONE_FRONT	= 15,
	EVENT_HYDROTRIGGER_TWO_FRONT	= 16,
	EVENT_HYDROTRIGGER_ONE_LEFT		= 17,
	EVENT_HYDROTRIGGER_TWO_LEFT		= 18,
	EVENT_HYDROTRIGGER_ONE_RIGHT	= 19,
	EVENT_HYDROTRIGGER_TWO_RIGHT	= 20,
	EVENT_SAY_TAUNT					= 21,
	EVENT_WASH_AWAY_TURN			= 22
};

enum Texts
{
	SAY_AGGRO				= 0,
	SAY_CALL_FIRST_WATER	= 1,
	SAY_CALL_SECOND_WATER	= 2,
	SAY_CALL_THIRD_WATER	= 3,
	SAY_CALL_FOURTH_WATER	= 4,
	SAY_DEATH_1				= 5,
	SAY_DEATH_2				= 6,
	SAY_DEATH_3				= 7,
	SAY_INTRO				= 8,
	SAY_PHASE_WASH_AWAY		= 9,
	SAY_SLAY_1				= 10,
	SAY_SLAY_2				= 11,
	SAY_TAUNT_1				= 12,
	SAY_TAUNT_2				= 13,
	SAY_TAUNT_3				= 14
};

enum Phases
{
	PHASE_NULL,
	PHASE_CORRUPT_LIVING_WATERS,
	PHASE_WASH_AWAY,
};


class boss_wise_mari : public CreatureScript
{
public:
	boss_wise_mari() : CreatureScript("boss_wise_mari") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new boss_wise_mariAI(creature);
	}

	struct boss_wise_mariAI : public ScriptedAI
	{
		boss_wise_mariAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		int32 hydrolanceCount;
		int32 hydrolanceWaterCount;
		int32 corruptWaterCount;

		void Reset()
		{
			events.Reset();

			if (instance)
			{
				instance->SetBossState(DATA_BOSS_WISE_MARI, NOT_STARTED);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

				hydrolanceCount = 0; // 0 = front | 1 = left | 2 = right
				hydrolanceWaterCount = 0; // Number of hydrolances casted before the new Corrupt living Water appears (5)
				corruptWaterCount = 0; // 0 = first corrupt living water | 1 = second | 2 = third | 3 = fourth & phase 2

				events.SetPhase(PHASE_NULL);
			}
		}

		void JustDied(Unit *pWho)
		{
			if (instance)
			{
				Talk(irand(SAY_DEATH_1, SAY_DEATH_3));
				instance->DoCastSpellOnPlayers(SPELL_BLESSING_OF_THE_WATERSPEAKER);
				if (GameObject* go = GameObject::GetGameObject(*me, instance->GetData64(DATA_WISE_MARI_GATE)))
                        instance->HandleGameObject(go->GetGUID(), false);
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
				if (me->HasAura(SPELL_WATER_BUBBLE))
					me->RemoveAurasDueToSpell(SPELL_WATER_BUBBLE);
				if (me->HasAura(SPELL_WASH_AWAY))
					me->RemoveAurasDueToSpell(SPELL_WASH_AWAY);
				if (me->HasAura(SPELL_WASH_AWAY_VISUAL))
					me->RemoveAurasDueToSpell(SPELL_WASH_AWAY_VISUAL);

				me->HandleEmoteCommand(0); // Remove emote
				me->SetFacingTo(1.250952f);
				me->GetMotionMaster()->MoveIdle();
				me->CombatStop();
				me->DeleteThreatList();
				instance->SetBossState(DATA_BOSS_WISE_MARI, FAIL);
			}
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_WISE_MARI, IN_PROGRESS);
				me->SetInCombatWithZone();
				me->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_01);
				DoCast(SPELL_WATER_BUBBLE);
				Talk(SAY_AGGRO);

				events.SetPhase(PHASE_CORRUPT_LIVING_WATERS);
				events.ScheduleEvent(EVENT_HYDROLANCE, 0, 0, PHASE_CORRUPT_LIVING_WATERS);
				events.ScheduleEvent(EVENT_CALL_FIRST_WATER, 8*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
			}
		}

		void UpdateAI(uint32 diff) 
		{
			if(!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING) && !events.IsInPhase(PHASE_WASH_AWAY))
				return;

			events.Update(diff);

			Map* map = me->GetMap();
			if (map && map->IsDungeon())
			{
				Map::PlayerList const &PlayerList = map->GetPlayers();

				if (!PlayerList.isEmpty())
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (i->getSource()->isAlive() && i->getSource()->IsInWater() && !i->getSource()->HasAura(SPELL_CORRUPTED_WATERS))
							i->getSource()->CastSpell(i->getSource(), SPELL_CORRUPTED_WATERS);

						if (i->getSource()->isAlive() && !i->getSource()->IsInWater() && i->getSource()->HasAura(SPELL_CORRUPTED_WATERS))
							i->getSource()->RemoveAurasDueToSpell(SPELL_CORRUPTED_WATERS);
					}
			}

			if (instance)
			{
				if (hydrolanceWaterCount == 5 && corruptWaterCount == 0) // First corrupt living water
				{
					events.ScheduleEvent(EVENT_CALL_SECOND_WATER, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
					hydrolanceWaterCount = 0;
				}
						
				if (hydrolanceWaterCount == 5 && corruptWaterCount == 1) // Second corrupt living water
				{
					events.ScheduleEvent(EVENT_CALL_THIRD_WATER, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
					hydrolanceWaterCount = 0;
				}

				if (hydrolanceWaterCount == 5 && corruptWaterCount == 2) // Third corrupt living water
				{
					events.ScheduleEvent(EVENT_CALL_FOURTH_WATER, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
					hydrolanceWaterCount = 0;
				}

				if (hydrolanceWaterCount == 5 && corruptWaterCount == 3) // Fourth corrupt living water
				{
					events.SetPhase(PHASE_WASH_AWAY);
					events.ScheduleEvent(EVENT_BUBBLE_BURST, 3*IN_MILLISECONDS, 0, PHASE_WASH_AWAY);
					hydrolanceWaterCount = 0;
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
						if (instance)
						{
							case EVENT_CALL_FIRST_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								if (Creature* firstTrigger = me->FindNearestCreature(NPC_FIRST_TRIGGER_WATER, 500, true))
									me->CastSpell(firstTrigger, SPELL_CALL_WATER);

								Talk(SAY_CALL_FIRST_WATER);

								events.ScheduleEvent(EVENT_FIRST_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_FIRST_WATER);
								break;
									
							case EVENT_FIRST_TRIGGER_WATER_AURA:
								if (Creature* firstTrigger = me->FindNearestCreature(NPC_FIRST_TRIGGER_WATER, 500, true))
									firstTrigger->CastSpell(firstTrigger, SPELL_HYDROLANCE_PRECAST);
									
								events.ScheduleEvent(EVENT_FIRST_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_CALL_SECOND_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								if (Creature* secondTrigger = me->FindNearestCreature(NPC_SECOND_TRIGGER_WATER, 500, true))
									me->CastSpell(secondTrigger, SPELL_CALL_WATER);

								Talk(SAY_CALL_SECOND_WATER);
								corruptWaterCount = 1;

								events.ScheduleEvent(EVENT_SECOND_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_SECOND_WATER);
								break;

							case EVENT_SECOND_TRIGGER_WATER_AURA:
								if (Creature* secondTrigger = me->FindNearestCreature(NPC_SECOND_TRIGGER_WATER, 500, true))
									secondTrigger->CastSpell(secondTrigger, SPELL_HYDROLANCE_PRECAST);

								events.ScheduleEvent(EVENT_SECOND_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_CALL_THIRD_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								if (Creature* thirdTrigger = me->FindNearestCreature(NPC_THIRD_TRIGGER_WATER, 500, true))
									me->CastSpell(thirdTrigger, SPELL_CALL_WATER);

								Talk(SAY_CALL_THIRD_WATER);
								corruptWaterCount = 2;

								events.ScheduleEvent(EVENT_THIRD_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_THIRD_WATER);
								break;

							case EVENT_THIRD_TRIGGER_WATER_AURA:
								if (Creature* thirdTrigger = me->FindNearestCreature(NPC_THIRD_TRIGGER_WATER, 500, true))
									thirdTrigger->CastSpell(thirdTrigger, SPELL_HYDROLANCE_PRECAST);
								
								events.ScheduleEvent(EVENT_THIRD_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_CALL_FOURTH_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								if (Creature* fourthTrigger = me->FindNearestCreature(NPC_FOURTH_TRIGGER_WATER, 500, true))
									me->CastSpell(fourthTrigger, SPELL_CALL_WATER);

								Talk(SAY_CALL_FOURTH_WATER);
								corruptWaterCount = 3;

								events.ScheduleEvent(EVENT_FOURTH_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_FOURTH_WATER);
								break;
	
							case EVENT_FOURTH_TRIGGER_WATER_AURA:
								if (Creature* fourthTrigger = me->FindNearestCreature(NPC_FOURTH_TRIGGER_WATER, 500, true))
									fourthTrigger->CastSpell(fourthTrigger, SPELL_HYDROLANCE_PRECAST);

								events.ScheduleEvent(EVENT_FOURTH_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_HYDROLANCE:
							{
								if (hydrolanceCount == 0) // Front
								{
									me->SetFacingTo(1.250952f);

									std::list<Creature*> hydrolanceTriggers;
						            me->GetCreatureListWithEntryInGrid(hydrolanceTriggers, NPC_HYDROLANCE_TRIGGER_FRONT, 500.0f);
							        if (!hydrolanceTriggers.empty())
								    {
									    for (std::list<Creature*>::iterator itr = hydrolanceTriggers.begin(); itr != hydrolanceTriggers.end(); ++itr)
										{
											if ((*itr)->isAlive())
												me->CastSpell((*itr), SPELL_HYDROLANCE_PRECAST);
										}
									}

									events.ScheduleEvent(EVENT_HYDROTRIGGER_ONE_FRONT, 1*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									events.ScheduleEvent(EVENT_HYDROTRIGGER_TWO_FRONT, 1500, 0, PHASE_CORRUPT_LIVING_WATERS);
									events.ScheduleEvent(EVENT_HYDROLANCE_FRONT, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									DoCast(SPELL_HYDROLANCE);
								}

								if (hydrolanceCount == 1) // Left
								{
									me->SetFacingTo(5.240993f);
									
									std::list<Creature*> hydrolanceTriggers;
						            me->GetCreatureListWithEntryInGrid(hydrolanceTriggers, NPC_HYDROLANCE_TRIGGER_LEFT, 500.0f);
							        if (!hydrolanceTriggers.empty())
								    {
									    for (std::list<Creature*>::iterator itr = hydrolanceTriggers.begin(); itr != hydrolanceTriggers.end(); ++itr)
										{
											if ((*itr)->isAlive())
												me->CastSpell((*itr), SPELL_HYDROLANCE_PRECAST);
										}
									}

									events.ScheduleEvent(EVENT_HYDROTRIGGER_ONE_LEFT, 1*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									events.ScheduleEvent(EVENT_HYDROTRIGGER_TWO_LEFT, 1500, 0, PHASE_CORRUPT_LIVING_WATERS);
									events.ScheduleEvent(EVENT_HYDROLANCE_LEFT, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									DoCast(SPELL_HYDROLANCE);
								}

								if (hydrolanceCount == 2) // Right
								{
									me->SetFacingTo(3.504827f);
									
									std::list<Creature*> hydrolanceTriggers;
						            me->GetCreatureListWithEntryInGrid(hydrolanceTriggers, NPC_HYDROLANCE_TRIGGER_RIGHT, 500.0f);
							        if (!hydrolanceTriggers.empty())
								    {
									    for (std::list<Creature*>::iterator itr = hydrolanceTriggers.begin(); itr != hydrolanceTriggers.end(); ++itr)
										{
											if ((*itr)->isAlive())
												me->CastSpell((*itr), SPELL_HYDROLANCE_PRECAST);
										}
									}

									events.ScheduleEvent(EVENT_HYDROTRIGGER_ONE_RIGHT, 1*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									events.ScheduleEvent(EVENT_HYDROTRIGGER_TWO_RIGHT, 1500, 0, PHASE_CORRUPT_LIVING_WATERS);
									events.ScheduleEvent(EVENT_HYDROLANCE_RIGHT, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									DoCast(SPELL_HYDROLANCE);
								}

								hydrolanceWaterCount++;
								events.ScheduleEvent(EVENT_HYDROLANCE, 4*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
								break;
							}

							case EVENT_HYDROTRIGGER_ONE_FRONT:
								if (Creature* hydroTriggerOneFront = me->FindNearestCreature(NPC_HYDROTRIGGER_ONE_FRONT, 500, true))
									hydroTriggerOneFront->CastSpell(hydroTriggerOneFront, SPELL_HYDROLANCE_PULSE_SMALL);

								events.CancelEvent(EVENT_HYDROTRIGGER_ONE_FRONT);
								break;

							case EVENT_HYDROTRIGGER_TWO_FRONT:
								if (Creature* hydroTriggerTwoFront = me->FindNearestCreature(NPC_HYDROTRIGGER_TWO_FRONT, 500, true))
									hydroTriggerTwoFront->CastSpell(hydroTriggerTwoFront, SPELL_HYDROLANCE_PULSE_SMALL);

								events.CancelEvent(EVENT_HYDROTRIGGER_TWO_FRONT);
								break;

							case EVENT_HYDROTRIGGER_ONE_LEFT:
								if (Creature* hydroTriggerOneLeft = me->FindNearestCreature(NPC_HYDROTRIGGER_ONE_LEFT, 500, true))
									hydroTriggerOneLeft->CastSpell(hydroTriggerOneLeft, SPELL_HYDROLANCE_PULSE_SMALL);

								events.CancelEvent(EVENT_HYDROTRIGGER_ONE_LEFT);
								break;
								
							case EVENT_HYDROTRIGGER_TWO_LEFT:
								if (Creature* hydroTriggerTwoLeft = me->FindNearestCreature(NPC_HYDROTRIGGER_TWO_LEFT, 500, true))
									hydroTriggerTwoLeft->CastSpell(hydroTriggerTwoLeft, SPELL_HYDROLANCE_PULSE_SMALL);

								events.CancelEvent(EVENT_HYDROTRIGGER_TWO_LEFT);
								break;

							case EVENT_HYDROTRIGGER_ONE_RIGHT:
								if (Creature* hydroTriggerOneRight = me->FindNearestCreature(NPC_HYDROTRIGGER_ONE_RIGHT, 500, true))
									hydroTriggerOneRight->CastSpell(hydroTriggerOneRight, SPELL_HYDROLANCE_PULSE_SMALL);

								events.CancelEvent(EVENT_HYDROTRIGGER_ONE_RIGHT);
								break;

							case EVENT_HYDROTRIGGER_TWO_RIGHT:
								if (Creature* hydroTriggerTwoRight = me->FindNearestCreature(NPC_HYDROTRIGGER_TWO_RIGHT, 500, true))
									hydroTriggerTwoRight->CastSpell(hydroTriggerTwoRight, SPELL_HYDROLANCE_PULSE_SMALL);

								events.CancelEvent(EVENT_HYDROTRIGGER_TWO_RIGHT);
								break;

							case EVENT_HYDROLANCE_FRONT:
							{
								std::list<Creature*> hydrolanceTriggers;
								me->GetCreatureListWithEntryInGrid(hydrolanceTriggers, NPC_HYDROLANCE_TRIGGER_FRONT, 500.0f);
								if (!hydrolanceTriggers.empty())
								{
									for (std::list<Creature*>::iterator itr = hydrolanceTriggers.begin(); itr != hydrolanceTriggers.end(); ++itr)
									{
										if ((*itr)->isAlive())
											(*itr)->CastSpell((*itr), SPELL_HYDROLANCE_PULSE_BIG);
									}
								}

								hydrolanceCount = 1; // Next => left
								events.CancelEvent(EVENT_HYDROLANCE_FRONT);
								break;
							}

							case EVENT_HYDROLANCE_LEFT:
							{
								std::list<Creature*> hydrolanceTriggers;
								me->GetCreatureListWithEntryInGrid(hydrolanceTriggers, NPC_HYDROLANCE_TRIGGER_LEFT, 500.0f);
								if (!hydrolanceTriggers.empty())
								{
									for (std::list<Creature*>::iterator itr = hydrolanceTriggers.begin(); itr != hydrolanceTriggers.end(); ++itr)
									{
										if ((*itr)->isAlive())
											(*itr)->CastSpell((*itr), SPELL_HYDROLANCE_PULSE_BIG);
									}
								}
							
								hydrolanceCount = 2; // Next => right
								events.CancelEvent(EVENT_HYDROLANCE_LEFT);
								break;
							}

							case EVENT_HYDROLANCE_RIGHT:
							{
								std::list<Creature*> hydrolanceTriggers;
								me->GetCreatureListWithEntryInGrid(hydrolanceTriggers, NPC_HYDROLANCE_TRIGGER_RIGHT, 500.0f);
								if (!hydrolanceTriggers.empty())
								{
									for (std::list<Creature*>::iterator itr = hydrolanceTriggers.begin(); itr != hydrolanceTriggers.end(); ++itr)
									{
										if ((*itr)->isAlive())
											(*itr)->CastSpell((*itr), SPELL_HYDROLANCE_PULSE_BIG);
									}
								}

								hydrolanceCount = 0; // Next => front
								events.CancelEvent(EVENT_HYDROLANCE_RIGHT);
								break;
							}

							case EVENT_BUBBLE_BURST:
								DoCast(SPELL_BUBBLE_BURST);
								Talk(SAY_PHASE_WASH_AWAY);

								events.ScheduleEvent(EVENT_WASH_AWAY, 4*IN_MILLISECONDS, 0, PHASE_WASH_AWAY);
								events.CancelEvent(EVENT_BUBBLE_BURST);
								break;
	
							case EVENT_WASH_AWAY:
								me->RemoveAurasDueToSpell(SPELL_WATER_BUBBLE, me->GetGUID());
								me->SetFacingTo(4.393149f);
								DoCast(SPELL_WASH_AWAY_VISUAL);
								if (Creature* washAwayTrigger = me->FindNearestCreature(NPC_WASH_AWAY_TRIGGER, 500, true))
									DoCast(washAwayTrigger, SPELL_WASH_AWAY, true);

								events.ScheduleEvent(EVENT_SAY_TAUNT, 18*IN_MILLISECONDS, 0, PHASE_WASH_AWAY);
								events.ScheduleEvent(EVENT_WASH_AWAY_TURN, 0, 0, PHASE_WASH_AWAY);
								events.CancelEvent(EVENT_WASH_AWAY);
								break;

							case EVENT_SAY_TAUNT:
								Talk(irand(SAY_TAUNT_1, SAY_TAUNT_3));

								events.ScheduleEvent(EVENT_SAY_TAUNT, 18*IN_MILLISECONDS, 0, PHASE_WASH_AWAY);
								break;

							case EVENT_WASH_AWAY_TURN:
								if (Creature* washAwayTrigger = me->FindNearestCreature(NPC_WASH_AWAY_TRIGGER, 500, true))
									me->SetFacingToObject(washAwayTrigger);

								events.ScheduleEvent(EVENT_WASH_AWAY_TURN, 1, 0, PHASE_WASH_AWAY);
								break;
						}

						default:
								break;
					}
				}
			}
		}
	};
};

class npc_corrupt_living_water : public CreatureScript 
{
public:
	npc_corrupt_living_water() : CreatureScript("npc_corrupt_living_water") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_corrupt_living_waterAI(creature);
	}

	struct npc_corrupt_living_waterAI : public ScriptedAI
	{
		npc_corrupt_living_waterAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset() 
		{
			me->SetInCombatWithZone();
		}

		void EnterEvadeMode() 
		{
			me->DespawnOrUnsummon();
		}

		void JustDied(Unit *pWho) 
		{
			DoCast(SPELL_SHA_RESIDUE);

			float x = me->GetPositionX();
			float y = me->GetPositionY();
			float z = me->GetPositionZ();
			float o = me->GetOrientation();

			me->SummonCreature(NPC_CORRUPT_DROPLET, (x + 2.0f), y, z, o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60*IN_MILLISECONDS);
			me->SummonCreature(NPC_CORRUPT_DROPLET, (x - 2.0f), y, z, o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60*IN_MILLISECONDS);
			me->SummonCreature(NPC_CORRUPT_DROPLET, x, (y - 2.0f), z, o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 diff) 
		{
			if(!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
	};
};


void AddSC_boss_wise_mari()
{
	new boss_wise_mari();
	new npc_corrupt_living_water();
}

