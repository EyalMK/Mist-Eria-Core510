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
	SM_DISPLAY_ALLIANCE_RESSOURCES	= 6442,
	SM_DISPLAY_HORDE_RESSOURCES		= 6443,
	SM_DISPLAY_PROGRESS_BAR			= 6875, // 0 = false, 1 = true
	SM_PROGRESS_BAR_STATUS			= 6876, // 0 = Horde max, 50 = Neutral, 100 = Alliance max
	SM_UNK5							= 6877
};

enum BG_SM_MineCarts
{
	BG_SM_MINE_CART_1		= 1,
	BG_SM_MINE_CART_2		= 2,
	BG_SM_MINE_CART_3		= 3
};

enum BG_SM_ProgressBarConsts
{
    BG_SM_PROGRESS_BAR_DONT_SHOW			= 0,
    BG_SM_PROGRESS_BAR_SHOW					= 1,
    BG_SM_PROGRESS_BAR_NEUTRAL				= 50,
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
	BG_SM_MINE_DEPOT			= 400433,
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
	BG_SM_OBJECT_MINE_DEPOT_1		= 6,
	BG_SM_OBJECT_MINE_DEPOT_2		= 7,
	BG_SM_OBJECT_MINE_DEPOT_3		= 8,
	BG_SM_OBJECT_MINE_DEPOT_4		= 9,
	BG_SM_OBJECT_MAX				= 10
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

enum BG_SM_CreatureIds
{
	NPC_MINE_CART_1			= 60378,
	NPC_MINE_CART_2			= 60379,
	NPC_MINE_CART_3			= 60380,
	NPC_MINE_CART_TRIGGER	= 400464,
};

const float BG_SM_DepotPos[4][4] =
{
    {566.950989f, 337.05801f, 347.295013f, 1.559089f},   // Waterfall
    {619.469971f, 79.719597f, 299.067993f, 1.625564f},   // Lava
    {895.974426f, 27.210802f, 364.390991f, 3.445790f},   // Diamond
    {778.444946f, 500.949707f, 359.738983f, 0.737040f}   // Troll
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

        /* Scorekeeping */
        void AddPoints(uint32 Team, uint32 Points);

        void RemovePoint(uint32 TeamID, uint32 Points = 1) { m_TeamScores[GetTeamIndexByTeamId(TeamID)] -= Points; }
        void SetTeamPoint(uint32 TeamID, uint32 Points = 0) { m_TeamScores[GetTeamIndexByTeamId(TeamID)] = Points; }
		void CheckPlayerNearMineCart(uint32 diff);
		uint32 GetMineCartTeamKeeper(uint8 mineCart);
        uint32 m_HonorScoreTics[2];
        uint32 m_TeamPointsCount[2];

        uint32 m_MineCartsTrigger[SM_MINE_CART_MAX];
		int32 m_MineCartsProgressBar[SM_MINE_CART_MAX];
		uint8 m_LastMineCart; // 0 = Reset, 1 = First Mine Cart, 2 = Second Mine Cart, 3 = Third Mine Cart
		uint32 m_MineCartTeamKeeper[SM_MINE_CART_MAX]; // keepers team
		uint32 m_MineCartSpawnTimer;
		int32 m_mineCartCheckTimer;

        uint32 m_HonorTics;
		bool m_IsInformedNearVictory;
};
#endif

