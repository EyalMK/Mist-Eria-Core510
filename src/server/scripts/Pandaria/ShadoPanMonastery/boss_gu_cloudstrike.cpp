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

class boss_gu_cloudstrike : public CreatureScript
{
    public:
        boss_gu_cloudstrike() : CreatureScript("boss_commander_rimok") { }

        struct boss_gu_cloudstrikeAI : public BossAI
        {
            boss_gu_cloudstrikeAI(Creature* creature) : BossAI(creature, DATA_GU_CLOUDSTRIKE)
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
            return new boss_gu_cloudstrikeAI(creature);
        }
};

void AddSC_boss_gu_cloudstrike()
{
    new boss_gu_cloudstrike();
}
