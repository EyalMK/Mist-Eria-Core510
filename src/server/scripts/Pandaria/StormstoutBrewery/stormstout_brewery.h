#ifndef STORMSTOUT_BREWERY_H
#define STORMSTOUT_BREWERY_H

#include "ScriptPCH.h" // Generic including, better than always including in each .cpp

#define MAX_ENCOUNTERS 3

inline void LogFunction(const char* name, bool enter = true)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : %s %s", enter ? "Entering" : "Leaving", name);
}

typedef std::list<Creature*> CreatureList ;
typedef std::list<Creature*>::iterator CreatureListIter ;
typedef std::list<Creature*>::const_iterator CreatureListConstIter ;

/**********************************/
/************ENTITIES**************/
/**********************************/
enum Creatures
{
    BOSS_OOK_OOK    = 56637,
    BOSS_HOPTALLUS  = 56717,
    BOSS_YAN_ZHU    = 59479,

    NPC_CHEN_STORMSTOUT     = 59704,
    NPC_ROLLING_BARREL      = 56682,
    VEHICLE_ROLLING_BARREL  = 0 /// @todo : This is a placeholder, unluckily
};

// Doors and others
enum GameObjects
{
    GOB_OOK_OOK_ENTRANCE    = 200002,
    GOB_HOPTALLUS_ENTRANCE  = 200003,
    GOB_HOPTALLUS_EXIT      = 200004,
    GOB_YAN_ZHU_ENTRANCE    = 200005
};

// For quests
enum Items
{

};

/**********************************/
/***********MISCELLANOUS***********/
/**********************************/
enum Spells
{
    SPELL_BANANA_BAR = 107297
};

enum InstanceEvents
{
    INSTANCE_EVENT_SUMMON_BARREL_0  = 0,
    INSTANCE_EVENT_SUMMON_BARREL_1  = 1,
    INSTANCE_EVENT_SUMMON_BARREL_2  = 1,
    INSTANCE_EVENT_SUMMON_BARREL_3  = 3,
    INSTANCE_EVENT_SUMMON_BARREL_4  = 4,
    INSTANCE_EVENT_SUMMON_BARREL_5  = 5,
    INSTANCE_EVENT_SUMMON_ALL       = 6
};

/**********************************/
/***************DATAS**************/
/**********************************/
enum InstanceDatas // Aka, data32
{
    INSTANCE_DATA_OOK_OOK_STATUS    = 0, // Ook Ook Fight
    INSTANCE_DATA_HOPTALLUS_STATUS  = 1, // Hoptallus Fight
    INSTANCE_DATA_YAN_ZHU_STATUS    = 2, // Yan Zhu Fight
    INSTANCE_DATA_UNCLE_GAO_WAVE    = 3 // Eventually
};

enum InstanceDatas64 // aka guids
{
    INSTANCE_DATA64_OOK_OOK_GUID    = 0,
    INSTANCE_DATA64_HOPTALLUS_GUID  = 1,
    INSTANCE_DATA64_YAN_ZHU_GUID    = 3,

    INSTANCE_DATA64_OOK_OOK_ENTRANCE_GUID   = 4, // Big enormous barrel
    INSTANCE_DATA64_HOPTALLUS_ENTRANCE_GUID = 5, // Standart door
    INSTANCE_DATA64_HOPTALLUS_EXIT_GUID     = 6, // Big giant carrot
    INSTANCE_DATA64_YAN_ZHU_ENTRANCE_GUID   = 7, // Standart door

    INSTANCE_DATA64_KILLED_HOZENS               = 8,
    INSTANCE_DATA64_KILLED_HOPTALLUS_TRASH      = 9,
    INSTANCE_DATA64_SUMMONED_HOPTALLUS_TRASH    = 10
};

enum NpcsDatas
{
    NPC_ROLLING_BARREL_DATA_SUMMONING_ID    = 0,
    BOSS_YAN_ZHU_DATA_ALE_SPELL             = 1,
    BOSS_YAN_ZHU_DATA_STOUT_SPELL           = 2,
    BOSS_YAN_ZHU_DATA_WHEAT_SPELL           = 3
};

#endif // STORMSTOUT_BREWERY_H
