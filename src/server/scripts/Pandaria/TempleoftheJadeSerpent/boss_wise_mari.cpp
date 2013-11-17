/* # Script de Tydrheal & Sungis : Wise Mari # */

/*
	Notes :
	What is missing ? :	- Hydroblast
						- Hydrolance triggers system
						- Water damage
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

	/* Corrupt Living Water */
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
	EVENT_HYDROBLAST				= 10,
	EVENT_FIRST_TRIGGER_WATER_AURA	= 11,
	EVENT_SECOND_TRIGGER_WATER_AURA	= 12,
	EVENT_THIRD_TRIGGER_WATER_AURA	= 13,
	EVENT_FOURTH_TRIGGER_WATER_AURA	= 14
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
	SAY_PHASE_HYDROBLAST	= 9,
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
	PHASE_HYDROBLAST,
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
		bool firstCorruptWater;
		bool secondCorruptWater;
		bool thirdCorruptWater;
		bool fourthCorruptWater;

		void Reset()
		{
			events.Reset();

			if (instance)
			{
				instance->SetBossState(DATA_BOSS_WISE_MARI, NOT_STARTED);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->RemoveAurasDueToSpell(SPELL_WATER_BUBBLE);
				events.SetPhase(PHASE_NULL);

				hydrolanceCount = 0;
				hydrolanceWaterCount = 0;
				firstCorruptWater = false;
				secondCorruptWater = false;
				thirdCorruptWater = false;
				fourthCorruptWater = false;
			}
		}

		void JustDied(Unit *pWho)
		{
			if (instance)
			{
				Talk(irand(SAY_DEATH_1, SAY_DEATH_3));
				instance->DoCastSpellOnPlayers(SPELL_BLESSING_OF_THE_WATERSPEAKER);
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
				instance->SetBossState(DATA_BOSS_WISE_MARI, FAIL);
				me->CombatStop();
				me->DeleteThreatList();
			}
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
			{
				instance->SetBossState(DATA_BOSS_WISE_MARI, IN_PROGRESS);
				me->SetInCombatWithZone();
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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			if (instance)
			{
				Creature* firstTrigger = me->FindNearestCreature(NPC_FIRST_TRIGGER_WATER, 500, true);
				Creature* secondTrigger = me->FindNearestCreature(NPC_SECOND_TRIGGER_WATER, 500, true);
				Creature* thirdTrigger = me->FindNearestCreature(NPC_THIRD_TRIGGER_WATER, 500, true);
				Creature* fourthTrigger = me->FindNearestCreature(NPC_FOURTH_TRIGGER_WATER, 500, true);

				if (hydrolanceCount == 5 && !firstCorruptWater)
				{
					events.ScheduleEvent(EVENT_CALL_SECOND_WATER, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
					firstCorruptWater = true;
					hydrolanceCount = 0;
				}
						
				if (hydrolanceCount == 5 && !secondCorruptWater)
				{
					events.ScheduleEvent(EVENT_CALL_THIRD_WATER, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
					secondCorruptWater = true;
					hydrolanceCount = 0;
				}

				if (hydrolanceCount == 5 && !thirdCorruptWater)
				{
					events.ScheduleEvent(EVENT_CALL_FOURTH_WATER, 2*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
					thirdCorruptWater = true;
					hydrolanceCount = 0;
				}

				if (hydrolanceCount == 5 && !fourthCorruptWater)
				{
					events.SetPhase(PHASE_HYDROBLAST);
					events.ScheduleEvent(EVENT_BUBBLE_BURST, 2*IN_MILLISECONDS, 0, PHASE_HYDROBLAST);
					fourthCorruptWater = true;
					hydrolanceCount = 0;
				}
			
				while(uint32 eventId = events.ExecuteEvent())
				{
					switch(eventId)
					{
						if (instance)
						{
							case EVENT_CALL_FIRST_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								me->CastSpell(firstTrigger, SPELL_CALL_WATER);
								Talk(SAY_CALL_FIRST_WATER);
							
								events.ScheduleEvent(EVENT_FIRST_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_FIRST_WATER);
								break;
									
							case EVENT_FIRST_TRIGGER_WATER_AURA:
								firstTrigger->CastSpell(firstTrigger, SPELL_HYDROLANCE_PRECAST);
									
								events.ScheduleEvent(EVENT_FIRST_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_CALL_SECOND_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								me->CastSpell(secondTrigger, SPELL_CALL_WATER);
								Talk(SAY_CALL_SECOND_WATER);

								events.ScheduleEvent(EVENT_SECOND_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_SECOND_WATER);
								break;

							case EVENT_SECOND_TRIGGER_WATER_AURA:
								secondTrigger->CastSpell(secondTrigger, SPELL_HYDROLANCE_PRECAST);

								events.ScheduleEvent(EVENT_SECOND_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_CALL_THIRD_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								me->CastSpell(thirdTrigger, SPELL_CALL_WATER);
								Talk(SAY_CALL_THIRD_WATER);

								events.ScheduleEvent(EVENT_THIRD_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_THIRD_WATER);
								break;

							case EVENT_THIRD_TRIGGER_WATER_AURA:
								thirdTrigger->CastSpell(thirdTrigger, SPELL_HYDROLANCE_PRECAST);
								
								events.ScheduleEvent(EVENT_THIRD_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_CALL_FOURTH_WATER:
								me->InterruptSpell(CURRENT_GENERIC_SPELL);
								me->CastSpell(fourthTrigger, SPELL_CALL_WATER);
								Talk(SAY_CALL_FOURTH_WATER);
	
								events.ScheduleEvent(EVENT_FOURTH_TRIGGER_WATER_AURA, 0);
								events.CancelEvent(EVENT_CALL_FOURTH_WATER);
								break;
	
							case EVENT_FOURTH_TRIGGER_WATER_AURA:
								fourthTrigger->CastSpell(fourthTrigger, SPELL_HYDROLANCE_PRECAST);

								events.ScheduleEvent(EVENT_FOURTH_TRIGGER_WATER_AURA, 3*IN_MILLISECONDS);
								break;

							case EVENT_HYDROLANCE:
							{
								if (hydrolanceWaterCount == 0) // Front
								{
									me->SetOrientation(1.250952f);
									
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

									events.ScheduleEvent(EVENT_HYDROLANCE_FRONT, 6*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									DoCast(SPELL_HYDROLANCE);
								}

								if (hydrolanceWaterCount == 1) // Left
								{
									me->SetOrientation(5.240993f);
									
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

									events.ScheduleEvent(EVENT_HYDROLANCE_LEFT, 6*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									DoCast(SPELL_HYDROLANCE);
								}

								if (hydrolanceWaterCount == 2) // Right
								{
									me->SetOrientation(3.504827f);
									
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

									events.ScheduleEvent(EVENT_HYDROLANCE_RIGHT, 6*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
									DoCast(SPELL_HYDROLANCE);
								}

								hydrolanceCount + 1;
								events.ScheduleEvent(EVENT_HYDROLANCE, 4*IN_MILLISECONDS, 0, PHASE_CORRUPT_LIVING_WATERS);
								break;
							}

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

								hydrolanceWaterCount = 1;
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
							
								hydrolanceWaterCount = 2;
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

								hydrolanceWaterCount = 0;
								events.CancelEvent(EVENT_HYDROLANCE_RIGHT);
								break;
							}

							case EVENT_BUBBLE_BURST:
								DoCast(SPELL_BUBBLE_BURST);
								Talk(SAY_PHASE_HYDROBLAST);

								events.ScheduleEvent(EVENT_HYDROBLAST, 4*IN_MILLISECONDS, 0, PHASE_HYDROBLAST);
								events.CancelEvent(EVENT_BUBBLE_BURST);
								break;
	
							case EVENT_HYDROBLAST:
								me->RemoveAurasDueToSpell(SPELL_WATER_BUBBLE, me->GetGUID());
								// Cast hydroblast spell

								events.CancelEvent(EVENT_HYDROBLAST);
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

