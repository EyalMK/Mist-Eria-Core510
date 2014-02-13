#include "SRASConnection.h"
#include "DatabaseEnv.h"

enum
{
    SEARCH_BY_NAME = 1,
    SEARCH_BY_ACCOUNT = 2,
    SEARCH_BY_IP = 3,
    SEARCH_BY_GUILD = 4,
    SEARCH_BY_ARENA_TEAM = 5,
    SEARCH_BY_SPELL = 6,
    SEARCH_BY_ITEM = 7,
    SEARCH_BY_DATE = 8,
    SEARCH_BY_PO = 9,
    SEARCH_BY_ACHIEVEMENT = 10
};

#define DB_CHAR 1
#define DB_AUTH 2

void SRASConnection::SearchQuery(SRASPacket pkt)
{
    pkt.next();
    int criteria = pkt.toInt();
    std::string query = pkt.next();

    CharacterDatabase.EscapeString(query);

    std::string sqlQuery;
    uint8 targetDb = 0;

    switch(criteria)
    {
        case SEARCH_BY_NAME:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT guid, name FROM characters WHERE name LIKE '%" + query + "%'";
            break;
        case SEARCH_BY_ACCOUNT:
            targetDb = DB_AUTH;
            sqlQuery = "SELECT id, username FROM account WHERE username LIKE '%" + query +"%'";
            break;
        case SEARCH_BY_IP:
            targetDb = DB_AUTH;
            sqlQuery = "SELECT id, username FROM account WHERE last_ip LIKE '%" + query +"%'";
            break;
        case SEARCH_BY_GUILD:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT guildid, name FROM guild WHERE name LIKE '%" + query + "%'";
            break;
        case SEARCH_BY_ARENA_TEAM:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT arenaTeamId, name FROM arena_team WHERE name LIKE '%"+query+"%'";
            break;
        case SEARCH_BY_SPELL:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT guid, name FROM characters WHERE guid IN (SELECT guid FROM character_spell WHERE spell='"+query+"')";
            break;
        case SEARCH_BY_ITEM:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT guid, name FROM characters WHERE guid IN (SELECT ci.guid FROM `character_inventory` AS ci INNER JOIN item_instance AS ii ON ci.item = ii.guid WHERE ii.itemEntry='"+query+"')";
            break;
        case SEARCH_BY_DATE:
            targetDb = DB_AUTH;
            sqlQuery = "SELECT id, username FROM account WHERE join_date > '"+query+"'";
            break;
        case SEARCH_BY_PO:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT guid, name FROM characters WHERE money >= '"+query+"'";
            break;
        case SEARCH_BY_ACHIEVEMENT:
            targetDb = DB_CHAR;
            sqlQuery = "SELECT guid, name FROM characters WHERE guid IN (SELECT guid FROM character_achievement WHERE achievement = '"+query+"')";
            break;
        default:
            SRASPacket resp;
            resp.add(SEARCH_RESPONSE);
            resp.add(0);
            SendPacket(resp.finalize());
            return;
            break;
    }

    if(!targetDb)
    {
        SRASPacket resp;
        resp.add(SEARCH_RESPONSE);
        resp.add(0);
        SendPacket(resp.finalize());
        return;
    }

    QueryResult searchResult;
    if(targetDb == DB_CHAR)
        searchResult = CharacterDatabase.Query(sqlQuery.c_str());
    else if(targetDb == DB_AUTH)
        searchResult = LoginDatabase.Query(sqlQuery.c_str());

    if(!searchResult)
    {
        SRASPacket resp;
        resp.add(SEARCH_RESPONSE);
        resp.add(0);
        SendPacket(resp.finalize());
        return;
    }

    SRASPacket resp;
    resp.add(SEARCH_RESPONSE);
    resp.add(searchResult->GetRowCount());
    resp.add(criteria);

    do
    {
        Field *f = searchResult->Fetch();
        uint32 id = f[0].GetUInt32();
        std::string name = f[1].GetString();

        resp.add(id);
        resp.add(name);
    }
    while (searchResult->NextRow());

    SendPacket(resp.finalize());
}
