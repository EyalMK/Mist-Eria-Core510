#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "stormstout_brewery.h"

enum Yells
{
};

enum Spells
{
};

enum Events
{
};

class boss_hoptallus : public CreatureScript
{
    public:
        boss_hoptallus() : CreatureScript("boss_hoptallus") { }

        struct boss_hoptallusAI : public BossAI
        {
            boss_hoptallusAI(Creature* creature) : BossAI(creature, DATA_HOPTALLUS)
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
            return new boss_hoptallusAI(creature);
        }
};

void AddSC_boss_hoptallus()
{
    new boss_hoptallus();
}
