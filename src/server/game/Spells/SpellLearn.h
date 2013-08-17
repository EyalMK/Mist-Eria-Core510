#ifndef _SPELL_LEARN_MGR_H
#define _SPELL_LEARN_MGR_H

#include <ace/Singleton.h>
#include "Common.h"
#include "SharedDefines.h"
#include "Spell.h"
#include "Player.h"

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
	typedef std::map<uint32, std::list<uint32> > SpecializationSpecByClass;


	SpellLearnMap sSpellLearnMap;
	SpecializationSpecByClass sSpecializationMap;

public:
	void Load();
	std::list<uint32> GetSpellList(uint32 classe, uint32 spec, uint32 levelMin, uint32 levelMax, bool withCommon);
	std::list<uint32> GetSpellList(uint32 classe, uint32 spec, uint32 level, bool withCommon);
	void PlayerLevelUp(Player* player);
};

#define sSpellLearnMgr ACE_Singleton<SpellLearnMgr, ACE_Null_Mutex>::instance()
#endif