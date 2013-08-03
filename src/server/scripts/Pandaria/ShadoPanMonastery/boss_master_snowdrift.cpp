#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "shadopan_monastery.h"

enum Yells
{
};

enum Spells
{
};

enum Events
{
};

class boss_master_snowdrift : public CreatureScript
{
    public:
        boss_master_snowdrift() : CreatureScript("boss_master_snowdrift") { }

        struct boss_master_snowdriftAI : public BossAI
        {
            boss_master_snowdriftAI(Creature* creature) : BossAI(creature, DATA_MASTER_SNOWDRIFT)
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
            return new boss_master_snowdriftAI(creature);
        }
};

void AddSC_boss_master_snowdrift()
{
    new boss_master_snowdrift();
}
