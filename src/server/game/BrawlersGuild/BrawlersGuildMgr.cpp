/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Common.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "BrawlersGuildMgr.h"
#include "Item.h"
#include "Chat.h"

float BrawlersTeleportLocations[MAX_BRAWLERS_GUILDS][MAX_TELEPORTS][3] =
{
	{ { -121.f, 2499.f, -57.f }, { -89.f, 2476.f, -43.f } },
	{ { 2032.f, -4753.f, 87.f }, { 2062.f, 4547.f, 87.f } }
};


BrawlersGuild::BrawlersGuild()
{
	brawlstate = BRAWL_STATE_WAITING;
}

BrawlersGuild::~BrawlersGuild()
{

}

void BrawlersGuild::Update(uint32 diff)
{
	CheckDisconectedPlayers();

	bool needUpdateAura = !removeList.empty();

	for(BrawlersList::iterator it = removeList.begin(); it != removeList.end(); it++)
		 waitList.remove(*it);
	removeList.clear();

	if (needUpdateAura)
		UpdateAllAuras();



}

void BrawlersGuild::AddPlayer(Player *player)
{
    if(!player)
        return;

    waitList.push_back(player->GetGUID());
    UpdateAura(player, waitList.size());
}

void BrawlersGuild::RemovePlayer(Player *player)
{
    if(!player)
        return;

    RemovePlayer(player->GetGUID());
}

void BrawlersGuild::RemovePlayer(uint64 guid)
{
    removeList.push_back(guid);
}

void BrawlersGuild::UpdateAura(Player* player, uint32 rank)
{
	if (!player)
		return;

	if (!player->HasAura(SPELL_QUEUED_FOR_BRAWL))
		player->CastSpell(player, SPELL_QUEUED_FOR_BRAWL, true);

	if (Aura* aura = player->GetAura(SPELL_QUEUED_FOR_BRAWL))
		if (AuraEffect* eff = aura->GetEffect(0))
			eff->SetAmount(rank);

	std::stringstream ss;
	ss << "rang : " << rank;

	ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
}

void BrawlersGuild::UpdateAllAuras()
{
	uint32 rank = 1;
	for(BrawlersList::iterator it = waitList.begin(); it != waitList.end(); it++)
	{
		if (Player *player = ObjectAccessor::FindPlayer(*it))
		{
			UpdateAura(player, rank);
			++rank;
		}
		else
			RemovePlayer(*it);
	}
}

void BrawlersGuild::CheckDisconectedPlayers()
{
	for (BrawlersList::iterator it = waitList.begin(); it != waitList.end(); it++)
		if (!ObjectAccessor::FindPlayer(*it))
			RemovePlayer(*it);
}


void BrawlersGuild::UpdateBrawl(uint32 diff)
{
	switch (brawlstate)
	{
		case BRAWL_STATE_WAITING:
		{
			
			break;
		}

		case BRAWL_STATE_COMBAT:
		{

			break;
		}

		case BRAWL_STATE_TRANSITION:
		{

			break;
		}

		default:
			break;
	}
}


















// BrawlersGuildMgr

BrawlersGuildMgr::BrawlersGuildMgr()
{

}

BrawlersGuildMgr::~BrawlersGuildMgr()
{

}

void BrawlersGuildMgr::Update(uint32 diff)
{
    for(uint8 i=0; i<MAX_BRAWLERS_GUILDS; ++i)
        guilds[i].Update(diff);
}

void BrawlersGuildMgr::AddPlayer(Player *player)
{
    if(!player)
        return;

    guilds[player->GetTeamId()].AddPlayer(player);

}

void BrawlersGuildMgr::RemovePlayer(Player *player)
{
    if(!player)
        return;

    RemovePlayer(player->GetGUID());
}

void BrawlersGuildMgr::RemovePlayer(uint64 guid)
{
    for(uint8 i=0; i<MAX_BRAWLERS_GUILDS; ++i)
        guilds[i].RemovePlayer(guid);
}
