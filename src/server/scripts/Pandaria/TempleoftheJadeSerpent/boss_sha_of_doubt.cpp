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

class boss_sha_of_doubt : public CreatureScript
{
    public:
        boss_sha_of_doubt() : CreatureScript("boss_sha_of_doubt") { }

        struct boss_sha_of_doubtAI : public BossAI
        {
            boss_sha_of_doubtAI(Creature* creature) : BossAI(creature, DATA_SHA_OF_DOUBT)
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
            return new boss_sha_of_doubtAI(creature);
        }
};

void AddSC_boss_sha_of_doubt()
{
    new boss_sha_of_doubt();
}
