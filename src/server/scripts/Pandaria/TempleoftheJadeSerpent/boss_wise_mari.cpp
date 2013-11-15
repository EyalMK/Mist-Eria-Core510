/* # Script de Tydrheal : Wise Mari # */

/* Notes : Tester -- verifier les spells -- SoundID
Ajouter l'ID du NPC_TRIGGER

Wise Mari : Script 95% verifier spells
Wise Mari Trigger : Script 100%	
Corrupt Living Water : Script 95% verifier spells	
*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Wise Mari */
	SPELL_BLESSING_OF_THE_WATERSPEAKER	= 121483,
	SPELL_CORRUPTED_WATERS				= 115167,
	SPELL_WATER_BUBBLE					= 106062,
	SPELL_CALL_WATER					= 106462,
	SPELL_BUBBLE_BURST					= 106612,
	SPELL_WASH_AWAY						= 106334,
	
	/* Corrupt Living Water */
	SPELL_SHA_RESIDUE = 106653
};

enum Events
{
	EVENT_WATER_BUBBLE,
	EVENT_PHASE_2,
	EVENT_CORRUPTED_WATERS,
	EVENT_CALL_WATER,
	EVENT_AFTER_PHASE_2,
	EVENT_SAY_DEATH_1,
	EVENT_SAY_DEATH_2,
	EVENT_DEATH
};

enum Actions
{
	ACTION_BOSS_WISE_MARI_RESET,
	ACTION_BOSS_WISE_MARI_DIED,
	ACTION_WISE_MARI_INTRO
};

enum Texts
{
	SAY_INTRO_1 = 0,
	SAY_COMBAT = 1,
	SAY_SPAWN = 2,
	SAY_SPAWN_2 = 3,
	SAY_SPAWN_3 = 4,
	SAY_SPAWN_4 = 5,
	SAY_AFTER_PHASE_2 = 6,
	SAY_MIDDLE_LIFE = 7,
	SAY_21_PER_CENT = 4,
	SAY_DEATH_1 = 8,
	SAY_DEATH_2 = 9,
	SAY_SLAY_1 = 10,
	SAY_SLAY_2 = 11,
	SAY_EVADE = 12
};

enum spawnIds
{
	CORRUPT_LIVING_WATER = 59873,
	CORRUPT_DROPLET = 62360,
	NPC_TRIGGER = 56449
};

enum Phases
{
	PHASE_1,
	PHASE_2,
	PHASE_DEATH
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
		
		bool checkWiseMariAlive;
		bool checkSaySpawn2;
		bool checkSaySpawn3;
		bool checkSaySpawn4;
		bool checkMiddleLife;
		bool checkHealthBelow21;
		int counterLivingWater;
		
		void Reset() 
		{
			checkHealthBelow21 = false;
			checkMiddleLife = false;
			checkSaySpawn2 = false;
			checkSaySpawn3 = false;
			checkSaySpawn4 = false;
			counterLivingWater = 0;
			checkWiseMariAlive = true;
			checkWiseMariAlive = me->isAlive();
			
			events.Reset();

			if (instance)
			{
				if(Creature *triggers = me->FindNearestCreature(NPC_TRIGGER, 50000.0f))
					if(triggers->AI())
					   triggers->AI()->Reset();
				instance->SetBossState(DATA_BOSS_WISE_MARI, NOT_STARTED);
				me->AI()->DoAction(ACTION_BOSS_WISE_MARI_RESET);
			}
			
		}

		void DoAction(int32 action) 
        {
            switch (action)
            {
				case ACTION_BOSS_WISE_MARI_RESET:
					checkWiseMariAlive = true;
					break;
				case ACTION_BOSS_WISE_MARI_DIED:
					checkWiseMariAlive = false;
					break;
				case ACTION_WISE_MARI_INTRO:
					Talk(SAY_INTRO_1);
					break;
				
			}
        }

		void JustDied(Unit *pWho) 
		{
			if (instance)
			{
				me->AI()->DoAction(ACTION_BOSS_WISE_MARI_DIED);

				if (!checkWiseMariAlive)
					instance->SetBossState(DATA_BOSS_WISE_MARI, DONE);

			}
		
		}

		void KilledUnit(Unit *pWho) 
		{	
			Talk(urand(SAY_SLAY_1, SAY_SLAY_2));
		}
		
		void EnterEvadeMode() 
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_WISE_MARI, FAIL);	
			
			Talk(SAY_EVADE);
		}

		void EnterCombat(Unit* /*who*/) 
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_WISE_MARI, IN_PROGRESS);

			me->SetInCombatWithZone();
			Talk(SAY_COMBAT);
			events.SetPhase(PHASE_1);
			events.ScheduleEvent(EVENT_WATER_BUBBLE, 0, 0, PHASE_1);
			events.ScheduleEvent(EVENT_CORRUPTED_WATERS, 0, 0, PHASE_1);
			events.ScheduleEvent(EVENT_CALL_WATER, 15*IN_MILLISECONDS, 0, PHASE_1);
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
						case EVENT_DEATH:
							me->Kill(me);
							events.CancelEvent(EVENT_DEATH);
							break;

						case EVENT_SAY_DEATH_1:
							me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
							me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
							Talk(SAY_DEATH_1);
							events.ScheduleEvent(EVENT_SAY_DEATH_2, 8*IN_MILLISECONDS, 0, PHASE_DEATH);
							events.CancelEvent(EVENT_SAY_DEATH_1);
							break;

						case EVENT_SAY_DEATH_2:
							Talk(SAY_DEATH_2);
							events.ScheduleEvent(EVENT_DEATH, 3*IN_MILLISECONDS, 0, PHASE_DEATH);
							DoCastToAllHostilePlayers(SPELL_BLESSING_OF_THE_WATERSPEAKER);
							events.CancelEvent(EVENT_SAY_DEATH_2);
							break;

						case EVENT_WATER_BUBBLE:
							DoCast(me, SPELL_WATER_BUBBLE);
							events.CancelEvent(EVENT_WATER_BUBBLE);
							break;
							
						case EVENT_PHASE_2:
							DoCast(me, SPELL_BUBBLE_BURST);
							DoCast(me, SPELL_WASH_AWAY);
							events.ScheduleEvent(EVENT_AFTER_PHASE_2, 10*IN_MILLISECONDS, 0, PHASE_2);
							events.CancelEvent(EVENT_PHASE_2);
							break;

						case EVENT_CORRUPTED_WATERS:
							DoCast(me, SPELL_CORRUPTED_WATERS); //voir il faudra surement modifier ce sort pour qu'il change de fontaine
							events.ScheduleEvent(EVENT_CORRUPTED_WATERS, 8*IN_MILLISECONDS, 0, PHASE_1);
							break;

						case EVENT_CALL_WATER:
							if(counterLivingWater < 2)
								Talk(SAY_SPAWN);
							counterLivingWater++;
							float x,y,z;
							me->GetPosition(x,y,z);
							me->SummonCreature(CORRUPT_LIVING_WATER, (x, y, z), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60*IN_MILLISECONDS);
							events.ScheduleEvent(EVENT_CORRUPTED_WATERS, 25*IN_MILLISECONDS, 0, PHASE_1);
							break;

						case EVENT_AFTER_PHASE_2:
							Talk(SAY_AFTER_PHASE_2);
							events.CancelEvent(EVENT_AFTER_PHASE_2);
							break;
							
						default:
							break;
					}
				}
			}

			if(counterLivingWater == 2 && !checkSaySpawn2)
			{
				Talk(SAY_SPAWN_2);
				checkSaySpawn2 = true;
			}

			if(counterLivingWater == 3 && !checkSaySpawn3)
			{
				Talk(SAY_SPAWN_3);
				checkSaySpawn3 = true;
			}

			if(counterLivingWater == 4 && !checkSaySpawn4)
			{
				Talk(SAY_SPAWN_4);
				events.SetPhase(PHASE_2);
				events.ScheduleEvent(EVENT_PHASE_2, 5*IN_MILLISECONDS, 0, PHASE_2);
				checkSaySpawn4 = true;
			}

			if(me->HealthBelowPct(50) && !checkMiddleLife)
			{
				Talk(SAY_MIDDLE_LIFE);
				checkMiddleLife = true;
			}

			if(me->HealthBelowPct(21) && !checkHealthBelow21)
			{
				Talk(SAY_21_PER_CENT);
				checkHealthBelow21 = true;
			}

			if(me->HealthBelowPct(3) && events.IsInPhase(PHASE_2))
			{
				events.ScheduleEvent(EVENT_SAY_DEATH_1, 0, 0, PHASE_DEATH);
				events.SetPhase(PHASE_DEATH);
			}

			if(!events.IsInPhase(PHASE_DEATH))
			{
				DoMeleeAttackIfReady();
			}
		}
	};
};

class npc_wise_intro_trigger : public CreatureScript 
{
public:
	npc_wise_intro_trigger() : CreatureScript("npc_wise_intro_trigger") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_wise_intro_triggerAI(creature);
	}

	struct npc_wise_intro_triggerAI : public ScriptedAI
	{
		npc_wise_intro_triggerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkTrigger; 

		void Reset() 
		{
			checkTrigger = true;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED); 
			//ajouter le flag pour qu'il soit invisible => Modifier dans la db pour la visibilite du PNJ par les joueurs.
		}

		void JustDied(Unit *pWho) 
		{

		}

		void EnterCombat(Unit* /*who*/) 
		{

		}

		void UpdateAI(uint32 diff) 
		{	
			if(checkTrigger)
			{
				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				if (!PlayerList.isEmpty())
				{
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (me->GetExactDist2d(i->getSource()->GetPositionX(),i->getSource()->GetPositionY()) < 12) // A tester si cela fonctionne.
						{
							if (instance)
								if (Creature* wise = me->GetCreature(*me, instance->GetData64(DATA_BOSS_WISE_MARI)))
									if (wise->AI())
									{
										wise->AI()->DoAction(ACTION_WISE_MARI_INTRO);
										checkTrigger = false;
									}
						}
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
		
		}

		void JustDied(Unit *pWho) 
		{
			DoCast(me, SPELL_SHA_RESIDUE);
			float x,y,z;
			me->SummonCreature(CORRUPT_DROPLET, (x, y, z), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60*IN_MILLISECONDS);
			me->SummonCreature(CORRUPT_DROPLET, (x, y, z), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60*IN_MILLISECONDS);
			me->SummonCreature(CORRUPT_DROPLET, (x, y, z), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60*IN_MILLISECONDS);
		}

		void EnterCombat(Unit* /*who*/) 
		{

		}

		void UpdateAI(uint32 diff) 
		{	
			
		}
	};
};


void AddSC_boss_wise_mari()
{
	new boss_wise_mari();
	new npc_wise_intro_trigger();
	new npc_corrupt_living_water();
}

