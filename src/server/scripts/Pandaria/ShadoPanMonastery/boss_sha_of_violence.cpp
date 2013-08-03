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

class boss_sha_of_violence : public CreatureScript
{
    public:
        boss_sha_of_violence() : CreatureScript("boss_sha_of_violence") { }

        struct boss_sha_of_violenceAI : public BossAI
        {
            boss_sha_of_violenceAI(Creature* creature) : BossAI(creature, DATA_SHA_OF_VIOLENCE)
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
            return new boss_sha_of_violenceAI(creature);
        }
};

void AddSC_boss_sha_of_violence()
{
    new boss_sha_of_violence();
}
