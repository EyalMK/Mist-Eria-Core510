/* # Script de Sungis : The Brawler's guild # */

#include "ScriptPCH.h"

enum Spells
{
	/* Bizmo's Brawlup Entrance */
	SPELL_QUEUED_FOR_BRAWL		= 132639,
};

enum Texts
{
	/* Bizmo */
	SAY_BIZMO_BOSS_DEATH		= 1,
	SAY_BIZMO_2					= 2,
	SAY_BIZMO_3					= 3,
	SAY_BIZMO_4					= 4,
	SAY_BIZMO_5					= 5,
	SAY_BIZMO_6					= 6,
	SAY_BIZMO_7					= 7,
	SAY_BIZMO_8					= 8,
	SAY_BIZMO_9					= 9,
	SAY_BIZMO_10				= 10,
	SAY_BIZMO_11				= 11,
	SAY_BIZMO_12				= 12,
	SAY_BIZMO_13				= 13,
};

#define DISPLAYID_GYROCOPTER	44634

class npc_bizmos_brawlpub_entrance : public CreatureScript
{
public:
    npc_bizmos_brawlpub_entrance() : CreatureScript("npc_bizmos_brawlpub_entrance") { }

    struct npc_bizmos_brawlpub_entranceAI : public ScriptedAI
    {
        npc_bizmos_brawlpub_entranceAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {

        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bizmos_brawlpub_entranceAI(creature);
    }
};

class npc_bizmo : public CreatureScript
{
public:
    npc_bizmo() : CreatureScript("npc_bizmo") { }

    struct npc_bizmoAI : public ScriptedAI
    {
        npc_bizmoAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
			me->Mount(DISPLAYID_GYROCOPTER);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bizmoAI(creature);
    }
};

void AddSC_the_brawlers_guild()
{
	new npc_bizmos_brawlpub_entrance();
	new npc_bizmo();
}