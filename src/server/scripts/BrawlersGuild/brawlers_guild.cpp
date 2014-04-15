
#include "ScriptPCH.h"
#include "BrawlersGuildMgr.h"

enum Events
{
	/* Bizmo */
    EVENT_BIZMO_TRIGGER_STOP	= 1
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
    SAY_BIZMO_13				= 13
};

enum Actions
{
	ACTION_QUEUE,
	ACTION_PLAYER_CAN_ENTER,
    ACTION_BIZMO_MOVE
};

enum Npcs
{
	NPC_BIZMO					= 68323,
	NPC_BIZMOS_BRAWLPUB_BOUNCER	= 68408,
    NPC_BRAWLERS_TRIGGER		= 400461
};

#define DISPLAYID_GYROCOPTER	44634
#define GOSSIP_ITEM_QUEUE		"Oui, inscrivez-moi pour un combat !"

class npc_brawlers_guild_queue : public CreatureScript
{
public:
    npc_brawlers_guild_queue() : CreatureScript("npc_brawlers_guild_queue") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player)
		{
			if (!player->HasAura(SPELL_QUEUED_FOR_BRAWL))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_QUEUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
				

			player->PlayerTalkClass->SendGossipMenu(player->GetGossipTextId(creature), creature->GetGUID());
		}

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		if (player)
		{
			player->PlayerTalkClass->ClearMenus();

			int32 bp = 0;

			if (action == GOSSIP_ACTION_INFO_DEF + 1)
			{
				sBrawlersGuildMgr->AddPlayer(player);
				player->CLOSE_GOSSIP_MENU();
			}

			if (Aura* aura = player->GetAura(SPELL_QUEUED_FOR_BRAWL))
				aura->SetCharges(2);


		}

		return true;
	}

    struct npc_brawlers_guild_queueAI : public ScriptedAI
    {
        npc_brawlers_guild_queueAI(Creature* creature) : ScriptedAI(creature) {}

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
        return new npc_brawlers_guild_queueAI(creature);
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

		void DoAction(const int32 action)
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

class spell_queued_for_brawl : public SpellScriptLoader
{
public:
	spell_queued_for_brawl() : SpellScriptLoader("spell_queued_for_brawl") { }

	class spell_queued_for_brawl_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_queued_for_brawl_AuraScript);

		void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			sBrawlersGuildMgr->RemovePlayer(GetOwner()->ToPlayer());
		}

		void Register()
		{
			AfterEffectRemove += AuraEffectRemoveFn(spell_queued_for_brawl_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_queued_for_brawl_AuraScript();
	}
};


void AddSC_the_brawlers_guild()
{
    new npc_brawlers_guild_queue();
    new npc_bizmo();
	new spell_queued_for_brawl();
}
