/* # Script de Tydrheal : Sha of Doubt # */

/* Notes : Tester -- voir spells -- SoundID

Sha of Doubt : Script 95% (terminé -- voir spells)

UPDATE creature_template SET ScriptName = 'boss_sha_of_doubt' WHERE entry = 56439;

INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(56439, 0, 0, "Rendez-vous ou mourez. Vous ne pouvez me vaincre.", 14, 0, 100, 0, 0, ???????, "ShaOfDoubt - combat"),
(56439, 1, 0, "Vous vous étiolez si facilement.", 14, 0, 100, 0, 0, ???????, "ShaOfDoubt - boundsOfReality"),
(56439, 2, 0, "Vous ne pouvez pas m’échapper. Je suis… dans… chaque… souffle.", 14, 0, 100, 0, 0, ???????, "ShaOfDoubt - Death"),
(56439, 3, 0, "Succombez à la noirceur dans votre âme.", 14, 0, 100, 0, 0, ???????, "ShaOfDoubt - Slay1"),
(56439, 4, 0, "Abandonnez toute espérance.", 14, 0, 100, 0, 0, ???????, "ShaOfDoubt - Slay2");

*/

#include "ScriptPCH.h"
#include "temple_of_the_jade_serpent.h"

enum Spells
{
	/* Sha Of Doubt */
	SPELL_BOUNDS_OF_REALITY = 117665,
	SPELL_GATHERING_DOUBT = 117570,
	SPELL_WITHER_WILL = 106736,
	SPELL_TOUCH_OF_NOTHINGNESS = 106113,
	

};

enum Events
{
	EVENT_WITHER_WILL,
	EVENT_VERIFICATION,
	EVENT_TOUCH_OF_NOTHINGNESS
};

enum Actions
{
	ACTION_BOSS_SHA_OF_DOUBT_RESET,
	ACTION_BOSS_SHA_OF_DOUBT_DIED
};

enum Texts
{
	SAY_ENTER_COMBAT = 0,
	SAY_BOUNDS_OF_REALITY = 1,
	SAY_DEATH = 2,
	SAY_SLAY_1 = 3,
	SAY_SLAY_2 = 4
};

enum Phases
{
	PHASE_COMBAT
};

enum SpawnIds
{
	FIGMENT_OF_DOUBT = 56792
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
		
		bool checkShaOfDoubtAlive;
		bool checkBounds1;
		bool checkBounds2;
		std::list<Creature*> listFigment;
		

		void Reset()
		{
			checkBounds1 = false;
			checkBounds2 = false;
			checkShaOfDoubtAlive = true;
			checkShaOfDoubtAlive = me->IsAlive();
						
			events.Reset();

			if (instance)
			{
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, NOT_STARTED);
				me->AI()->DoAction(ACTION_BOSS_SHA_OF_DOUBT_RESET);
			}
			
		}
		
		void DoAction(int32 action)
        {
            switch (action)
            {
				case ACTION_BOSS_SHA_OF_DOUBT_RESET:
					checkShaOfDoubtAlive = true;
					break;
				case ACTION_BOSS_SHA_OF_DOUBT_DIED:
					checkShaOfDoubtAlive = false;
					break;
			}
        }

		void JustDied(Unit *pWho)
		{
			Talk(SAY_DEATH);

			me->AI()->DoAction(ACTION_BOSS_SHA_OF_DOUBT_DIED);
			
			if (instance)
			{
				if(!checkShaOfDoubtAlive)
					instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, DONE);

			}
			
		}

		void KilledUnit(Unit *pWho)
		{
			Talk(urand(SAY_SLAY_1, SAY_SLAY_2));
		}
		
		void EnterEvadeMode()
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, FAIL);	
		}

		void EnterCombat(Unit* /*who*/)
		{
			if (instance)
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, IN_PROGRESS);

			me->SetInCombatWithZone();
			events.SetPhase(PHASE_COMBAT);
			events.ScheduleEvent(EVENT_WITHER_WILL, 6*IN_MILLISECONDS, 0, PHASE_COMBAT);
		}
		
		void Verification()
		{
			GetCreatureListWithEntryInGrid(listFigment, me, FIGMENT_OF_DOUBT, 50000.0f);
			if(!listFigment.empty())
			{
				for(std::list<Creature*>::const_iterator i = listFigment.begin() ; i != listFigment.end() ; ++i)
				{
					if((*i)->IsAlive())
					{
						DoCast(*i, SPELL_GATHERING_DOUBT);
						(*i)->DespawnOrUnsummon();	
						if(Aura* aura = me->GetAura(SPELL_BOUNDS_OF_REALITY))
							me->RemoveAura(aura);
					}
					else
					{
						if(Aura* aura = me->GetAura(SPELL_BOUNDS_OF_REALITY))
							me->RemoveAura(aura);

					}
				}
			}
			else
			{
				if(Aura* aura = me->GetAura(SPELL_BOUNDS_OF_REALITY))
					me->RemoveAura(aura);		
			}

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
						case EVENT_WITHER_WILL:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_WITHER_WILL);
								}
							events.ScheduleEvent(EVENT_WITHER_WILL, 7*IN_MILLISECONDS, 0, PHASE_COMBAT);							
							break;
							
						case EVENT_VERIFICATION:
							Verification();
							events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 3*IN_MILLISECONDS, 0, PHASE_COMBAT);
							events.CancelEvent(EVENT_VERIFICATION);							
							break;

						case EVENT_TOUCH_OF_NOTHINGNESS:
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								if(target->GetTypeId() == TYPEID_PLAYER)
								{
									DoCast(target, SPELL_TOUCH_OF_NOTHINGNESS);
								}
							events.ScheduleEvent(EVENT_TOUCH_OF_NOTHINGNESS, 15*IN_MILLISECONDS, 0, PHASE_COMBAT);
							break;
							
						default:
							break;
					}
				}
			}
			
			if(me->HealthBelowPct(75) && !checkBounds1)
			{
				Talk(SAY_BOUNDS_OF_REALITY);
				DoCast(me, SPELL_BOUNDS_OF_REALITY);
				me->SummonCreature(FIGMENT_OF_DOUBT, (me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600*IN_MILLISECONDS);
				me->SummonCreature(FIGMENT_OF_DOUBT, (me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600*IN_MILLISECONDS);
				me->SummonCreature(FIGMENT_OF_DOUBT, (me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600*IN_MILLISECONDS);
				me->SummonCreature(FIGMENT_OF_DOUBT, (me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_VERIFICATION, 30*IN_MILLISECONDS, 0, PHASE_COMBAT);
				checkBounds1 = true;
			}

			if(me->HealthBelowPct(25) && !checkBounds2)
			{
				checkBounds1 = false;
				checkBounds2 = true;
			}
					

			DoMeleeAttackIfReady();
		}
	};
};

void AddSC_boss_sha_of_doubt()
{
	new boss_sha_of_doubt();
}