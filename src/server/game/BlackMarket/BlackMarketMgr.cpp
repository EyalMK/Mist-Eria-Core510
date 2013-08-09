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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "BlackMarketMgr.h"
#include "Item.h"
#include "Language.h"
#include "Log.h"
#include <vector>


bool BMAuctionEntry::LoadFromDB(Field* fields)
{
    id = fields[0].GetUInt32();
    bidder = fields[8].GetUInt32();
    bid = fields[9].GetUInt32();
}

void BMAuctionEntry::SaveToDB(SQLTransaction& trans)
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_BLACKMARKET_AUCTION);
    stmt->setUInt32(0, id);
    stmt->setUInt32(1, templateId);
    stmt->setUInt32(2, starttime);
    stmt->setUInt32(3, bid);
    stmt->setUInt32(6, bidder);
    trans->Append(stmt);
}

void BMAuctionEntry::DeleteFromDB(SQLTransaction& trans)
{
	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BLACKMARKET_AUCTION);
	stmt->setUInt32(0, id);
	trans->Append(stmt);
}





BlackMarketMgr::BlackMarketMgr()
{
}

BlackMarketMgr::~BlackMarketMgr()
{
	/*
    for (ItemMap::iterator itr = mAitems.begin(); itr != mAitems.end(); ++itr)
        delete itr->second;
	*/
}

void BlackMarketMgr::DeleteExpiredAuctionsAtStartup()
{
    // Deletes expired auctions. Should be called at server start before loading auctions.

    // DO NOT USE after auctions are already loaded since this deletes from the DB
    //  and assumes the auctions HAVE NOT been loaded into a list or AuctionEntryMap yet

    uint32 oldMSTime = getMSTime();
    uint32 expirecount = 0;
    time_t curTime = sWorld->GetGameTime();

    // Query the DB to see if there are any expired auctions
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_BLACKMARKET_EXPIRED_AUCTIONS);
    stmt->setUInt32(0, (uint32)curTime+60);
    PreparedQueryResult expAuctions = CharacterDatabase.Query(stmt);

    if (!expAuctions)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> No expired blackmarket auctions to delete");

        return;
    }

    do
    {
        Field* fields = expAuctions->Fetch();

        BMAuctionEntry* auction = new BMAuctionEntry();

        // Can't use LoadFromDB() because it assumes the auction map is loaded
        if (!auction->LoadFromFieldList(fields))
        {
            // For some reason the record in the DB is broken (possibly corrupt
            //  faction info). Delete the object and move on.
            delete auction;
            continue;
        }

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        if (auction->bidder)
        {
            // Send the item to the winner
            sBlackMarketMgr->SendAuctionWonMail(auction, trans);
        }


        // Delete the auction from the DB
        auction->DeleteFromDB(trans);
        CharacterDatabase.CommitTransaction(trans);

        // Release memory
        delete auction;
        ++expirecount;

    } while (expAuctions->NextRow());

    sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Deleted %u expired auctions in %u ms", expirecount, GetMSTimeDiffToNow(oldMSTime));


}

void BlackMarketMgr::BuildBlackMarketAuctionsPacket(WorldPacket& data, uint32 guidLow)
{
	uint32 count = 0;

	data << uint32(1); // unk, Maybe "Hot Item!"
	data.WriteBits(count, 20); // placeholder

	for(BMAuctionEntryMap::const_iterator itr = GetAuctionsBegin(); itr != GetAuctionsEnd(); ++itr)
	{
		BMAuctionEntry* auction = itr->second;

		if (!auction->isActive)
			continue;

		data.WriteBit((guidLow == auction->bidder));
		++count;
	}

	for(BMAuctionEntryMap::const_iterator itr = GetAuctionsBegin(); itr != GetAuctionsEnd(); ++itr)
	{
		BMAuctionEntry* auction = itr->second;

		if (!auction->isActive)
			continue;

		data << uint32(auction->seller); //seller
		data << uint32(auction->TimeLeft()); //time left
		data << uint64(0); //unk
		data << uint64(0); //unk
		data << uint64(auction->bid); // price
		data << uint32(0); //unk
		data << uint32(0); //unk
		data << uint32(auction->itemCount); //stack count
		data << uint32(auction->itemEntry); //item id
		data << uint32(0); //unk
	}

	data.PutBits<uint32>(4, count, 20);




}