/* # Script de Sungis : The Brawler's guild # */

#include "ScriptPCH.h"

enum Spells
{
	/* Bizmo's Brawlup Bouncer */
	SPELL_QUEUED_FOR_BRAWL		= 132639,
	SPELL_ARENA_TELEPORTATION	= 105315,
};

enum Events
{
	/* Bizmo */
	EVENT_BIZMO_TRIGGER_STOP	= 1,
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

enum Actions
{
	ACTION_QUEUE,
	ACTION_PLAYER_CAN_ENTER,
	ACTION_BIZMO_MOVE,
};

enum Npcs
{
	NPC_BIZMO					= 68323,
	NPC_BIZMOS_BRAWLPUB_BOUNCER	= 68408,
	NPC_BRAWLERS_TRIGGER		= 400461,
};

#define DISPLAYID_GYROCOPTER	44634
#define GOSSIP_ITEM_QUEUE		"Oui, inscrivez-moi pour un combat !"

class npc_bizmos_brawlpub_bouncer : public CreatureScript
{
public:
    npc_bizmos_brawlpub_bouncer() : CreatureScript("npc_bizmos_brawlpub_bouncer") { }

    struct npc_bizmos_brawlpub_bouncerAI : public ScriptedAI
    {
        npc_bizmos_brawlpub_bouncerAI(Creature* creature) : ScriptedAI(creature) {}

		bool OnGossipHello(Player* player, Creature* creature)
        {
			if (player)
			{
				if (!player->HasAura(SPELL_QUEUED_FOR_BRAWL))
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_QUEUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

				player->PlayerTalkClass->SendGossipMenu(40040, creature->GetGUID());
			}

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
			if (player)
			{
				player->PlayerTalkClass->ClearMenus();

				if (action == GOSSIP_ACTION_INFO_DEF+1)
				{
					player->CastSpell(player, SPELL_QUEUED_FOR_BRAWL, true);
					player->CLOSE_GOSSIP_MENU();
				}
			}

            return true;
        }

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
        return new npc_bizmos_brawlpub_bouncerAI(creature);
    }
};

class npc_bizmo : public CreatureScript
{
public:
    npc_bizmo() : CreatureScript("npc_bizmo") { }

    struct npc_bizmoAI : public ScriptedAI
    {
        npc_bizmoAI(Creature* creature) : ScriptedAI(creature) {}

		EventMap events;
		bool battleStart;

        void Reset()
        {
			events.Reset();
			battleStart = false;
			me->Mount(DISPLAYID_GYROCOPTER);
        }

		void DoAction(int32 action)
        {
            switch (action)
            {
				case ACTION_BIZMO_MOVE:
				{
					if (Creature* trigger = me->FindNearestCreature(NPC_BRAWLERS_TRIGGER, 99999.0f, true))
					{
						me->GetMotionMaster()->MovePoint(0, trigger->GetHomePosition());
						battleStart = true;
					}
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
			if (battleStart)
				if (Creature* trigger = me->FindNearestCreature(NPC_BRAWLERS_TRIGGER, 99999.0f, true))
					if (me->GetExactDist(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ()) <= 1)
						events.ScheduleEvent(EVENT_BIZMO_TRIGGER_STOP, 0);

			while(uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					case EVENT_BIZMO_TRIGGER_STOP:
						if (Creature* trigger = me->FindNearestCreature(NPC_BRAWLERS_TRIGGER, 99999.0f, true))
						{
							me->Relocate(trigger->GetHomePosition());
							me->SetFacingTo(trigger->GetOrientation());
							events.CancelEvent(EVENT_BIZMO_TRIGGER_STOP);
							break;
						}

					default:
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bizmoAI(creature);
    }
};

class npc_the_brawlers_guild_trigger : public CreatureScript
{
public:
    npc_the_brawlers_guild_trigger() : CreatureScript("npc_the_brawlers_guild_trigger") { }

    struct npc_the_brawlers_guild_triggerAI : public ScriptedAI
    {
        npc_the_brawlers_guild_triggerAI(Creature* creature) : ScriptedAI(creature) {}

		/*Map* map;
		EventMap events;
		bool bossState;
		uint64 playerQueue[50];
		uint32 queue;

        void Reset()
        {
			events.Reset();

			bossState = false;
			queue = 0;
        }

		void DoAction(int32 action)
        {
            switch (action)
            {
				case ACTION_QUEUE:
				{
					map = me->GetMap();
					if (map)
					{
						Map::PlayerList const &PlayerList = map->GetPlayers();

						if (!PlayerList.isEmpty())
							for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
								if (Player* player = i->getSource())
									if (player->HasAura(SPELL_QUEUED_FOR_BRAWL))
									{
										queue++;
										playerQueue[queue] = player->GetGUID();
										sLog->outDebug(LOG_FILTER_NETWORKIO, "SUNGIS QUEUE VALUE = %u", queue);
									}
					}

					break;
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
			if (!bossState)
			{
				map = me->GetMap();
				if (map)
				{
					Map::PlayerList const &PlayerList = map->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->getSource())
								if (player->HasAura(SPELL_QUEUED_FOR_BRAWL))
									if (playerQueue[1] == player->GetGUID())
									{
										if (Creature* bizmo = me->FindNearestCreature(NPC_BIZMO, 99999.0f, true))
											bizmo->AI()->DoAction(ACTION_BIZMO_MOVE);

										player->CastSpell(player, SPELL_ARENA_TELEPORTATION, true);
										bossState = true;
									}
					}
			}
        }*/
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_brawlers_guild_triggerAI(creature);
    }
};

class npc_brawlgar_arena_grunt : public CreatureScript
{
public:
    npc_brawlgar_arena_grunt() : CreatureScript("npc_brawlgar_arena_grunt") { }

    struct npc_brawlgar_arena_gruntAI : public ScriptedAI
    {
        npc_brawlgar_arena_gruntAI(Creature* creature) : ScriptedAI(creature) {}

		bool OnGossipHello(Player* player, Creature* creature)
        {
			if (player)
			{
				if (!player->HasAura(SPELL_QUEUED_FOR_BRAWL))
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_QUEUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

				player->PlayerTalkClass->SendGossipMenu(40040, creature->GetGUID());
			}

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
			if (player)
			{
				player->PlayerTalkClass->ClearMenus();

				if (action == GOSSIP_ACTION_INFO_DEF+1)
				{
					player->CastSpell(player, SPELL_QUEUED_FOR_BRAWL, true);
					player->CLOSE_GOSSIP_MENU();
				}
			}

            return true;
        }

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
        return new npc_brawlgar_arena_gruntAI(creature);
    }
};

void AddSC_the_brawlers_guild()
{
	new npc_bizmos_brawlpub_bouncer();
	new npc_bizmo();
	new npc_the_brawlers_guild_trigger();
	new npc_brawlgar_arena_grunt();
}