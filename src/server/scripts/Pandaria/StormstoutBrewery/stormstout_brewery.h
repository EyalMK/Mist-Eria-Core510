#ifndef STORMSTOUT_BREWERY_H
#define STORMSTOUT_BREWERY_H

#include "ScriptPCH.h"

// Max Encounters in the instance
//! First encounter : OokOok PreEvent ; second : OokOok Event
#define MAX_ENCOUNTER 4

// Max number of different npcs that when killed increase the killed hozen counter
#define MAX_OOKOOK_ENTOURAGE 6

// Max number of alamental that can be summoned
#define MAX_ALAMENTAL 3

// Creatures entries we will need to access
enum InstanceCreatures
{
    // Bosses
    Boss_OokOok = 56637,
    Boss_Hoptallus = 56717,
    Boss_YanZhutheUncasked = 59479,


    // Alamentals

    // Stout ability
    Mob_SudsyBrewAlamental = 59522,
    Mob_FizzyBrewAlamental = 59520,

    // Ale ability
    Mob_BubblingBrewAlamental = 59521,
    Mob_YeasyBrewAlamental = 66413,

    // Wheat ability
    Mob_StoutBrewAlamental = 59519,
    Mob_BloatedBrewAlamental = 59518


    // Miscellanous NPCS
};

// GameObjects entries we will need to access
enum InstanceGameObjects
{
    // Doors of OokOok
    GameObject_OokOokEntranceDoor = 0,
    GameObject_OokOokExitDoor = 1,

    // Doors of Hoptallus
    GameObject_HoptallusEntranceDoor = 2,
    GameObject_HoptallusExitDoor = 3,

    // Door of YanZhu
    GameObject_YanZhuEntranceDoor = 4


    // Miscellanous GameObjects
};

// Spells entries we need at any time, such as the power bar of OokOok
enum InstanceSpells
{
    Spell_EnableBananaBar = 107297 // Each time a player enter the instance AND if OokOok event has not started, we apply this on the player
};

// Just in case (Hotpallus pre event ?)
enum InstanceActions
{
    Action_StartHoptallusEvent = 0
};

// Identifier for each encounter and global event that occurs in the instance, such as the number of hozen killed
enum InstanceDatas
{
    Data_OokOokPartyEventProgress = 0,
    Data_OokOokEventProgress = 1,
    Data_HoptallusEventProgress = 2,
    Data_YanZhuEventProgress = 3,

    // Alamentals
    Data_AleAlamental = 4,
    Data_WheatAlamental = 5,
    Data_StoutAlamental = 6
};

// Identifier returning the guids (used in the ObjectAccessor)
enum InstanceDatas64
{
    Data_OokOokPartyCounter = 0,
    Data_BossOokOok = 1,
    Data_BossHoptallus = 2,
    Data_BossYanZhu = 3
};

// Actions that need to be accessed at any time, such at the OokOok launching phase
enum StaticActions
{
    Action_StartBossOokOokEvent = 1
};

// Each time a creature of one of these is killed, we increment the killed hozen counter (and the power of the players)
static const uint32 OokOokEntourage[MAX_OOKOOK_ENTOURAGE] = {0};
// We need to access it during OokOok encounter, so it is here ;

#endif // STORMSTOUT_BREWERY_H
