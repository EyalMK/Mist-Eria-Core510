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

class boss_taran_zhu : public CreatureScript
{
    public:
        boss_taran_zhu() : CreatureScript("boss_taran_zhu") { }

        struct boss_taran_zhuAI : public BossAI
        {
            boss_taran_zhuAI(Creature* creature) : BossAI(creature, DATA_TARAN_ZHU)
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
            return new boss_taran_zhuAI(creature);
        }
};

void AddSC_boss_taran_zhu()
{
    new boss_taran_zhu();
}
