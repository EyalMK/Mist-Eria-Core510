#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "terrace_of_the_endless_spring.h"

enum Yells
{
};

// me->AttackStop();
// me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
// me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
// me->NearTeleportTo(TELE_X, TELE_Y, TELE_Z, TELE_O);
// me->RemoveUnitMovementFlag(MOVEMENTFLAG_LEVITATING);


enum Spells
{
    /* Lei Shi */
    SPELL_AFRAID             = 123181,
    SPELL_GET_AWAY           = 123461,
    SPELL_SPRAY              = 123121,
    SPELL_PROTECT            = 123250,
    SPELL_CLOUDED_REFLECTION = 123620,
    SPELL_HIDE               = 123244,
    SPELL_TRANSFORM          = 127535,
    SPELL_BERSERK            = 26662,
	
    /* Protecteur animé */
    SPELL_ICE_TRAP           = 135382,
    SPELL_PROTECT_ANIMATED   = 123505
};

enum Events
{
    /* Lei Shi */
    EVENT_AFRAID = 1,
    EVENT_GET_AWAY = 2,
    EVENT_SPRAY = 3,
    EVENT_PROTECT = 4,
    EVENT_CLOUDED_REFLECTION = 5,
    EVENT_HIDE = 6,
    EVENT_TRANSFORM = 7,
    EVENT_BERSERK = 8
};

enum Npcs
{
    /* Protecteur animé */
	NPC_ANIMATED_PROTECTOR   = 57109
};

static Position SpawnLocations[4]=
{
	{0.0f, 0.0f, 0.0f, 0.0f}, // Spawn 1 
	{0.0f, 0.0f, 0.0f, 0.0f}, // Spawn 2
    {0.0f, 0.0f, 0.0f, 0.0f}, // Spawn 3
	{0.0f, 0.0f, 0.0f, 0.0f}  // Spawn 4
};

struct LocationsXY
{
    float x, y;
    uint32 id;
};

class boss_lei_shi : public CreatureScript
{
    public:
        boss_lei_shi() : CreatureScript("boss_lei_shi") { }

        struct boss_lei_shiAI : public BossAI
        {
            boss_lei_shiAI(Creature* creature) : BossAI(creature, DATA_LEI_SHI)
            {
				instance = creature->GetInstanceScript();
            }

            void Reset()
            {
				_Reset();
				events.Reset();
				me->RemoveAurasDueToSpell(SPELL_AFRAID);
            }

            void EnterCombat(Unit* /*who*/)
            {
				_EnterCombat();
				DoCast(me, SPELL_AFRAID, true);
            }

            void JustDied(Unit* /*killer*/)
            {
			    _JustDied();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (me->HealthBelowPctDamaged(80, damage) ||
                    me->HealthBelowPctDamaged(60, damage) ||
                    me->HealthBelowPctDamaged(40, damage) ||
                    me->HealthBelowPctDamaged(20, damage))
                {
                    DoCast(SPELL_PROTECT);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                /*
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                */

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_lei_shiAI(creature);
        }
};

class npc_animated_protector : public CreatureScript 
{
public:
	npc_animated_protector() : CreatureScript("npc_animated_protector") { }

	CreatureAI* GetAI(Creature* creature) const 
	{
		return new npc_animated_protectorAI(creature);
	}

	struct npc_animated_protectorAI : public ScriptedAI
	{
		npc_animated_protectorAI(Creature *creature) : ScriptedAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;		
	    uint32 Ice_Trap_Timer; 
		uint32 Protect_Animated_Timer;
		
            void JustDied(Unit* /*killer*/)
            {
			    _JustDied();
			    DespawnCreatures(NPC_ANIMATED_PROTECTOR, 1000.0f);  //
            }

		void Reset()
		{
				Ice_Trap_Timer = 2200;
			    Protect_Animated_Timer = 25000;	
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

						default:
							break;
					}
				}
			}

			DoMeleeAttackIfReady();
		}
	};
};	
	


void AddSC_boss_lei_shi()
{
    new boss_lei_shi();
    new npc_animated_protector();
}
