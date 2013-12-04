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
};

enum Events
{
	EVENT_WITHER_WILL				= 1,
	EVENT_TOUCH_OF_NOTHINGNESS		= 2,
	EVENT_BOUNDS_OF_REALITY			= 3,
	EVENT_SUMMON_FIGMENT_OF_DOUBT	= 4
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
	NPC_SHA_TRIGGER = 400453,
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

		void Reset()
		{		
			events.Reset();

			if (instance)
			{
				boundsOfReality = false;
				events.SetPhase(PHASE_NULL);
				instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, NOT_STARTED);
			}
		}
		
		void JustDied(Unit *pWho)
		{
			if (instance)
					instance->SetBossState(DATA_BOSS_SHA_OF_DOUBT, DONE);
		}

		void KilledUnit(Unit *pWho)
		{

		}
		
		void EnterEvadeMode()
		{
			if (instance)
			{
				boundsOfReality = false;
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

			if (HealthBelowPct(75) && events.IsInPhase(PHASE_COMBAT))
			{
				events.ScheduleEvent(EVENT_BOUNDS_OF_REALITY, 1*IN_MILLISECONDS, 0, PHASE_BOUNDS_OF_REALITY);
				events.SetPhase(PHASE_BOUNDS_OF_REALITY);
			}

			if (HealthBelowPct(50) && events.IsInPhase(PHASE_COMBAT))
			{
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

        void Reset()
		{
			events.Reset();
		}

		void EnterEvadeMode()
		{
			if (instance)
				me->DespawnOrUnsummon();
		}

        void JustDied(Unit *pWho)
        {

        }

        void EnterCombat(Unit* /*who*/)
        {

        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
        }
    };
};

void AddSC_boss_sha_of_doubt()
{
	new boss_sha_of_doubt();
	new npc_figment_of_doubt();
}