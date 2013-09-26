#include "MasteryMgr.h"

void MasteryMgr::LoadFromDB()
{
	uint32 oldMSTime = getMSTime();

	PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_MASTERIES);
	PreparedQueryResult result = WorldDatabase.Query(stmt);

	if (!result)
    {
        sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded 0 Masteries. DB table `masteries` is empty.");
        return;
    }

	uint32 count = 0;

	do
    {
        Field* fields = result->Fetch();

		uint32 id = fields[0].GetInt32();
		float base = fields[1].GetFloat();
		float ratio = fields[2].GetFloat();

		masteriesMap[(TalentTree)id] = Mastery(base, ratio);

        ++count;
    } while (result->NextRow());

	sLog->outInfo(LOG_FILTER_SERVER_LOADING, ">> Loaded %u Masteries in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}


Mastery MasteryMgr::getMastery(TalentTree specId)
{
	std::map<TalentTree, Mastery>::const_iterator itr = masteriesMap.find(specId);
	if(itr != masteriesMap.end())
	{
		return itr->second;
	}
	else // Not loaded Mastery
	{
		return Mastery(0.0f, 0.0f);
	}
}