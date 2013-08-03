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

class boss_lorewalker_stonestep : public CreatureScript
{
    public:
        boss_lorewalker_stonestep() : CreatureScript("boss_lorewalker_stonestep") { }

        struct boss_lorewalker_stonestepAI : public BossAI
        {
            boss_lorewalker_stonestepAI(Creature* creature) : BossAI(creature, DATA_LOREWALKER_STONESTEP)
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
            return new boss_lorewalker_stonestepAI(creature);
        }
};

void AddSC_boss_lorewalker_stonestep()
{
    new boss_lorewalker_stonestep();
}
