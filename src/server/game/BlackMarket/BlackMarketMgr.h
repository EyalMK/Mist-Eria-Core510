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

#ifndef _BLACK_MARKET_MGR_H
#define _BLACK_MARKET_MGR_H

#include <ace/Singleton.h>

#include "Common.h"
#include "DatabaseEnv.h"
#include "DBCStructure.h"

class Item;
class Player;
class WorldPacket;

#define MAX_AUCTION_ITEMS 160

enum BMAuctionError
{
    ERR_AUCTION_OK                  = 0,
    ERR_AUCTION_INVENTORY           = 1,
    ERR_AUCTION_DATABASE_ERROR      = 2,
    ERR_AUCTION_NOT_ENOUGHT_MONEY   = 3,
    ERR_AUCTION_ITEM_NOT_FOUND      = 4,
    ERR_AUCTION_HIGHER_BID          = 5,
    ERR_AUCTION_BID_INCREMENT       = 7,
    ERR_AUCTION_BID_OWN             = 10,
    ERR_RESTRICTED_ACCOUNT          = 13,
};

enum BMAuctionAction
{
    AUCTION_SELL_ITEM = 0,
    AUCTION_CANCEL = 1,
    AUCTION_PLACE_BID = 2
};

enum BMMailAuctionAnswers
{
    AUCTION_OUTBIDDED           = 0,
    AUCTION_WON                 = 1,
    AUCTION_SUCCESSFUL          = 2,
    AUCTION_EXPIRED             = 3,
    AUCTION_CANCELLED_TO_BIDDER = 4,
    AUCTION_CANCELED            = 5,
    AUCTION_SALE_PENDING        = 6
};

struct BMAuctionEntry
{
    uint32 id;
	uint32 templateId;
    uint32 itemEntry;
    uint32 itemCount;
    uint32 seller;
    uint32 startTime;
	uint32 duration;
	uint32 bid;
    uint32 bidder;
	bool isActive;

    // helpers
    void DeleteFromDB(SQLTransaction& trans);
    void SaveToDB(SQLTransaction& trans);
    bool LoadFromDB(Field* fields);
	uint32 TimeLeft() { return startTime + duration - time(NULL); }

};


class BlackMarketMgr
{
    friend class ACE_Singleton<BlackMarketMgr, ACE_Null_Mutex>;

    private:
        BlackMarketMgr();
        ~BlackMarketMgr();

		typedef std::map<uint32, BMAuctionEntry*> BMAuctionEntryMap;

		BMAuctionEntryMap BMAuctionsMap;

	public:

		BMAuctionEntry* GetAuction(uint32 id) const
		{
			BMAuctionEntryMap::const_iterator itr = BMAuctionsMap.find(id);
			return itr != BMAuctionsMap.end() ? itr->second : NULL;
		}

		uint32 GetAuctionCount() { return BMAuctionsMap.size(); }

		BMAuctionEntryMap::iterator GetAuctionsBegin() { return BMAuctionsMap.begin(); }
		BMAuctionEntryMap::iterator GetAuctionsEnd() { return BMAuctionsMap.end(); }


        //auction messages
        void SendAuctionWonMail(BMAuctionEntry* auction, SQLTransaction& trans);
        void SendAuctionOutbiddedMail(BMAuctionEntry* auction, uint32 newPrice, Player* newBidder, SQLTransaction& trans);


        // Used primarily at server start to avoid loading a list of expired auctions
        void DeleteExpiredAuctionsAtStartup();

        void LoadAuctions();

		void AddAuction(BMAuctionEntry* auction);

        void Update();

		void BuildBlackMarketAuctionsPacket(WorldPacket& data, uint32 guidLow);

    private:
		
};

#define sBlackMarketMgr ACE_Singleton<BlackMarketMgr, ACE_Null_Mutex>::instance()

#endif
