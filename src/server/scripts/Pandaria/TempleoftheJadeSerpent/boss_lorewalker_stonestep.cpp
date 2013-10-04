/* # Script de Tydrheal : Lorewalker Stonestep # */

/* Notes : Tester -- voir spells -- SoundID

AJOUTER LES ID pour les TRIGGERS

Lorewalker Stonestep : Script 99% 
Npc Zao Sunseeker : Script 99%
Npc Sun: Script 99%
Npc Haunting Sha : Script 99%
Lorewalker Stonestep trigger : Script 99% A test -- ajouter le flag invisbilité dans la DB
Lorewalker Stonestep trigger 2 : Script 99% A test -- ajouter le flag invisibilité dans la DB
Lorewalker Stonestep trigger 3 : Script 99% A test -- ajouter le flag invisibilité dans la DB
Lorewalker Stonestep trigger 4 : Script 99% A test -- ajouter le flag invisibilité dans la DB
Corrupted Scroll : Script 100% A test


UPDATE creature_template SET ScriptName = 'boss_lorewalker_stonestep' WHERE entry = 56843;
UDAPTE creature_template SET ScriptName = 'npc_zao_sunseeker' WHERE entry = 58826;
UDAPTE creature_template SET ScriptName = 'npc_sun' WHERE entry = 56915;
UDAPTE creature_template SET ScriptName = 'npc_hauting_sha' WHERE entry = 59555;
UDAPTE creature_template SET ScriptName = 'npc_haunting_sha_transition' WHERE entry = ?????;
UDAPTE creature_template SET ScriptName = 'npc_lorewalker_intro_trigger' WHERE entry = 57080;
UDAPTE creature_template SET ScriptName = 'npc_lorewalker_intro_trigger_2' WHERE entry = ?????;
UDAPTE creature_template SET ScriptName = 'npc_lorewalker_intro_trigger_3' WHERE entry = ?????;
UDAPTE creature_template SET ScriptName = 'npc_lorewalker_intro_trigger_4' WHERE entry = ?????;
UDAPTE creature_template SET ScriptName = 'npc_lorewalker_intro_trigger_5' WHERE entry = ?????;

INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(56843, 0, 0, "Vous, là ! Sur la galerie ! Ma bibliothèque sacrée est possédée par les sha !", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro"),
(56843, 1, 0, "Regardez-les se répandre par chacune des lettres de ces parchemins sacrés… ils les corrompent ! Ils les souillent !", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro2"),
(56843, 2, 0, "Je vous en conjure, étrangers. Purifiez cet endroit de leur présence impure ! Nos écrits doivent trouver le repos !", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro3"),
(56843, 3, 0, "Vous devez détruire la source de la corruption ! Vite, avant que l'histoire qui nous est chère ne soit effacée à tout jamais !", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro4"),
(56843, 4, 0, "Ça, alors. Si je ne me trompe pas, le conte de Zao Cherche Soleil prend vie sous nos yeux.", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro5"),
(56843, 5, 0, "Selon la légende, il y a bien longtemps, cinq soleils brillaient dans le ciel. Lorsque le vent soufflait, la chaleur intense qu’ils généraient provoquait de grands feux de forêt.", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro6"),
(56843, 6, 0, "Les récoltes se flétrissaient, les villages brûlaient, et les familles erraient sans cesse à la recherche d’un abri contre les flammes…", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Intro7"),
(56843, 7, 0, "Le feu... La bibliothèque va brûler ! Je vous en prie, héros ! Purgez ce mal avant que le temple ne soit réduit en cendres !", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - AfterBeginCombat"),
(56843, 8, 0, "Zao aussi est corrompu ! S’il vous plaît, combattez-le dignement…", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - Phase2"),
(56843, 9, 0, "Je vous remercie, héros. Vous avez apporté la paix à ce temple sacré.", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - DeathZao");
(56843, 10, 0, "Maintenant, partez ! J’ai besoin de méditer avant d’entamer le rituel de restauration.", 14, 0, 100, 0, 0, ??????, "LorewalkerStonestep - AfetrDeathZao");


INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(58826, 0, 0, "Je ne le permettrai plus !", 14, 0, 100, 0, 0, ??????, "ZaoSunseeker - IntroDebutSun");
(58826, 1, 0, "Cette chaleur torride a dû émousser mes sens… Vous n’êtes tous que des mirages !", 14, 0, 100, 0, 0, ??????, "ZaoSunseeker - Phase2");

*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/*Sun*/
	SPELL_SUNFIRE_RAYS = 107223,

	/*Zao Sunseeker*/
	SPELL_HELLFIRE_ARROWS = 113017,
	SPELL_LOREWALKERS_ALACRITY = 122714,

	/*Haunting Sha*/
	SPELL_HAUNTING_GAZE = 114646
};

enum Events
{
	/* Lorewalker Stonestep */
	EVENT_INTRO_6,
	EVENT_INTRO_7,
	EVENT_COMBAT_ZAO,
	EVENT_AFTER_COMBAT,
	EVENT_AFTER_DEATH_ZAO,

	/* Zao Sunseeker */
	EVENT_BEGIN_COMBAT,
	EVENT_PHASE_2,
	EVENT_HELLFIRE_ARROWS,

	/* Sun */
	EVENT_SUNFIRE_RAYS,

	/*Haunting Sha*/
	EVENT_HAUNTING_GAZE

};

enum Actions
{
	/* Lorewalker Stonestep */
	ACTION_BOSS_LOREWALKER_STONESTEP_RESET,
	ACTION_BOSS_LOREWALKER_STONESTEP_DIED,
	ACTION_LOREWALKER_INTRO,
	ACTION_LOREWALKER_INTRO_2,
	ACTION_LOREWALKER_INTRO_3,
	ACTION_LOREWALKER_INTRO_4,
	ACTION_LOREWALKER_INTRO_5,
	ACTION_PHASE_2,
	ACTION_SAY_DEATH_ZAO,

	/*Zao Sunseeker*/
	ACTION_NPC_ZAO_SUNSEEKER_RESET,
	ACTION_NPC_ZAO_SUNSEEKER_DIED,
	ACTION_BEGIN_COMBAT,
	ACTION_HAUNTING_SHA_DIED,
	ACTION_HAUNTING_SHA_TRANSITION,

	/*Sun*/
	ACTION_NPC_SUN_RESET,
	ACTION_NPC_SUN_DIED,
	ACTION_REMOVE_FLAG
};

enum Texts
{
	/* Lorewalker Stonestep */
	SAY_INTRO = 0,
	SAY_INTRO_2 = 1,
	SAY_INTRO_3 = 2,
	SAY_INTRO_4 = 3,
	SAY_INTRO_5 = 4,
	SAY_INTRO_6 = 5,
	SAY_INTRO_7 = 6,
	SAY_AFTER_BEGIN_COMBAT = 7,
	SAY_PHASE_2_LOREWALKER = 8,
	SAY_DEATH_ZAO = 9,
	SAY_AFTER_DEATH_ZAO = 10,

	/*Zao Sunseeker*/
	SAY_COMBAT = 0,
	SAY_PHASE_2 = 1
};

enum Phases
{
	PHASE_INTRO,
	PHASE_2,
	PHASE_COMBAT,
	PHASE_DEATH_ZAO
};

enum CreaturesIds2
{
	HAUNTING_SHA = 59555,
	HAUNTING_SHA_TRANSITION = 00004,
	NPC_TRIGGER_1 = 00000,
	NPC_TRIGGER_2 = 00001,
	NPC_TRIGGER_3 = 00002,
	NPC_TRIGGER_4 = 00003
};

Position const spawnPos[3] =
{
	{0.0f, 0.0f, 0.0f, 0.0f}, // Spawn 1 haunting sha phase 2
	{0.0f, 0.0f, 0.0f, 0.0f}, // Spawn 2 haunting sha phase 2
    {0.0f, 0.0f, 0.0f, 0.0f} // Spawn 3 haunting sha phase 2

};


class boss_lorewalker_stonestep : public CreatureScript
{
public:
	boss_lorewalker_stonestep() : CreatureScript("boss_lorewalker_stonestep") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new boss_lorewalker_stonestepAI(creature);
	}

	struct boss_lorewalker_stonestepAI : public ScriptedAI
	{
		boss_lorewalker_stonestepAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkLorewalkerStonestepAlive;
		
		void Reset() OVERRIDE
		{
			checkLorewalkerStonestepAlive = true;
			checkLorewalkerStonestepAlive = me->IsAlive();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
						
			events.Reset();

			if (instance)
			{
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, NOT_STARTED);
				me->AI()->DoAction(ACTION_BOSS_LOREWALKER_STONESTEP_RESET);

				if (Creature* zao = me->GetCreature(*me, instance->GetData64(DATA_NPC_ZAO_SUNSEEKER)))
						if (zao->AI())
						{
							zao->RemoveCorpse();
							zao->Respawn();
						}
				if (Creature* scroll = me->GetCreature(*me, instance->GetData64(DATA_NPC_CORRUPTED_SCROLL)))
					if (scroll->AI())
					{
						scroll->RemoveCorpse();
						scroll->Respawn();
					}
				if (Creature* sun1 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_1)))
					if (sun1->AI())
					{
						sun1->RemoveCorpse();
						sun1->Respawn();
					}
				if (Creature* sun2 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_2)))
					if (sun2->AI())
					{
						sun2->RemoveCorpse();
						sun2->Respawn();
					}
				if (Creature* sun3 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_3)))
					if (sun3->AI())
					{
						sun3->RemoveCorpse();
						sun3->Respawn();
					}
				if (Creature* sun4 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_4)))
					if (sun4->AI())
					{
						sun4->RemoveCorpse();
						sun4->Respawn();
					}
				if (Creature* sun5 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_5)))
					if (sun5->AI())
					{
						sun5->RemoveCorpse();
						sun5->Respawn();
					}
				if(Creature *triggers1 = me->FindNearestCreature(NPC_TRIGGER_1, 50000.0f))
					if(triggers1->AI())
					   triggers1->AI()->Reset();
				if(Creature *triggers2 = me->FindNearestCreature(NPC_TRIGGER_2, 50000.0f))
					if(triggers2->AI())
					   triggers2->AI()->Reset();
				if(Creature *triggers3 = me->FindNearestCreature(NPC_TRIGGER_3, 50000.0f))
					if(triggers3->AI())
					   triggers3->AI()->Reset();
				if(Creature *triggers4 = me->FindNearestCreature(NPC_TRIGGER_4, 50000.0f))
					if(triggers4->AI())
					   triggers4->AI()->Reset();
			}
			
		}

		void DoAction(int32 action) OVERRIDE
        {
            switch (action)
            {
				case ACTION_BOSS_LOREWALKER_STONESTEP_RESET:
					checkLorewalkerStonestepAlive = true;
					break;
				case ACTION_BOSS_LOREWALKER_STONESTEP_DIED:
					checkLorewalkerStonestepAlive = false;
					break;
				case ACTION_LOREWALKER_INTRO:
					events.SetPhase(PHASE_INTRO);
					Talk(SAY_INTRO);
					break;
				case ACTION_LOREWALKER_INTRO_2:
					Talk(SAY_INTRO_2);
					break;				
				case ACTION_LOREWALKER_INTRO_3:
					Talk(SAY_INTRO_3);
					break;
				case ACTION_LOREWALKER_INTRO_4:
					Talk(SAY_INTRO_4);
					break;
				case ACTION_LOREWALKER_INTRO_5:
					Talk(SAY_INTRO_5);
					events.ScheduleEvent(EVENT_INTRO_6, 10*IN_MILLISECONDS, 0, PHASE_INTRO);
					break;
				case ACTION_PHASE_2:
					Talk(SAY_PHASE_2_LOREWALKER);
					break;
				case ACTION_SAY_DEATH_ZAO:
					if (instance)
						instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, DONE);
					me->SetVisible(true);
					Talk(SAY_DEATH_ZAO);
					events.SetPhase(PHASE_DEATH_ZAO);
					events.ScheduleEvent(EVENT_AFTER_DEATH_ZAO, 8*IN_MILLISECONDS, 0, PHASE_DEATH_ZAO);
					break;
			}
        }

		void JustDied(Unit *pWho) OVERRIDE
		{
			
		}

		void KilledUnit(Unit *pWho) OVERRIDE
		{

		}
		
		void EnterEvadeMode() OVERRIDE
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, FAIL);	
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_LOREWALKER_STONESTEP, IN_PROGRESS);

			me->SetInCombatWithZone();
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
						case EVENT_INTRO_6:
							Talk(SAY_INTRO_6);
							events.ScheduleEvent(EVENT_INTRO_7, 12*IN_MILLISECONDS, 0, PHASE_INTRO);
							events.CancelEvent(EVENT_INTRO_6);
							break;

						case EVENT_INTRO_7:
							Talk(SAY_INTRO_7);
							events.ScheduleEvent(EVENT_COMBAT_ZAO, 9*IN_MILLISECONDS, 0, PHASE_INTRO);
							events.CancelEvent(EVENT_INTRO_7);
							break;

						case EVENT_COMBAT_ZAO:
							events.SetPhase(PHASE_COMBAT);
							if (instance)
								if (Creature* zao = me->GetCreature(*me, instance->GetData64(DATA_NPC_ZAO_SUNSEEKER)))
									if (zao->AI())
									{
										zao->AI()->DoAction(ACTION_BEGIN_COMBAT);
									}
							events.ScheduleEvent(EVENT_AFTER_COMBAT, 3*IN_MILLISECONDS, 0 , PHASE_COMBAT);
							events.CancelEvent(EVENT_COMBAT_ZAO);
							break;

						case EVENT_AFTER_COMBAT:
							Talk(SAY_AFTER_BEGIN_COMBAT);
							me->SetVisible(false);
							events.CancelEvent(EVENT_AFTER_COMBAT);
							break;

						case EVENT_AFTER_DEATH_ZAO:
							Talk(SAY_AFTER_DEATH_ZAO);
							events.CancelEvent(EVENT_AFTER_DEATH_ZAO);
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

class npc_zao_sunseeker : public CreatureScript 
{
public:
	npc_zao_sunseeker() : CreatureScript("npc_zao_sunseeker") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_zao_sunseekerAI(creature);
	}

	struct npc_zao_sunseekerAI : public ScriptedAI
	{
		npc_zao_sunseekerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkZaoSunseekerAlive;
		int counterHauntingShaDeath;
		int counterHauntingShaTransition;
		int checkEvadeMode;
		bool checkTransition;

		void Reset() OVERRIDE
		{
			checkTransition = true;
			counterHauntingShaDeath = 0;
			counterHauntingShaTransition = 0;
			checkZaoSunseekerAlive = true;
			checkZaoSunseekerAlive = me->IsAlive();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetVisible(false);
			events.Reset();

			if (instance)
			{
				if(checkEvadeMode == 1)
				{
					if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
						if (lorewalker->AI())
						{
							lorewalker->RemoveCorpse();
							lorewalker->Respawn();
						}
					checkEvadeMode = 0;
				}
				instance->SetBossState(DATA_NPC_ZAO_SUNSEEKER, NOT_STARTED);
				me->AI()->DoAction(ACTION_NPC_ZAO_SUNSEEKER_RESET);
			}

		}

		void DoAction(int32 action) OVERRIDE
		{
			switch (action)
            {
				case ACTION_NPC_ZAO_SUNSEEKER_RESET:
					checkZaoSunseekerAlive = true;
					break;
				case ACTION_NPC_ZAO_SUNSEEKER_DIED:
					checkZaoSunseekerAlive = false;
					break;
				case ACTION_BEGIN_COMBAT:
					Talk(SAY_COMBAT);
					events.SetPhase(PHASE_COMBAT);
					events.ScheduleEvent(EVENT_BEGIN_COMBAT, 0, 0, PHASE_COMBAT);
					break;
				case ACTION_HAUNTING_SHA_DIED:
					counterHauntingShaDeath++;
					break;
				case ACTION_HAUNTING_SHA_TRANSITION:
					counterHauntingShaTransition++;
					break;

			}
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			DoCastToAllHostilePlayers(SPELL_LOREWALKERS_ALACRITY);
			if (instance)
			{
				me->AI()->DoAction(ACTION_NPC_ZAO_SUNSEEKER_DIED);

				if (!checkZaoSunseekerAlive)
					instance->SetBossState(DATA_NPC_ZAO_SUNSEEKER, DONE);

				if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
						if (lorewalker->AI())
						{
							lorewalker->AI()->DoAction(ACTION_SAY_DEATH_ZAO);
						}
			}
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			me->SetInCombatWithZone();
		}

		void EnterEvadeMode() OVERRIDE
		{
			checkEvadeMode = 1;
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
						case EVENT_BEGIN_COMBAT:
							me->SetVisible(true);
							if (instance)
							{
								if (Creature* sun1 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_1)))
									if (sun1->AI())
									{
										sun1->AI()->DoAction(ACTION_REMOVE_FLAG);
									}
								if (Creature* sun2 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_2)))
									if (sun2->AI())
									{
										sun2->AI()->DoAction(ACTION_REMOVE_FLAG);
									}
								if (Creature* sun3 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_3)))
									if (sun3->AI())
									{
										sun3->AI()->DoAction(ACTION_REMOVE_FLAG);
									}
								if (Creature* sun4 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_4)))
									if (sun4->AI())
									{
										sun4->AI()->DoAction(ACTION_REMOVE_FLAG);
									}
								if (Creature* sun5 = me->GetCreature(*me, instance->GetData64(DATA_NPC_SUN_5)))
									if (sun5->AI())
									{
										sun5->AI()->DoAction(ACTION_REMOVE_FLAG);
									}
							}
							events.CancelEvent(EVENT_BEGIN_COMBAT);
							break;

						case EVENT_PHASE_2:
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
							if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
								if (lorewalker->AI())
								{
									lorewalker->AI()->DoAction(ACTION_PHASE_2);
								}
							events.ScheduleEvent(EVENT_HELLFIRE_ARROWS, 2*IN_MILLISECONDS, 0, PHASE_2);
							events.CancelEvent(EVENT_PHASE_2);
							break;

						case EVENT_HELLFIRE_ARROWS:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_HELLFIRE_ARROWS); 
								}
							events.ScheduleEvent(EVENT_HELLFIRE_ARROWS, 3*IN_MILLISECONDS, 0, PHASE_2);
							break;

						default:
							break;
					}
				}
			}

			if(counterHauntingShaDeath == 5 && events.IsInPhase(PHASE_COMBAT))
			{
				me->SummonCreature(HAUNTING_SHA_TRANSITION, spawnPos[0], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10*IN_MILLISECONDS);
				me->SummonCreature(HAUNTING_SHA_TRANSITION, spawnPos[1], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10*IN_MILLISECONDS);
				me->SummonCreature(HAUNTING_SHA_TRANSITION, spawnPos[2], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10*IN_MILLISECONDS);
				events.SetPhase(PHASE_2);
			}
			
			if(counterHauntingShaTransition = 3 && checkTransition)
			{
				Talk(SAY_PHASE_2);
				events.ScheduleEvent(EVENT_PHASE_2, 4*IN_MILLISECONDS, 0, PHASE_2);
				checkTransition = false;
			}
					
			if(events.IsInPhase(PHASE_2))
				DoMeleeAttackIfReady();
		}
	};
};

class npc_sun : public CreatureScript 
{
public:
	npc_sun() : CreatureScript("npc_sun") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_sunAI(creature);
	}

	struct npc_sunAI : public ScriptedAI
	{
		npc_sunAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkSunAlive;
		int checkEvadeMode;

		void Reset() OVERRIDE
		{
			checkSunAlive = true;
			checkSunAlive = me->IsAlive();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetVisible(false);
						
			events.Reset();

			if (instance)
			{
				if(checkEvadeMode == 1)
				{
					if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
						if (lorewalker->AI())
						{
							lorewalker->RemoveCorpse();
							lorewalker->Respawn();
						}
					checkEvadeMode = 0;
				}
				me->AI()->DoAction(ACTION_NPC_SUN_RESET);
			}
		}

		void DoAction(int32 action) OVERRIDE
		{
			switch (action)
            {
				case ACTION_NPC_SUN_RESET:
					checkSunAlive = true;
					break;
				case ACTION_NPC_SUN_DIED:
					checkSunAlive = false;
					break;
				case ACTION_REMOVE_FLAG:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetVisible(true);
					break;
			}
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			me->SummonCreature(HAUNTING_SHA, (me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 80*IN_MILLISECONDS);
			me->AI()->DoAction(ACTION_NPC_SUN_DIED);
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			me->SetInCombatWithZone();
			events.SetPhase(PHASE_COMBAT);
			events.ScheduleEvent(EVENT_SUNFIRE_RAYS, 0, 0, PHASE_COMBAT);
		}

		void EnterEvadeMode() OVERRIDE
		{
			checkEvadeMode = 1;
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
						case EVENT_SUNFIRE_RAYS:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_SUNFIRE_RAYS);
								}
							events.ScheduleEvent(EVENT_SUNFIRE_RAYS, 8, 0, PHASE_COMBAT);
							break;

						default:
							break;
					}
				}
			}
					
		}
	};
};

class npc_haunting_sha : public CreatureScript 
{
public:
	npc_haunting_sha() : CreatureScript("npc_haunting_sha") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_haunting_shaAI(creature);
	}

	struct npc_haunting_shaAI : public ScriptedAI
	{
		npc_haunting_shaAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		int checkEvadeMode;

		void Reset() OVERRIDE
		{
			events.Reset();
			if(checkEvadeMode == 1)
				{
					if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
						if (lorewalker->AI())
						{
							lorewalker->RemoveCorpse();
							lorewalker->Respawn();
						}
					checkEvadeMode = 0;
				}
		}

		void JustSummoned(Creature* creature) OVERRIDE
		{
			events.Reset();
			if(checkEvadeMode == 1)
				{
					if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
						if (lorewalker->AI())
						{
							lorewalker->RemoveCorpse();
							lorewalker->Respawn();
						}
					checkEvadeMode = 0;
				}
		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
			{
				if (Creature* zao = me->GetCreature(*me, instance->GetData64(DATA_NPC_ZAO_SUNSEEKER)))
					if (zao->AI())
					{
						zao->AI()->DoAction(ACTION_HAUNTING_SHA_DIED);
					}
			}

		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
			me->SetInCombatWithZone();
			events.ScheduleEvent(EVENT_HAUNTING_GAZE, 6*IN_MILLISECONDS);
		}

		void EnterEvadeMode() OVERRIDE
		{
			checkEvadeMode = 1;
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
						case EVENT_HAUNTING_GAZE:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_HAUNTING_GAZE);
								}
							events.CancelEvent(EVENT_HAUNTING_GAZE);
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

class npc_haunting_sha_transition : public CreatureScript 
{
public:
	npc_haunting_sha_transition() : CreatureScript("npc_haunting_sha_transition") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_haunting_sha_transitionAI(creature);
	}

	struct npc_haunting_sha_transitionAI : public ScriptedAI
	{
		npc_haunting_sha_transitionAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;

		void Reset() OVERRIDE
		{
			events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetSpeed(MOVE_WALK, 5.0f);
			if(Creature* zao = me->GetCreature(*me, instance->GetData64(DATA_NPC_ZAO_SUNSEEKER)))
				if (zao->AI())
				{
					me->GetMotionMaster()->MovePoint(0, zao->GetPositionX(), zao->GetPositionY(), zao->GetPositionZ());
				}
		}

		void JustSummoned(Creature* creature) OVERRIDE
		{
			events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetSpeed(MOVE_WALK, 5.0f);
			if(Creature* zao = me->GetCreature(*me, instance->GetData64(DATA_NPC_ZAO_SUNSEEKER)))
				if (zao->AI())
				{
					me->GetMotionMaster()->MovePoint(0, zao->GetPositionX(), zao->GetPositionY(), zao->GetPositionZ());
				}
		}

		void JustDied(Unit *pWho) OVERRIDE
		{

		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{
		}

		void EnterEvadeMode() OVERRIDE
		{
		}

		void UpdateAI(uint32 diff) OVERRIDE
		{	
			if(!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (instance)
					if (Creature* zao= me->GetCreature(*me, instance->GetData64(DATA_NPC_ZAO_SUNSEEKER)))
						if (me->GetExactDist2d(zao->GetPositionX(),zao->GetPositionY()) < 1.0f) 
						{
							if (zao->AI())
							{
								zao->AI()->DoAction(ACTION_HAUNTING_SHA_TRANSITION);
								me->DespawnOrUnsummon();
							}
						}		
		}
	};
};

class npc_lorewalker_intro_trigger : public CreatureScript 
{
public:
	npc_lorewalker_intro_trigger() : CreatureScript("npc_lorewalker_intro_trigger") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_lorewalker_intro_triggerAI(creature);
	}

	struct npc_lorewalker_intro_triggerAI : public ScriptedAI
	{
		npc_lorewalker_intro_triggerAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkTrigger; 

		void Reset() OVERRIDE
		{
			checkTrigger = true;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED); 
			//ajouter le flag pour qu'il soit invisible => Modifier dans la db pour la visibilité du PNJ par les joueurs.
		}

		void JustDied(Unit *pWho) OVERRIDE
		{

		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{

		}

		void UpdateAI(uint32 diff) OVERRIDE
		{	
			if(checkTrigger)
			{
				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				if (!PlayerList.isEmpty())
				{
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (me->GetExactDist2d(i->GetSource()->GetPositionX(),i->GetSource()->GetPositionY()) < 12) // A tester si cela fonctionne.
						{
							if (instance)
								if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
									if (lorewalker->AI())
									{
										lorewalker->AI()->DoAction(ACTION_LOREWALKER_INTRO);
										checkTrigger = false;
									}
						}
					}
				}
			}
		}
	};
};

class npc_lorewalker_intro_trigger_2 : public CreatureScript 
{
public:
	npc_lorewalker_intro_trigger_2() : CreatureScript("npc_lorewalker_intro_trigger_2") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_lorewalker_intro_trigger_2AI(creature);
	}

	struct npc_lorewalker_intro_trigger_2AI : public ScriptedAI
	{
		npc_lorewalker_intro_trigger_2AI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkTrigger; 

		void Reset() OVERRIDE
		{
			checkTrigger = true;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED); 
			//ajouter le flag pour qu'il soit invisible => Modifier dans la db pour la visibilité du PNJ par les joueurs.
		}

		void JustDied(Unit *pWho) OVERRIDE
		{

		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{

		}

		void UpdateAI(uint32 diff) OVERRIDE
		{	
			if(checkTrigger)
			{
				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				if (!PlayerList.isEmpty())
				{
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (me->GetExactDist2d(i->GetSource()->GetPositionX(),i->GetSource()->GetPositionY()) < 12) // A tester si cela fonctionne.
						{
							if (instance)
								if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
									if (lorewalker->AI())
									{
										lorewalker->AI()->DoAction(ACTION_LOREWALKER_INTRO_2);
										checkTrigger = false;
									}
						}
					}
				}
			}
		}
	};
};

class npc_lorewalker_intro_trigger_3 : public CreatureScript 
{
public:
	npc_lorewalker_intro_trigger_3() : CreatureScript("npc_lorewalker_intro_trigger_3") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_lorewalker_intro_trigger_3AI(creature);
	}

	struct npc_lorewalker_intro_trigger_3AI : public ScriptedAI
	{
		npc_lorewalker_intro_trigger_3AI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkTrigger; 

		void Reset() OVERRIDE
		{
			checkTrigger = true;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED); 
			//ajouter le flag pour qu'il soit invisible => Modifier dans la db pour la visibilité du PNJ par les joueurs.
		}

		void JustDied(Unit *pWho) OVERRIDE
		{

		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{

		}

		void UpdateAI(uint32 diff) OVERRIDE
		{	
			if(checkTrigger)
			{
				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				if (!PlayerList.isEmpty())
				{
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (me->GetExactDist2d(i->GetSource()->GetPositionX(),i->GetSource()->GetPositionY()) < 12) // A tester si cela fonctionne.
						{
							if (instance)
								if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
									if (lorewalker->AI())
									{
										lorewalker->AI()->DoAction(ACTION_LOREWALKER_INTRO_3);
										checkTrigger = false;
									}
						}
					}
				}
			}
		}
	};
};

class npc_lorewalker_intro_trigger_4 : public CreatureScript 
{
public:
	npc_lorewalker_intro_trigger_4() : CreatureScript("npc_lorewalker_intro_trigger_4") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_lorewalker_intro_trigger_4AI(creature);
	}

	struct npc_lorewalker_intro_trigger_4AI : public ScriptedAI
	{
		npc_lorewalker_intro_trigger_4AI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		
		bool checkTrigger; 

		void Reset() OVERRIDE
		{
			checkTrigger = true;
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED); 
			//ajouter le flag pour qu'il soit invisible => Modifier dans la db pour la visibilité du PNJ par les joueurs.
		}

		void JustDied(Unit *pWho) OVERRIDE
		{

		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{

		}

		void UpdateAI(uint32 diff) OVERRIDE
		{	
			if(checkTrigger)
			{
				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				if (!PlayerList.isEmpty())
				{
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (me->GetExactDist2d(i->GetSource()->GetPositionX(),i->GetSource()->GetPositionY()) < 12) // A tester si cela fonctionne.
						{
							if (instance)
								if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
									if (lorewalker->AI())
									{
										lorewalker->AI()->DoAction(ACTION_LOREWALKER_INTRO_4);
										checkTrigger = false;
									}
						}
					}
				}
			}
		}
	};
};

class npc_corrupted_scroll : public CreatureScript 
{
public:
	npc_corrupted_scroll() : CreatureScript("npc_corrupted_scroll") { }

	CreatureAI* GetAI(Creature* creature) const OVERRIDE
	{
		return new npc_corrupted_scrollAI(creature);
	}

	struct npc_corrupted_scrollAI : public ScriptedAI
	{
		npc_corrupted_scrollAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;
		EventMap events;
		

		void Reset() OVERRIDE
		{

		}

		void JustDied(Unit *pWho) OVERRIDE
		{
			if (instance)
				if (Creature* lorewalker = me->GetCreature(*me, instance->GetData64(DATA_BOSS_LOREWALKER_STONESTEP)))
					if (lorewalker->AI())
					{
						lorewalker->AI()->DoAction(ACTION_LOREWALKER_INTRO_5);
					}
		}

		void EnterCombat(Unit* /*who*/) OVERRIDE
		{

		}

		void UpdateAI(uint32 diff) OVERRIDE
		{	
			
		}
	};
};

void AddSC_boss_lorewalker_stonestep()
{
	new boss_lorewalker_stonestep();
	new npc_zao_sunseeker();
	new npc_sun();
	new npc_haunting_sha();
	new npc_haunting_sha_transition();
	new npc_lorewalker_intro_trigger();
	new npc_lorewalker_intro_trigger_2();
	new npc_lorewalker_intro_trigger_3();
	new npc_lorewalker_intro_trigger_4();
	new npc_corrupted_scroll();
}