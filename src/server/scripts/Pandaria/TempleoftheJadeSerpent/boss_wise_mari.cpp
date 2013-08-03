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

class boss_wise_mari : public CreatureScript
{
    public:
        boss_wise_mari() : CreatureScript("boss_wise_mari") { }

        struct boss_wise_mariAI : public BossAI
        {
            boss_wise_mariAI(Creature* creature) : BossAI(creature, DATA_WISE_MARI)
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
            return new boss_wise_mariAI(creature);
        }
};

void AddSC_boss_wise_mari()
{
    new boss_wise_mari();
}
