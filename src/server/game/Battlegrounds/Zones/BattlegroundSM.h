/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BATTLEGROUNDSM_H
#define __BATTLEGROUNDSM_H

#include "Language.h"
#include "Battleground.h"

enum BG_SM_WorldStates
{
	SM_UNK1							= 6436,
    SM_ALLIANCE_RESOURCES           = 6437,
    SM_HORDE_RESOURCES              = 6438,
	SM_UNK2							= 6439,
	SM_UNK3							= 6440,
	SM_UNK4							= 6441,
	SM_UNK5							= 6442,
	SM_UNK6							= 6443
};

enum BG_SM_ProgressBarConsts
{
    BG_SM_MINE_CART_RADIUS                  = 24,
    BG_SM_PROGRESS_BAR_DONT_SHOW			= 0,
    BG_SM_PROGRESS_BAR_SHOW					= 1,
    BG_SM_PROGRESS_BAR_PERCENT_GREY			= 40,
    BG_SM_PROGRESS_BAR_STATE_MIDDLE			= 50,
    BG_SM_PROGRESS_BAR_HORDE_CONTROLLED		= 0,
    BG_SM_PROGRESS_BAR_NEUTRAL_LOW			= 30,
    BG_SM_PROGRESS_BAR_NEUTRAL_HIGH			= 70,
    BG_SM_PROGRESS_BAR_ALLI_CONTROLLED		= 100
};

enum BG_SM_Sounds
{
    BG_SM_SOUND_MINE_CART_CAPTURED_HORDE		= 8213,
    BG_SM_SOUND_MINE_CART_CAPTURED_ALLIANCE		= 8173,
	BG_SM_SOUND_NEAR_VICTORY					= 8456,
};

enum BG_SM_Spells
{
    BG_SM_CONTROL_VISUAL_ALLIANCE	= 116086,
    BG_SM_CONTROL_VISUAL_HORDE		= 116085,
	BG_SM_CONTROL_VISUAL_NEUTRAL	= 118001
};

enum SMBattlegroundObjectEntry
{

};

enum SMBattlegroundGaveyards
{
	SM_GRAVEYARD_MAIN_ALLIANCE     = 4062,
    SM_GRAVEYARD_MAIN_HORDE        = 4061,
};

enum SMBattlegroundCreaturesTypes
{
    SM_SPIRIT_ALLIANCE		= 0,
    SM_SPIRIT_HORDE			= 1,
	SM_MINE_CART_TRIGGER	= 2,

    BG_SM_CREATURES_MAX		= 3
};

enum SMBattlegroundObjectTypes
{
	BG_SM_OBJECT_DOOR_A_1			= 0,
    BG_SM_OBJECT_DOOR_H_1			= 1,
	BG_SM_OBJECT_DOOR_A_2			= 2,
    BG_SM_OBJECT_DOOR_H_2			= 3,
	BG_SM_OBJECT_NEEDLE_INDICATOR_1	= 4,
	BG_SM_OBJECT_NEEDLE_INDICATOR_2	= 5,
	BG_SM_OBJECT_MAX				= 6
};

enum BG_SM_Score
{
    BG_SM_WARNING_NEAR_VICTORY_SCORE    = 1400,
    BG_SM_MAX_TEAM_SCORE                = 1600
};

enum SMBattlegroundMineCartState
{
    SM_MINE_CART_CONTROL_NEUTRAL	= 0,
    SM_MINE_CART_CONTROL_ALLIANCE	= 1,
    SM_MINE_CART_CONTROL_HORDE		= 2,
};

const float BG_SM_TriggerPositions[4][3] =
{
    {566.950562f, 337.057861f, 346.712219f},   // Waterfall
    {619.470337f, 79.719612f, 298.262085f},   // Lava
    {895.925598f, 27.365923f, 364.005798f},   // Diamond
    {779.437683f, 499.856049f, 359.337891f}   // Troll
};

enum BG_SM_CreatureIds
{
	NPC_MINE_CART_1			= 60378,
	NPC_MINE_CART_2			= 60379,
	NPC_MINE_CART_3			= 60380,
	NPC_MINE_CART_TRIGGER	= 400464,
};

#define MINE_CART_CHECK_TIMER		1000
#define MINE_CART_RESPAWN_TIME		10000
#define MINE_CART_SPAWN_INTERVAL	30000

#define POINTS_PER_KILL			0
#define POINTS_PER_MINE_CART	0
#define NEUTRAL					0

#define BG_SM_NotSMWeekendHonorTicks    260
#define BG_SM_SMWeekendHonorTicks       160
#define SM_MINE_CART_MAX				3

struct BattlegroundSMScore : public BattlegroundScore
{
    BattlegroundSMScore() : MineCartCaptures(0) { }
    ~BattlegroundSMScore() { }
    uint32 MineCartCaptures;
};

class BattlegroundSM : public Battleground
{
    public:
        BattlegroundSM();
        ~BattlegroundSM();

        /* inherited from BattlegroundClass */
        void AddPlayer(Player* player);
        void StartingEventCloseDoors();
        void StartingEventOpenDoors();

        void RemovePlayer(Player* player, uint64 guid, uint32 team);
        void HandleBuffUse(uint64 buff_guid);
        void HandleKillPlayer(Player* player, Player* killer);
        WorldSafeLocsEntry const* GetClosestGraveYard(Player* player);
        bool SetupBattleground();
        void Reset();
        void UpdateTeamScore(uint32 Team);
        void EndBattleground(uint32 winner);
        void UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor = true);
        void FillInitialWorldStates(WorldPacket& data);

        /* Battleground Events */
        void EventPlayerClickedOnNeedle(Player* Source, GameObject* target_obj);

        uint32 GetPrematureWinner();
    private:
        void PostUpdateImpl(uint32 diff);

        void EventTeamCapturedMineCart(uint32 team, uint32 mineCart[SM_MINE_CART_MAX]);
        void UpdatePointsCount(uint32 Team);
		void SummonMineCart(uint32 diff);

        /* Point status updating procedures */
        void CheckSomeoneLeftMineCartArea();
        void CheckSomeoneJoinedMineCartArea();
        void UpdateMineCartStatuses();

        /* Scorekeeping */
        void AddPoints(uint32 Team, uint32 Points);

        void RemovePoint(uint32 TeamID, uint32 Points = 1) { m_TeamScores[GetTeamIndexByTeamId(TeamID)] -= Points; }
        void SetTeamPoint(uint32 TeamID, uint32 Points = 0) { m_TeamScores[GetTeamIndexByTeamId(TeamID)] = Points; }
		void CheckPlayerNearMineCart(uint32 diff);
        uint32 m_HonorScoreTics[2];
        uint32 m_TeamPointsCount[2];

        uint32 m_MineCartsTrigger[SM_MINE_CART_MAX];
		uint8 m_LastMineCart; // 0 = Reset, 1 = First Mine Cart, 2 = Second Mine Cart, 3 = Third Mine Cart

        uint32 m_MineCartKeeper; // keepers team
		uint32 m_MineCartSpawnTimer;

        uint32 m_MineCartOwnedByTeam[SM_MINE_CART_MAX];
        uint8 m_MineCartState[SM_MINE_CART_MAX];
        int32 m_MineCartBarStatus[SM_MINE_CART_MAX];
		uint32 mineCartCheckTimer;
        typedef std::vector<uint64> PlayersNearMineCartType;
        PlayersNearMineCartType m_PlayersNearMineCart[SM_MINE_CART_MAX + 1];
        uint8 m_CurrentMineCartPlayersCount[2*SM_MINE_CART_MAX];

        int32 m_MineCartAddingTimer;
        uint32 m_HonorTics;
		bool m_IsInformedNearVictory;
};
#endif

