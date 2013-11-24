/* Script de Tydrheal */
/* Battleground: Temple de Kotmogu */

#ifndef __BATTLEGROUNDTK_H
#define __BATTLEGROUNDTK_H

#include "Battleground.h"


enum BG_TK_Sound
{
    BG_TK_SOUND_HORDE_ORB_PICKED_UP		= 8212,
    BG_TK_SOUND_ALLIANCE_ORB_PICKED_UP	= 8174,
	BG_TK_SOUND_ORB_RETURNED			= 8192,
	BG_TK_SOUND_NEAR_VICTORY			= 8456
};

enum BG_TK_OrbState
{
    BG_TK_ORB_STATE_ON_BASE			= 0,
    BG_TK_ORB_STATE_ON_PLAYER		= 1
};

enum BG_TK_Score
{
	BG_TK_KILL_TEAM_SCORE				= 10, // 10 points pour la team par kill
	BG_TK_KILL_ORB_TEAM_SCORE			= 20, // 20 points pour la team par kill de possesseur d'orb
	BG_TK_EXT_TEAM_SCORE				= 3, // 3 VP par seconde avec l'orb en possession à l'extérieur du temple
	BG_TK_IN_TEAM_SCORE					= 4, // 4 VP par seconde avec l'orb en possession dans le temple
	BG_TK_MIDDLE_TEAM_SCORE				= 5, // 5 VP par seconde avec l'orb en possession au milieu du BG
    BG_TK_WARNING_NEAR_VICTORY_SCORE    = 1400,
    BG_TK_MAX_TEAM_SCORE                = 1600
};

enum BG_TK_WorldStates
{
	BG_TK_RESOURCES_ALLIANCE		= 1776,
    BG_TK_RESOURCES_HORDE			= 1777,
    BG_TK_RESOURCES_MAX				= 1780,
    BG_TK_RESOURCES_WARNING			= 1955,
    BG_TK_ORB_UNK_ALLIANCE			= 1545,
    BG_TK_ORB_UNK_HORDE				= 1546,
//    ORB_UNK						= 1547,
    BG_TK_ORB_CAPTURES_ALLIANCE		= 1581,
    BG_TK_ORB_CAPTURES_HORDE		= 1582,
    BG_TK_ORB_CAPTURES_MAX			= 1601,
    BG_TK_ORB_STATE_HORDE			= 2338,
    BG_TK_ORB_STATE_ALLIANCE		= 2339,
    BG_TK_STATE_TIMER				= 4248,
    BG_TK_STATE_TIMER_ACTIVE		= 4247
};

enum BG_TK_ObjectTypes
{
	BG_TK_OBJECT_ORB_BLUE		= 0,
    BG_TK_OBJECT_ORB_PURPLE		= 1,
    BG_TK_OBJECT_ORB_GREEN		= 2,
    BG_TK_OBJECT_ORB_ORANGE		= 3,
    BG_TK_OBJECT_DOOR_A			= 4,
    BG_TK_OBJECT_DOOR_H			= 5,
    BG_TK_OBJECT_MAX			= 6
};

enum BG_TK_CreatureTypes
{
    TK_SPIRIT_MAIN_ALLIANCE   = 0,
    TK_SPIRIT_MAIN_HORDE      = 1,

    BG_CREATURES_MAX_TK      = 2
};

enum BG_TK_Graveyards
{
    TK_GRAVEYARD_MAIN_ALLIANCE     = 3552,
    TK_GRAVEYARD_MAIN_HORDE        = 3553
};

enum BG_TK_SpellId
{
	BG_TK_AURA_ORB_BLUE = 121164,
	BG_TK_AURA_ORB_PURPLE = 121175,
	BG_TK_AURA_ORB_GREEN = 121176,
	BG_TK_AURA_ORB_ORANGE = 121177
};

enum BG_TK_GameObjectId
{
    BG_OBJECT_DOOR_TK_A = 400424,
	BG_OBJECT_DOOR_TK_H = 400425,
	BG_OBJECT_TK_ORB_BLUE = 400420,
	BG_OBJECT_TK_ORB_PURPLE = 400421,
	BG_OBJECT_TK_ORB_GREEN = 400422,
	BG_OBJECT_TK_ORB_ORANGE = 400423
};

const float BG_TK_ZoneLimit[8][4] =
{
    {0.0f, 0.0f, 0.0f, 0.0f},                   // 1 coin du rectangle limite zone 1 (extérieur) et 2 (pseudo-intérieur)
    {0.0f, 0.0f, 0.0f, 0.0f},                    // 2 coin du rectangle limite zone 1 (extérieur) et 2 (pseudo-intérieur)
    {0.0f, 0.0f, 0.0f, 0.0f},                     // 3 coin du rectangle limite zone 1 (extérieur) et 2 (pseudo-intérieur)
    {0.0f, 0.0f, 0.0f, 0.0f},                     // 4 coin du rectangle limite zone 1 (extérieur) et 2 (pseudo-intérieur)
    {0.0f, 0.0f, 0.0f, 0.0f},                    // 1 coin du rectangle limite zone 2 (pseudo-intérieur) et 3 (intérieur)
    {0.0f, 0.0f, 0.0f, 0.0f},                   // 2 coin du rectangle limite zone 2 (pseudo-intérieur) et 3 (intérieur)
    {0.0f, 0.0f, 0.0f, 0.0f},                     // 3 coin du rectangle limite zone 2 (pseudo-intérieur) et 3 (intérieur)
	{0.0f, 0.0f, 0.0f, 0.0f}                    // 4 coin du rectangle limite zone 2 (pseudo-intérieur) et 3 (intérieur)
};

class BattlegroundTKScore : public BattlegroundScore
{
    public:
        BattlegroundTKScore(): OrbPossesions(0), PointsScored(0) {};
        virtual ~BattlegroundTKScore() {};

        uint32 OrbPossesions;
        uint32 PointsScored;
};

class BattlegroundTK : public Battleground
{
    public:
		/*Construction*/
        BattlegroundTK();
        ~BattlegroundTK();

		/* inherited from BattlegroundClass */
        void AddPlayer(Player* player);
        void StartingEventCloseDoors();
        void StartingEventOpenDoors();

		 /* Battleground Events */
        void EventPlayerDroppedOrb(Player* Source, uint32 orb);
        void EventPlayerClickedOnOrb(Player* Source, GameObject* target_obj);
        void EventPlayerCapturedOrb(Player* Source);

        void RemovePlayer(Player* player, uint64 guid, uint32 team);
        void HandleKillPlayer(Player* player, Player* killer);
        bool SetupBattleground();
        void Reset();
		void Udapte(uint32 diff);
        void EndBattleground(uint32 winner);
        void GetZonePlayerWithOrb(Player* player);

		/* BG Orbs */
        uint64 GetOrbPickerGUID(int32 team) const
        {
            if (team == TEAM_ALLIANCE || team == TEAM_HORDE)
                return m_orbOwners[team];
            return 0;
        }

        void SetOrbPicker(uint64 guid, uint32 orb)     { m_orbOwners[orb] = guid; }
		void SetOrbTeam(uint64 team, uint32 orb)     { m_orbTeam[orb] = team; }
		void SetOrbZone(uint8 zone, uint32 orb)      {_orbZone[orb] = zone; }
        bool IsAllianceOrbPickedup() const         { return m_orbOwners[TEAM_ALLIANCE] != 0; }
        bool IsHordeOrbPickedup() const            { return m_orbOwners[TEAM_HORDE] != 0; }
		uint64 GetOrbOwners(uint8 orb)             { return m_orbOwners[orb]; }
        uint8 GetOrbState(uint32 orb)             { return _orbState[orb]; }
		uint8 GetOrbZone(uint8 orb)              { return _orbZone[orb]; }
		void RespawnOrb(uint32 orb);
		void RespawnOrbAfterDrop(uint32 orb);
		bool HasAnOrb(Player* player);

		uint32 GetPrematureWinner();
        void UpdateScore(uint16 team, int16 points);
        void UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor = true);
		void FillInitialWorldStates(WorldPacket& data);
		/* Scorekeeping */
        void AddPoint(uint32 TeamID, uint32 Points = 1)     { m_TeamScores[GetTeamIndexByTeamId(TeamID)] += Points; }
        void SetTeamPoint(uint32 TeamID, uint32 Points = 0) { m_TeamScores[GetTeamIndexByTeamId(TeamID)] = Points; }
        void RemovePoint(uint32 TeamID, uint32 Points = 1)  { m_TeamScores[GetTeamIndexByTeamId(TeamID)] -= Points; }

		 private:
        uint64 m_orbOwners[4];                            // 0 = orb 1, 1 = orb 2, 2 = orb 3, 3 = orb 4 (for the guid player)
		uint64 m_orbTeam[4];                               // for the team who has this orb
 
        uint8 _orbState[4];                               // for checking orb state (on player, on base)
		uint8 _orbZone[4]; // 0 = orb 1, 1 = orb 2, 2 = orb 3, 3 = orb 4 (for the zone: 0: base, 1: zone 1, 2: zone 2, 3: zone 3)

		int32 m_Team_Scores[2];
		bool m_IsInformedNearVictory[2];

        uint32 m_ReputationCapture;
        uint32 m_HonorWinKills;
        uint32 m_HonorEndKills;
		uint32 _minutesElapsed;
        uint32 secondTimer0;
		uint32 secondTimer1;
		uint32 secondTimer2;
		uint32 secondTimer3;
		uint32 auraTimer0;
		uint32 auraTimer1;
		uint32 auraTimer2;
		uint32 auraTimer3;
		uint32 auraCounter0;
		uint32 auraCounter1;
		uint32 auraCounter2;
		uint32 auraCounter3;

        void PostUpdateImpl(uint32 diff);
};

#endif
