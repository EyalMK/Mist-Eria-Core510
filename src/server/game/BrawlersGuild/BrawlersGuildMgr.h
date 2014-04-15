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


enum BrawlerSpells
{
	SPELL_QUEUED_FOR_BRAWL = 132639,
	SPELL_ARENA_TELEPORTATION = 105315
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

float BrawlersTeleportLocations[MAX_BRAWLERS_GUILDS][MAX_TELEPORTS][3] =
{
    {{-121.f, 2499.f, -57.f},{-89.f, 2476.f, -43.f}},
    {{2032.f, -4753.f, 87.f},{2062.f, 4547.f, 87.f}}
};

#define BrawlersList std::list<uint64>



class BrawlersGuild
{
    public:
        BrawlersGuild();
        ~BrawlersGuild();

        void Update(uint32 diff);

        void AddPlayer(Player *player);

        void RemovePlayer(Player *player);
        void RemovePlayer(uint64 guid);


    private:

        void UpdateAura(Player* player, uint32 rank);
		void UpdateAllAuras();

        BrawlersList waitList;
        BrawlersList removeList;
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

    private:

        BrawlersGuild guilds[MAX_BRAWLERS_GUILDS];
		
};

#define sBrawlersGuildMgr ACE_Singleton<BrawlersGuildMgr, ACE_Null_Mutex>::instance()

#endif
