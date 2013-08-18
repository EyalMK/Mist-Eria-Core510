#include "SpellLearn.h"
#include "SpellInfo.h"
#include "DBCStores.h"
#include "SpellMgr.h"
#include "World.h"

SpellLearnMgr::SpellLearnMgr()
{
}

SpellLearnMgr::~SpellLearnMgr()
{
}

void SpellLearnMgr::Load()
{
	for(uint32 i = 0 ; i < sChrSpecializationStore.GetNumRows() ; i++)
	{
		if(ChrSpecializationEntry const* specialisation = sChrSpecializationStore.LookupEntry(i))
		{
			sSpecializationMap[specialisation->ClassId].push_back(specialisation->Id);
		}
	}

	for(uint32 i = 0 ; i < sChrClassesStore.GetNumRows() ; i++)
	{
		if(ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(i))
		{
			//common branch without any spec
			sSpecializationMap[classEntry->ClassID].push_back(0);

			sSpellLearnMap[classEntry->ClassID] = new LevelsList;
			for(uint32 y = 0 ; y < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) ; y++)
			{
				sSpellLearnMap[classEntry->ClassID]->push_back(new SpecialisationList);
				
				for( std::list<uint32>::iterator itr = sSpecializationMap[classEntry->ClassID].begin() ; itr != sSpecializationMap[classEntry->ClassID].end() ; itr++ )
				{
					(*((*(sSpellLearnMap[classEntry->ClassID]))[y]))[*itr] = new SpellList;
				}
			}
		}
	}

	uint32 oldMSTime = getMSTime();

	PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_SPELLS_BY_LEVELS);
	PreparedQueryResult result = WorldDatabase.Query(stmt);

	if (!result)
	{
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 spells to learn. DB table `spell_to_learn` is empty.");
		return;
	}

	uint32 count = 0;
	do
	{
		Field* fields           = result->Fetch();
		uint32 spellId			= fields[0].GetUInt32();
		uint32 classId      	= fields[1].GetUInt32();
		uint32 specId       	= fields[2].GetUInt32();
		uint32 level     		= fields[3].GetUInt32();

		((*((*(sSpellLearnMap[classId]))[level-1]))[specId])->push_back(spellId);

		++count;
	} while (result->NextRow());

	sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u spells to learn in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}


std::list<uint32> SpellLearnMgr::GetSpellList(uint32 classe, uint32 spec, uint32 levelMin, uint32 levelMax, bool withCommon)
{
	std::list<uint32> result;

	if(levelMin < 0) levelMin = 0;
	if(levelMax < 0) levelMax = 0;
	if(levelMin > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) levelMin = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
	if(levelMax > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) levelMax = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

	if(ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(classe))
	{
		for(uint32 i = levelMin ; i <= levelMax ; i++)
		{
			result.insert(result.end(), (*((*((*(sSpellLearnMap[classEntry->ClassID]))[i-1]))[spec])).begin(), (*((*((*(sSpellLearnMap[classEntry->ClassID]))[i-1]))[spec])).end());
			if(withCommon)
				result.insert(result.end(), (*((*((*(sSpellLearnMap[classEntry->ClassID]))[i-1]))[0])).begin(), (*((*((*(sSpellLearnMap[classEntry->ClassID]))[i-1]))[0])).end());
		}
	}
	return result;
}

std::list<uint32> SpellLearnMgr::GetSpellList(uint32 classe, uint32 spec, uint32 level, bool withCommon)
{
	return GetSpellList(classe, spec, level, level, withCommon);
}

void SpellLearnMgr::PlayerLevelUp(Player* player)
{
	if(!player) return;
	std::list<uint32> list = GetSpellList(player->getClass(), player->GetActiveSpec(), 1, player->getLevel(), true);
	if(!list.empty())
	{
		std::list<uint32>::const_iterator itr = list.begin();
		for(; itr != list.end() ; itr++)
		{
			if(!player->HasSpell(*itr)) player->learnSpell(*itr,true);
		}
	}
}