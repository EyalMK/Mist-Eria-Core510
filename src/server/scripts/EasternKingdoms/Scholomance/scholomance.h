#include "ScriptPCH.h"

/** Global header for the instance "Scholomance **/

/// Const value representing how many encounters there are in the instance
#define MAX_ENCOUNTER 5

/// Enumerates entries of the bosses
enum Bosses {
	BOSS_INSTRUCTOR_CHILLHEART 	= 58633,
	BOSS_JANDICE_BAROV			= 59184,
	BOSS_RATTLEGORE				= 59153,
	BOSS_LILIAN_VOSS			= 58722,
	BOSS_DARKMASTER_GANDLING	= 59080
};

/// Miscellanous creatures of the instance, not tied to a specific script
enum Creatures {

};

/// Miscellanous GameObjects such as doors
enum GameObjects {

};

/// Used in InstanceScript to get status of the bosses
enum Datas {
	// Bosses states
	DATA_INSTRUCTOR_CHILLHEART  = 1,
	DATA_JANDICE_BAROV			= 2,
	DATA_RATTLEGORE				= 3,
	DATA_LILIAN_VOSS			= 4,
	DATA_DARKMASTER_GANDLING	= 5
};

/// Used in InstanceScript to get datas stored on 64 bits, such as guids
enum Datas64 {
	DATA64_INSTRUCTOR_CHILLHEART  = 1,
	DATA64_JANDICE_BAROV			= 2,
	DATA64_RATTLEGORE				= 3,
	DATA64_LILIAN_VOSS			= 4,
	DATA64_DARKMASTER_GANDLING	= 5
};