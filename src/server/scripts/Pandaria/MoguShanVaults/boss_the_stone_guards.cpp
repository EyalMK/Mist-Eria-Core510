#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mogushan_vaults.h"

enum Yells
{
};

enum Spells
{
};

enum Events
{
};

class boss_the_stone_guard : public CreatureScript
{
    public:
        boss_the_stone_guard() : CreatureScript("boss_the_stone_guard") { }

        struct boss_the_stone_guardAI : public BossAI
        {
            boss_the_stone_guardAI(Creature* creature) : BossAI(creature, DATA_THE_STONE_GUARD)
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
            return new boss_the_stone_guardAI(creature);
        }
};

void AddSC_boss_the_stone_guard()
{
    new boss_the_stone_guard();
}
