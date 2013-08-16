#ifndef _SPELL_LEARN_MGR_H
#define _SPELL_LEARN_MGR_H

#include <ace/Singleton.h>
#include "Common.h"
#include "SharedDefines.h"
#include "spell.h"

class SpellLearnMgr
{
	friend class ACE_Singleton<SpellLearnMgr, ACE_Null_Mutex>;

private:
	SpellLearnMgr();
	~SpellLearnMgr();

	typedef std::list<uint32> SpellList;
	typedef std::map<uint32, SpellList*> SpecialisationList;
	typedef std::vector<SpecialisationList*> LevelsList;
	typedef std::map<uint32, LevelsList*> SpellLearnMap;

	typedef std::map<uint32, uint32> SpecializationSpecBySpell;


	SpellLearnMap sSpellLearnMap;
	SpecializationSpecBySpell sSpecializationMap;

public:
	void Load();

	uint32 GetSpecializationSpecBySpell(uint32 spellId);
};

#define sSpellLearnMgr ACE_Singleton<SpellLearnMgr, ACE_Null_Mutex>::instance()
#endif