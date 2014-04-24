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

#ifndef _BRAWLERS_GUILD_MGR_H
#define _BRAWLERS_GUILD_MGR_H

#include <ace/Singleton.h>

#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCStructure.h"

class Player;


enum BrawlersSpells
{
	SPELL_QUEUED_FOR_BRAWL = 132639,
	SPELL_ARENA_TELEPORTATION = 105315,
	SPELL_ALLIANCE_SOUND = 136144,
	SPELL_HORDE_SOUND = 136143
};

enum BrawlersAchievement
{
	ACHIEVEMENT_FIRST_RULE_A = 7947,
	ACHIEVEMENT_FIRST_RULE_H = 7948
};

enum BrawlersGuilds
{
    ALLIANCE_GUILD = 0,
    HORDE_GUILD,
    MAX_BRAWLERS_GUILDS
};

enum BrawlersTeleports
{
    ARENA = 0,
    OUTSIDE,
    MAX_TELEPORTS
};

enum BrawlersStates
{
	BRAWL_STATE_WAITING = 0,
	BRAWL_STATE_PREPARE_COMBAT,
	BRAWL_STATE_COMBAT,
	BRAWL_STATE_TRANSITION
};

#define MAX_BRAWLERS_RANK 10
#define BOSS_PER_RANK 4
#define MAX_BRAWLERS_REPUTATION 10000

#define BrawlersList std::list<uint64>

class BrawlersGuild
{
    public:
        BrawlersGuild(uint32 _id);
        ~BrawlersGuild();

        void Update(uint32 diff);

        void AddPlayer(Player *player);

        void RemovePlayer(Player *player);
        void RemovePlayer(uint64 guid);

		void BossReport(uint64 guid, bool win);

    private:

		uint32 id;

        void UpdateAura(Player* player, uint32 rank);
		void UpdateAllAuras();

		void CheckDisconectedPlayers();
		void RemovePlayers();

		void UpdateBrawl(uint32 diff);

		void PrepareCombat();
		void StartCombat();
		void EndCombat(bool win);

		void RewardPlayer(Player *player);

        BrawlersList waitList;
        BrawlersList removeList;

		BrawlersStates brawlstate;


		//Combat

		uint64 current;
		int32 prepareCombatTimer;
		int32 combatTimer;
		int32 transitionTimer;
};






class BrawlersGuildMgr
{
    friend class ACE_Singleton<BrawlersGuildMgr, ACE_Null_Mutex>;

    private:
        BrawlersGuildMgr();
        ~BrawlersGuildMgr();

	public:

        void Update(uint32 diff);

        void AddPlayer(Player *player);

        void RemovePlayer(Player *player);
		void RemovePlayer(uint64 guid);

		void BossReport(uint64 guid, bool win);

    private:

        BrawlersGuild *guilds[MAX_BRAWLERS_GUILDS];
		
};

#define sBrawlersGuildMgr ACE_Singleton<BrawlersGuildMgr, ACE_Null_Mutex>::instance()

#endif
