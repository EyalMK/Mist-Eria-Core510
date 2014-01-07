/* Kun-Lai Summit */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"

/*######
## npc_suspicious_snow_pile
######*/

#define NPC_MISCHIEVOUS_SNOW_SPRITE 59693

class npc_suspicious_snow_pile : public CreatureScript
{
public:
    npc_suspicious_snow_pile() : CreatureScript("npc_suspicious_snow_pile") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_suspicious_snow_pileAI(creature);
    }

    struct npc_suspicious_snow_pileAI : public ScriptedAI
    {
        npc_suspicious_snow_pileAI(Creature* creature) : ScriptedAI(creature) { }

        void MoveInLineOfSight(Unit* who)
        {
			if (!me->IsWithinDistInMap(who, 1.0f) || intro)
				return;

			if (!who || !who->IsInWorld())
				return;

            if (who->GetTypeId() == TYPEID_PLAYER && me->isAlive())
            {
                me->SummonCreature(NPC_MISCHIEVOUS_SNOW_SPRITE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()+1, 0, TEMPSUMMON_TIMED_DESPAWN, 300000);
                me->DisappearAndDie();
            }
        }
    };
};

void AddSC_kun_lai_summit()
{
    new npc_suspicious_snow_pile();
}
