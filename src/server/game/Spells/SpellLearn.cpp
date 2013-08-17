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
			if(ChrClassesEntry const* classe = sChrClassesStore.LookupEntry(specialisation->ClassId))
			{
				sSpecializationSpecMap[classe->spellfamily].push_back(specialisation->Id);
			}
		}
	}

	for(uint32 i = 0 ; i < sChrClassesStore.GetNumRows() ; i++)
	{
		if(ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(i))
		{
			//common branch without any spec
			sSpecializationSpecMap[classEntry->spellfamily].push_back(0);

			sSpellLearnMap[classEntry->spellfamily] = new LevelsList;
			for(uint32 y = 0 ; y < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) ; y++)
			{
				sSpellLearnMap[classEntry->spellfamily]->push_back(new SpecialisationList);
				
				for( std::list<uint32>::iterator itr = sSpecializationSpecMap[classEntry->spellfamily].begin() ; itr != sSpecializationSpecMap[classEntry->spellfamily].end() ; itr++ )
				{
					(*((*(sSpellLearnMap[classEntry->spellfamily]))[y]))[*itr] = new SpellList;
				}
			}
		}
	}

	for(uint32 i = 0 ; i < sSpellStore.GetNumRows() ; i++)
	{
		if(SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i))
		{
			uint32 spellFamily = spellInfo->SpellFamilyName;
			if((spellFamily < SPELLFAMILY_MAGE || spellFamily > SPELLFAMILY_SHAMAN) && spellFamily != SPELLFAMILY_DEATHKNIGHT && spellFamily != SPELLFAMILY_MONK) continue;
			uint32 level = spellInfo->SpellLevel - 1;
			if(level < 0 || level > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL) - 1) continue;
			uint32 specialisationId = GetSpecializationSpecBySpell(i);


			((*((*(sSpellLearnMap[spellFamily]))[level]))[specialisationId])->push_back(spellInfo->Id);
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
	if(ChrClassesEntry const* classEntry = sChrClassesStore.LookupEntry(classe))
	{
		uint32 spellfamily = classEntry->spellfamily;
		for(uint32 i = levelMin ; i <= levelMax ; i++)
		{
			result.insert(result.end(), (*((*((*(sSpellLearnMap[spellfamily]))[i]))[spec])).begin(), (*((*((*(sSpellLearnMap[spellfamily]))[i]))[spec])).end());
			if(withCommon)
				result.insert(result.end(), (*((*((*(sSpellLearnMap[spellfamily]))[i]))[0])).begin(), (*((*((*(sSpellLearnMap[spellfamily]))[i]))[0])).end());
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