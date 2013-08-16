#include "SpellLearn.h"
#include "SpellInfo.h"
#include "DBCStores.h"
#include "SpellMgr.h"

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
			sSpecializationMap.insert(specialisationSpell->SpellId, specialisationSpell->SpecId);
		}
	}

	for(uint32 i = 0 ; i < sSpellStore.GetNumRows() ; i++)
	{
		if(SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(i))
		{
			uint32 classId = spellInfo->SpellFamilyName;
			uint32 level = spellInfo->SpellLevel;
			uint32 specialisationId = GetSpecializationSpecBySpell(i);


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
