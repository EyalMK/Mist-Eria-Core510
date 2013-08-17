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
	for(uint32 i = 0 ; i < sSpecializationSpellStorage.GetNumRows() ; i++)
	{
		if(SpecializationSpellEntry const* specialisationSpell = sSpecializationSpellStorage.LookupEntry(i))
		{
			sSpecializationMap[specialisationSpell->SpellId] = specialisationSpell->SpecId;
		}
	}

	for(uint32 i = 0 ; i < sChrSpecializationStore.GetNumRows() ; i++)
	{
		if(ChrSpecializationEntry const* specialisation = sChrSpecializationStore.LookupEntry(i))
		{
			sSpecializationSpecMap[specialisation->ClassId].push_back(specialisation->Id);
		}
	}

	for(uint32 i = 0 ; i < sChrClassesStore.GetNumRows() ; i++)
	{
		if(ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(i))
		{
			//common branch without any spec
			sSpecializationSpecMap[i].push_back(0);

			sSpellLearnMap[classEntry->ClassID] = new LevelsList;
			for(uint32 y = 0 ; y < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) ; i++)
			{
				sSpellLearnMap[classEntry->ClassID]->push_back(new SpecialisationList);
				
				for( std::list<uint32>::iterator itr = sSpecializationSpecMap[classEntry->ClassID].begin() ; itr != sSpecializationSpecMap[classEntry->ClassID].end() ; itr++ )
				{
					(*((*(sSpellLearnMap[classEntry->ClassID]))[y]))[*itr] = new SpellList;
				}
			}
		}
	}

	for(uint32 i = 0 ; i < sSpellStore.GetNumRows() ; i++)
	{
		if(SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i))
		{
			uint32 classId = spellInfo->SpellFamilyName;
			uint32 level = spellInfo->SpellLevel;
			uint32 specialisationId = GetSpecializationSpecBySpell(i);

			(*((*((*(sSpellLearnMap[classId]))[level]))[specialisationId])).push_back(spellInfo->Id);
		}
	}
}

uint32 SpellLearnMgr::GetSpecializationSpecBySpell(uint32 spellId)
{
	SpecializationSpecBySpell::const_iterator itr = sSpecializationMap.find(spellId);
	if (itr == sSpecializationMap.end())
		return 0;

	return itr->second;
}


std::list<uint32> SpellLearnMgr::GetSpellList(uint32 classe, uint32 spec, uint32 levelMin, uint32 levelMax, bool withCommon)
{
	std::list<uint32> result;
	for(uint32 i = levelMin ; i <= levelMax ; i++)
	{
		result.insert(result.end(), (*((*((*(sSpellLearnMap[classe]))[i]))[spec])).begin(), (*((*((*(sSpellLearnMap[classe]))[i]))[spec])).end());
		if(withCommon)
			result.insert(result.end(), (*((*((*(sSpellLearnMap[classe]))[i]))[0])).begin(), (*((*((*(sSpellLearnMap[classe]))[i]))[0])).end());
	}
	return result;
}

std::list<uint32> SpellLearnMgr::GetSpellList(uint32 classe, uint32 spec, uint32 level, bool withCommon)
{
	std::list<uint32> result;
	result.insert(result.end(), (*((*((*(sSpellLearnMap[classe]))[level]))[spec])).begin(), (*((*((*(sSpellLearnMap[classe]))[level]))[spec])).end());
	if(withCommon) result.insert(result.end(), (*((*((*(sSpellLearnMap[classe]))[level]))[0])).begin(), (*((*((*(sSpellLearnMap[classe]))[level]))[0])).end());
	return result;
}

void SpellLearnMgr::PlayerLevelUp(Player* player)
{
	if(!player) return;
	std::list<uint32> list = GetSpellList(player->getClass(), player->GetActiveSpec(), player->getLevel(), true);
	if(!list.empty())
	{
		std::list<uint32>::const_iterator itr = list.begin();
		for(; itr != list.end() ; itr++)
		{
			player->learnSpell(*itr,true);
		}
	}
	
}
