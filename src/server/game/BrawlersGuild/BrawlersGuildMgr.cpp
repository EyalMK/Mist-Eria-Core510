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

int32 BrawlersTeleportLocations[MAX_BRAWLERS_GUILDS][MAX_TELEPORTS][4] =
{
	{ { 369, -121, 2499, -57 }, { 369, -89, 2476, -43 } },
	{ { 1043, 2032, -4753, 87 }, { 1043, 2062, 4547, 87 } }
};

uint32 BrawlersBoss[MAX_BRAWLERS_RANK][BOSS_PER_RANK] =
{
	{ 67262, 67579, 68257, 67572 },
	{ 68255, 67269, 67525, 68254 },
	{ 67594, 67268, 68253, 68251 },
	{ 67540, 68377, 67485, 67451 },
	{ 67591, 67596, 68252, 67516 },
	{ 68256, 67588, 68305, 68250 },
	{ 67487, 67678, 67424, 67573 },
	{ 70659, 70733, 70983, 70977 },
	{ 70694, 70656, 70739, 70068 },
	{ 67490, 67483, 67571, 67518 },
};

uint32 BrawlersFaction[MAX_BRAWLERS_GUILDS] =
{
	1419, 1374
};

BrawlersGuild::BrawlersGuild(uint32 _id)
{
	id = _id;
	brawlstate = BRAWL_STATE_WAITING;
	current = 0;
}

BrawlersGuild::~BrawlersGuild()
{

}

void BrawlersGuild::Update(uint32 diff)
{
	CheckDisconectedPlayers();

	bool needUpdateAura = !removeList.empty();

	RemovePlayers();
		 
	removeList.clear();

	if (needUpdateAura)
		UpdateAllAuras();

	UpdateBrawl(diff);

}

void BrawlersGuild::AddPlayer(Player *player)
{
    if(!player)
        return;

    waitList.push_back(player->GetGUID());
    UpdateAura(player, waitList.size());
}

void BrawlersGuild::RemovePlayers()
{
	for (BrawlersList::iterator it = removeList.begin(); it != removeList.end(); it++)
	{
		waitList.remove(*it);
		if (Player *player = ObjectAccessor::FindPlayer(*it))
		{
			player->RemoveAura(SPELL_QUEUED_FOR_BRAWL);
			ChatHandler(player->GetSession()).PSendSysMessage("Vous n'etes plus dans la file pour une baston");
		}
			
	}
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

	if (rank <= 3 || rank == waitList.size())
	{
		std::stringstream ss;
		ss << "Place dans la file d'attente pour un combat : " << rank;
		if (rank == 1)
			ss << "\nVous etes le prochain !";
		ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());
	}
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

		if (current && !!ObjectAccessor::FindPlayer(current))
			EndCombat(false);
}


void BrawlersGuild::UpdateBrawl(uint32 diff)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "diff %d", diff);
	switch (brawlstate)
	{
		case BRAWL_STATE_WAITING:
		{
			if (!waitList.empty())
				PrepareCombat();
			break;
		}

		case BRAWL_STATE_PREPARE_COMBAT:
		{
			sLog->outDebug(LOG_FILTER_NETWORKIO, "prepareCombatTimer %d", prepareCombatTimer);
			if (prepareCombatTimer <= 0)
				StartCombat();
			else
				prepareCombatTimer -= diff;
			break;
		}

		case BRAWL_STATE_COMBAT:
		{
			sLog->outDebug(LOG_FILTER_NETWORKIO, "combatTimer %d", combatTimer);
			if (combatTimer <= 0)
				EndCombat(false);
			else
				combatTimer -= diff;
			break;
		}

		case BRAWL_STATE_TRANSITION:
		{
			sLog->outDebug(LOG_FILTER_NETWORKIO, "transitionTimer %d", transitionTimer);
			if (transitionTimer <= 0)
			   brawlstate = BRAWL_STATE_WAITING;
			else
				transitionTimer -= diff;
			break;
		}

		default:
			break;
	}
}

void BrawlersGuild::PrepareCombat()
{
	

	if (waitList.empty())
		return;

	current = waitList.front();
	RemovePlayer(current);

	if (Player *player = ObjectAccessor::FindPlayer(current))
	{
		ChatHandler(player->GetSession()).PSendSysMessage("PrepareCombat");

		player->TeleportTo(BrawlersTeleportLocations[id][ARENA][0], BrawlersTeleportLocations[id][ARENA][1], BrawlersTeleportLocations[id][ARENA][2], BrawlersTeleportLocations[id][ARENA][3], 0.f);
		player->CastSpell(player, SPELL_ARENA_TELEPORTATION, true);
		player->RemoveAura(SPELL_QUEUED_FOR_BRAWL);
		prepareCombatTimer = 5000;
		brawlstate = BRAWL_STATE_PREPARE_COMBAT;
	}
	else
		return;
}

void BrawlersGuild::StartCombat()
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "START COMBAT, guid %ld", current);

	if (Player *player = ObjectAccessor::FindPlayer(current))
	{
		ChatHandler(player->GetSession()).PSendSysMessage("StartCombat");

		if (uint32 entry = GetBossForPlayer(player))
			player->SummonCreature(entry, BrawlersTeleportLocations[id][ARENA][0], BrawlersTeleportLocations[id][ARENA][1], BrawlersTeleportLocations[id][ARENA][2], BrawlersTeleportLocations[id][ARENA][3], TEMPSUMMON_TIMED_DESPAWN, 125000);
		combatTimer = 120000;
		brawlstate = BRAWL_STATE_COMBAT;
	}
	else
	{
		EndCombat(false);
		return;
	}
}

void BrawlersGuild::EndCombat(bool win)
{

	if (Player *player = ObjectAccessor::FindPlayer(current))
	{
		ChatHandler(player->GetSession()).PSendSysMessage("EndCombat");

		player->TeleportTo(BrawlersTeleportLocations[id][OUTSIDE][0], BrawlersTeleportLocations[id][OUTSIDE][1], BrawlersTeleportLocations[id][OUTSIDE][2], BrawlersTeleportLocations[id][OUTSIDE][3], 0.f);
		player->CastSpell(player, SPELL_ARENA_TELEPORTATION, true);
		transitionTimer = 5000;


		if (win)
			RewardPlayer(player);
	}

	current = 0;
	brawlstate = BRAWL_STATE_TRANSITION;
}

uint32 BrawlersGuild::GetPlayerRank(Player *player)
{
	if (!player)
		return 0;

	std::stringstream ss;
	ss << "GetRank : rep " << player->GetReputation(BrawlersFaction[player->GetTeamId()]) << ", rank " << player->GetReputation(BrawlersFaction[player->GetTeamId()]) / (REPUTATION_PER_RANK*BOSS_PER_RANK);
	ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());

	return player->GetReputation(BrawlersFaction[player->GetTeamId()]) / (REPUTATION_PER_RANK*BOSS_PER_RANK);
}

uint32 BrawlersGuild::GetPlayerSubRank(Player *player)
{
	if (!player)
		return 0;

	std::stringstream ss;
	ss << "GetSubRank : rep " << player->GetReputation(BrawlersFaction[player->GetTeamId()]) << ", subrank " << (player->GetReputation(BrawlersFaction[player->GetTeamId()]) / REPUTATION_PER_RANK) % BOSS_PER_RANK;
	ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());

	return (player->GetReputation(BrawlersFaction[player->GetTeamId()]) / REPUTATION_PER_RANK) % BOSS_PER_RANK;
}

uint32 BrawlersGuild::GetBossForPlayer(Player *player)
{
	if (!player)
		return 0;

	uint32 rank = GetPlayerRank(player);
	uint32 subrank = GetPlayerSubRank(player);

	std::stringstream ss;
	ss << "GetBossForPlayer : rank " << rank << ", subrank " << subrank;
	ChatHandler(player->GetSession()).PSendSysMessage(ss.str().c_str());

	if (rank < MAX_BRAWLERS_RANK)
		return BrawlersBoss[rank][subrank];
	
	return 0;
}

void BrawlersGuild::RewardPlayer(Player *player)
{
	if (!player)
		return;

	player->AddItem(92718, 1);

	uint32 rep = player->GetReputation(BrawlersFaction[player->GetTeamId()]);

	rep += REPUTATION_PER_RANK;
	if (rep > MAX_BRAWLERS_REPUTATION)
		rep = MAX_BRAWLERS_REPUTATION;

	player->SetReputation(BrawlersFaction[player->GetTeamId()], rep);
}

void BrawlersGuild::BossReport(uint64 guid, bool win)
{
	if (current && current == guid)
		EndCombat(win);
}

bool BrawlersGuild::IsPlayerInBrawl(Player* player)
{
	if (!player || !current)
		return false;

	return player->GetGUID() == current;
}















// BrawlersGuildMgr

BrawlersGuildMgr::BrawlersGuildMgr()
{
	guilds[ALLIANCE_GUILD] = new BrawlersGuild(ALLIANCE_GUILD);
	guilds[HORDE_GUILD] = new BrawlersGuild(HORDE_GUILD);
}

BrawlersGuildMgr::~BrawlersGuildMgr()
{
	for (uint8 i; i < MAX_BRAWLERS_GUILDS; ++i)
		delete guilds[i];
}

void BrawlersGuildMgr::Update(uint32 diff)
{
    for(uint8 i=0; i<MAX_BRAWLERS_GUILDS; ++i)
        guilds[i]->Update(diff);
}

void BrawlersGuildMgr::AddPlayer(Player *player)
{
    if(!player)
        return;

    guilds[player->GetTeamId()]->AddPlayer(player);

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
        guilds[i]->RemovePlayer(guid);
}

void BrawlersGuildMgr::BossReport(uint64 guid, bool win)
{
	for (uint8 i = 0; i<MAX_BRAWLERS_GUILDS; ++i)
		guilds[i]->BossReport(guid, win);
}

bool BrawlersGuildMgr::IsPlayerInBrawl(Player* player)
{
	if (!player)
		return false;

	for (uint8 i = 0; i < MAX_BRAWLERS_GUILDS; ++i)
		if (guilds[i]->IsPlayerInBrawl(player))
			return true;

		return false;
}