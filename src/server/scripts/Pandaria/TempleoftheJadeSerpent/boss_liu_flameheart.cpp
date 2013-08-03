#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "temple_of_the_jade_serpent.h"

enum Yells
{
};

enum Spells
{
};

enum Events
{
};

class boss_liu_flameheart : public CreatureScript
{
    public:
        boss_liu_flameheart() : CreatureScript("boss_liu_flameheart") { }

        struct boss_liu_flameheartAI : public BossAI
        {
            boss_liu_flameheartAI(Creature* creature) : BossAI(creature, DATA_LIU_FLAMEHEART)
            {
            }

            void Reset()
            {
            }

            void EnterCombat(Unit* /*who*/)
            {
            }

            void JustDied(Unit* /*killer*/)
            {
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
            return new boss_liu_flameheartAI(creature);
        }
};

void AddSC_boss_liu_flameheart()
{
    new boss_liu_flameheart();
}
