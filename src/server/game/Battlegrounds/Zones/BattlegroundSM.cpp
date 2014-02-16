/* Script de Sungis : Silvershard Mines */

#include "BattlegroundSM.h"
#include "ScriptPCH.h"
#include "ObjectMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "BattlegroundMgr.h"
#include "Creature.h"
#include "Language.h"
#include "Object.h"
#include "Player.h"
#include "Util.h"

// these variables aren't used outside of this file, so declare them only here
uint32 BG_SM_HonorScoreTicks[BG_HONOR_MODE_NUM] =
{
    260, // normal honor
    160  // holiday
};

BattlegroundSM::BattlegroundSM()
{
    m_BuffChange = true;
    BgObjects.resize(BG_SM_OBJECT_MAX);
    BgCreatures.resize(BG_SM_CREATURES_MAX);

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_TK_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_TK_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_TK_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_TK_HAS_BEGUN;
}

BattlegroundSM::~BattlegroundSM()
{
}

void BattlegroundSM::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // Check if player joined Mine Cart
        BattlegroundSM::CheckSomeoneJoinedMineCartArea();
        // Check if player left Mine Cart
        BattlegroundSM::CheckSomeoneLeftMineCartArea();
        BattlegroundSM::UpdateMineCartStatuses();

		//BattlegroundSM::SummonMineCart(diff);
		BattlegroundSM::CheckPlayerNearMineCart(diff);
    }
}

void BattlegroundSM::StartingEventCloseDoors()
{
    SpawnBGObject(BG_SM_OBJECT_DOOR_A_1, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_A_2, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_SM_OBJECT_DOOR_H_1, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_H_2, RESPAWN_IMMEDIATELY);

    //for (uint32 i = BG_SM_OBJECT_A_BANNER_FEL_REAVER_CENTER; i < BG_SM_OBJECT_MAX; ++i)
        //SpawnBGObject(i, RESPAWN_ONE_DAY);
}

void BattlegroundSM::StartingEventOpenDoors()
{
    SpawnBGObject(BG_SM_OBJECT_DOOR_A_1, RESPAWN_ONE_DAY);
    SpawnBGObject(BG_SM_OBJECT_DOOR_A_2, RESPAWN_ONE_DAY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_H_1, RESPAWN_ONE_DAY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_H_2, RESPAWN_ONE_DAY);

	Creature* trigger = NULL;
	if (trigger = AddCreature(NPC_MINE_CART_TRIGGER, SM_MINE_CART_TRIGGER, 0, 748.360779f, 195.203018f, 331.861938f, 2.428625f))
	{
		if (uint8 creatureId = urand(1, 3))
		{
			switch (creatureId)
			{
				case 1:
				{
					if (trigger)
					{
						if (m_LastMineCart != creatureId)
						{
							trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f);
							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
							}

							m_LastMineCart = creatureId;
						}
						else
						{
							if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f),
															trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
								if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								}
						}
					}
					break;
				}

				case 2:
				{
					if (trigger)
					{
						if (m_LastMineCart != creatureId)
						{
							trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f);
							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
							}

							m_LastMineCart = creatureId;
						}
						else
						{
							if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
															trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
								if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								}
						}
					}
					break;
				}

				case 3:
				{
					if (trigger)
					{
						if (m_LastMineCart != creatureId)
						{
							trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f);
							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
							}

							m_LastMineCart = creatureId;
						}
						else
						{
							if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
															trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f)))
								if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								}
						}
					}
					break;
				}

				default:
					break;
			}
		}
	}
    //for (uint32 i = BG_SM_OBJECT_N_BANNER_FEL_REAVER_CENTER; i <= BG_SM_OBJECT_FLAG_NETHERSTORM; ++i)
        //SpawnBGObject(i, RESPAWN_IMMEDIATELY);
    /*for (uint32 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        //randomly spawn buff
        uint8 buff = urand(0, 2);
        SpawnBGObject(BG_SM_OBJECT_SPEEDBUFF_FEL_REAVER + buff + i * 3, RESPAWN_IMMEDIATELY);
    }*/
}

void BattlegroundSM::SummonMineCart(uint32 diff)
{
	if (m_MineCartSpawnTimer <= 0)
	{
		Creature* trigger = NULL;
		if (trigger = AddCreature(NPC_MINE_CART_TRIGGER, SM_MINE_CART_TRIGGER, 0, 748.360779f, 195.203018f, 331.861938f, 2.428625f))
		{
			if (uint8 creatureNumber = urand(1, 3))
			{
				switch (creatureNumber)
				{
					case 1:
					{
						if (trigger)
						{
							if (m_LastMineCart != creatureNumber)
							{
								trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f);
								if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								}

								m_LastMineCart = creatureNumber;
							}
							else
							{
								if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f),
																trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
									if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
									{
										cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
										cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);

										if (chosenCart->GetEntry() == NPC_MINE_CART_2)
											m_LastMineCart = 2;
										else m_LastMineCart = 3;
									}
							}
						}
						break;
					}

					case 2:
					{
						if (trigger)
						{
							if (m_LastMineCart != creatureNumber)
							{
								trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f);
								if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								}

								m_LastMineCart = creatureNumber;
							}
							else
							{
								if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
																trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
									if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
									{
										cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
										cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);

										if (chosenCart->GetEntry() == NPC_MINE_CART_1)
											m_LastMineCart = 1;
										else m_LastMineCart = 3;
									}
							}
						}
						break;
					}

					case 3:
					{
						if (trigger)
						{
							if (m_LastMineCart != creatureNumber)
							{
								trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f);
								if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								}

								m_LastMineCart = creatureNumber;
							}
							else
							{
								if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
																trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f)))
									if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
									{
										cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
										cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);

										if (chosenCart->GetEntry() == NPC_MINE_CART_1)
											m_LastMineCart = 1;
										else m_LastMineCart = 2;
									}
							}
						}
						break;
					}

					default:
						break;
				}
			}
		}
		m_MineCartSpawnTimer = MINE_CART_SPAWN_INTERVAL;

	} else m_MineCartSpawnTimer -= diff;
}

void BattlegroundSM::CheckPlayerNearMineCart(uint32 diff)
{
	if (mineCartCheckTimer <= 0)
	{
		for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
			if (Player* player = ObjectAccessor::FindPlayer(itr->first))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					if (player->FindNearestCreature(NPC_MINE_CART_1, 24.0f))
						UpdateWorldStateForPlayer(SM_UNK1, 1, player);

					if (player->FindNearestCreature(NPC_MINE_CART_2, 24.0f))
						UpdateWorldStateForPlayer(SM_UNK2, 1, player);

					if (player->FindNearestCreature(NPC_MINE_CART_3, 24.0f))
						UpdateWorldStateForPlayer(SM_UNK3, 1, player);
				}
				else
				{
					if (player->FindNearestCreature(NPC_MINE_CART_1, 24.0f))
						UpdateWorldStateForPlayer(SM_UNK4, 1, player);

					if (player->FindNearestCreature(NPC_MINE_CART_2, 24.0f))
						UpdateWorldStateForPlayer(SM_UNK5, 1, player);

					if (player->FindNearestCreature(NPC_MINE_CART_3, 24.0f))
						UpdateWorldStateForPlayer(SM_UNK6, 1, player);
				}

					mineCartCheckTimer = MINE_CART_CHECK_TIMER;
			}
	} else mineCartCheckTimer -= diff;
}

void BattlegroundSM::AddPoints(uint32 Team, uint32 Points)
{
    TeamId team_index = GetTeamIndexByTeamId(Team);
    m_TeamScores[team_index] += Points;
    m_HonorScoreTics[team_index] += Points;
    if (m_HonorScoreTics[team_index] >= m_HonorTics)
    {
        RewardHonorToTeam(GetBonusHonorFromKill(1), Team);
        m_HonorScoreTics[team_index] -= m_HonorTics;
    }
    UpdateTeamScore(team_index);
}

void BattlegroundSM::CheckSomeoneJoinedMineCartArea()
{
    GameObject* obj = NULL;
    /*for (uint8 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        obj = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_TOWER_CAP_FEL_REAVER + i]);
        if (obj)
        {
            uint8 j = 0;
            while (j < m_PlayersNearMineCart[SM_MINE_CART_MAX].size())
            {
                Player* player = ObjectAccessor::FindPlayer(m_PlayersNearMineCart[SM_MINE_CART_MAX][j]);
                if (!player)
                {
                    sLog->outError(LOG_FILTER_BATTLEGROUND, "BattlegroundSM:CheckSomeoneJoinedPoint: Player (GUID: %u) not found!", GUID_LOPART(m_PlayersNearMineCart[SM_MINE_CART_MAX][j]));
                    ++j;
                    continue;
                }
                if (player->CanCaptureTowerPoint() && player->IsWithinDistInMap(obj, BG_SM_POINT_RADIUS))
                {
                    //player joined point!
                    //show progress bar
                    UpdateWorldStateForPlayer(PROGRESS_BAR_PERCENT_GREY, BG_SM_PROGRESS_BAR_PERCENT_GREY, player);
                    UpdateWorldStateForPlayer(PROGRESS_BAR_STATUS, m_PointBarStatus[i], player);
                    UpdateWorldStateForPlayer(PROGRESS_BAR_SHOW, BG_SM_PROGRESS_BAR_SHOW, player);
                    //add player to point
                    m_PlayersNearMineCart[i].push_back(m_PlayersNearMineCart[SM_MINE_CART_MAX][j]);
                    //remove player from "free space"
                    m_PlayersNearMineCart[SM_MINE_CART_MAX].erase(m_PlayersNearMineCart[SM_MINE_CART_MAX].begin() + j);
                }
                else
                    ++j;
            }
        }
    }*/
}

void BattlegroundSM::CheckSomeoneLeftMineCartArea()
{
    //reset current point counts
    /*for (uint8 i = 0; i < 2*SM_MINE_CART_MAX; ++i)
        m_CurrentPointPlayersCount[i] = 0;
    GameObject* obj = NULL;
    for (uint8 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        obj = HashMapHolder<Creature>::Find(BgObjects[BG_SM_OBJECT_TOWER_CAP_FEL_REAVER + i]);
        if (obj)
        {
            uint8 j = 0;
            while (j < m_PlayersNearMineCart[i].size())
            {
                Player* player = ObjectAccessor::FindPlayer(m_PlayersNearMineCart[i][j]);
                if (!player)
                {
                    sLog->outError(LOG_FILTER_BATTLEGROUND, "BattlegroundSM:CheckSomeoneLeftPoint Player (GUID: %u) not found!", GUID_LOPART(m_PlayersNearMineCart[i][j]));
                    //move not existed player to "free space" - this will cause many error showing in log, but it is a very important bug
                    m_PlayersNearMineCart[SM_MINE_CART_MAX].push_back(m_PlayersNearMineCart[i][j]);
                    m_PlayersNearMineCart[i].erase(m_PlayersNearMineCart[i].begin() + j);
                    continue;
                }
                if (!player->CanCaptureTowerPoint() || !player->IsWithinDistInMap(obj, BG_SM_POINT_RADIUS))
                    //move player out of point (add him to players that are out of points
                {
                    m_PlayersNearMineCart[SM_MINE_CART_MAX].push_back(m_PlayersNearMineCart[i][j]);
                    m_PlayersNearMineCart[i].erase(m_PlayersNearMineCart[i].begin() + j);
                    this->UpdateWorldStateForPlayer(PROGRESS_BAR_SHOW, BG_SM_PROGRESS_BAR_DONT_SHOW, player);
                }
                else
                {
                    //player is neat flag, so update count:
                    m_CurrentPointPlayersCount[2 * i + GetTeamIndexByTeamId(player->GetTeam())]++;
                    ++j;
                }
            }
        }
    }*/
}

void BattlegroundSM::UpdateMineCartStatuses()
{
    /*for (uint8 mineCart = 0; mineCart < SM_MINE_CART_MAX; ++mineCart)
    {
        if (m_PlayersNearMineCart[mineCart].empty())
            continue;
        //count new point bar status:
        m_PointBarStatus[mineCart] += (m_CurrentPointPlayersCount[2 * mineCart] - m_CurrentPointPlayersCount[2 * mineCart + 1] < BG_SM_POINT_MAX_CAPTURERS_COUNT) ? m_CurrentPointPlayersCount[2 * mineCart] - m_CurrentPointPlayersCount[2 * mineCart + 1] : BG_SM_POINT_MAX_CAPTURERS_COUNT;

        if (m_PointBarStatus[mineCart] > BG_SM_PROGRESS_BAR_ALI_CONTROLLED)
            //point is fully alliance's
            m_PointBarStatus[mineCart] = BG_SM_PROGRESS_BAR_ALI_CONTROLLED;
        if (m_PointBarStatus[mineCart] < BG_SM_PROGRESS_BAR_HORDE_CONTROLLED)
            //point is fully horde's
            m_PointBarStatus[mineCart] = BG_SM_PROGRESS_BAR_HORDE_CONTROLLED;

        uint32 pointOwnerTeamId = 0;
        //find which team should own this point
        if (m_PointBarStatus[mineCart] <= BG_SM_PROGRESS_BAR_NEUTRAL_LOW)
            pointOwnerTeamId = HORDE;
        else if (m_PointBarStatus[mineCart] >= BG_SM_PROGRESS_BAR_NEUTRAL_HIGH)
            pointOwnerTeamId = ALLIANCE;
        else
            pointOwnerTeamId = SM_POINT_NO_OWNER;

        for (uint8 i = 0; i < m_PlayersNearMineCart[mineCart].size(); ++i)
        {
            Player* player = ObjectAccessor::FindPlayer(m_PlayersNearMineCart[mineCart][i]);
            if (player)
            {
                this->UpdateWorldStateForPlayer(PROGRESS_BAR_STATUS, m_PointBarStatus[mineCart], player);
                //if point owner changed we must evoke event!
                if (pointOwnerTeamId != m_PointOwnedByTeam[mineCart])
                {
                    //point was uncontrolled and player is from team which captured point
                    if (m_PointState[mineCart] == SM_POINT_STATE_UNCONTROLLED && player->GetTeam() == pointOwnerTeamId)
                        this->EventTeamCapturedPoint(player, mineCart);

                    //point was under control and player isn't from team which controlled it
                    if (m_PointState[mineCart] == SM_POINT_UNDER_CONTROL && player->GetTeam() != m_PointOwnedByTeam[point])
                        this->EventTeamLostPoint(player, mineCart);
                }

                /// @workaround The original AreaTrigger is covered by a bigger one and not triggered on client side.
                if (mineCart == FEL_REAVER && m_PointOwnedByTeam[mineCart] == player->GetTeam())
                    if (m_FlagState && GetFlagPickerGUID() == player->GetGUID())
                        if (player->GetDistance(2044.0f, 1729.729f, 1190.03f) < 3.0f)
                            EventPlayerCapturedFlag(player, BG_SM_OBJECT_FLAG_FEL_REAVER);
            }
        }
    }*/
}

void BattlegroundSM::UpdateTeamScore(uint32 Team)
{
    uint32 score = GetTeamScore(Team);

    if (!m_IsInformedNearVictory && score >= BG_SM_WARNING_NEAR_VICTORY_SCORE)
    {
        if (Team == ALLIANCE)
            SendMessageToAll(LANG_BG_TK_A_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        else
            SendMessageToAll(LANG_BG_TK_H_NEAR_VICTORY, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        PlaySoundToAll(BG_SM_SOUND_NEAR_VICTORY);
        m_IsInformedNearVictory = true;
    }

    if (score >= BG_SM_MAX_TEAM_SCORE)
    {
        score = BG_SM_MAX_TEAM_SCORE;
        if (Team == TEAM_ALLIANCE)
            EndBattleground(ALLIANCE);
        else
            EndBattleground(HORDE);
    }

    if (Team == TEAM_ALLIANCE)
        UpdateWorldState(SM_ALLIANCE_RESOURCES, score);
    else
        UpdateWorldState(SM_HORDE_RESOURCES, score);
}

void BattlegroundSM::EndBattleground(uint32 winner)
{
    // Win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);
    // Complete map reward
    RewardHonorToTeam(GetBonusHonorFromKill(1), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(1), HORDE);

    Battleground::EndBattleground(winner);
}

void BattlegroundSM::UpdatePointsCount(uint32 Team)
{
    if (Team == ALLIANCE)
        UpdateWorldState(SM_ALLIANCE_RESOURCES, m_TeamPointsCount[TEAM_ALLIANCE]);
    else
        UpdateWorldState(SM_HORDE_RESOURCES, m_TeamPointsCount[TEAM_HORDE]);
}

void BattlegroundSM::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    //create score and add it to map
    BattlegroundSMScore* sc = new BattlegroundSMScore;

    m_PlayersNearMineCart[SM_MINE_CART_MAX].push_back(player->GetGUID());

    PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundSM::RemovePlayer(Player* player, uint64 guid, uint32 /*team*/)
{
    // sometimes flag aura not removed :(
    for (int j = SM_MINE_CART_MAX; j >= 0; --j)
    {
        for (size_t i = 0; i < m_PlayersNearMineCart[j].size(); ++i)
            if (m_PlayersNearMineCart[j][i] == guid)
                m_PlayersNearMineCart[j].erase(m_PlayersNearMineCart[j].begin() + i);
    }
}

bool BattlegroundSM::SetupBattleground()
{
    /*// doors
    if (!AddObject(BG_SM_OBJECT_DOOR_A, BG_OBJECT_A_DOOR_SM_ENTRY, 2527.6f, 1596.91f, 1262.13f, -3.12414f, -0.173642f, -0.001515f, 0.98477f, -0.008594f, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_SM_OBJECT_DOOR_H, BG_OBJECT_H_DOOR_SM_ENTRY, 1803.21f, 1539.49f, 1261.09f, 3.14159f, 0.173648f, 0, 0.984808f, 0, RESPAWN_IMMEDIATELY)
		)
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundEY: Failed to spawn some object Battleground not created!");
        return false;
    }

    //buffs
    for (int i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        AreaTriggerEntry const* at = sAreaTriggerStore.LookupEntry(m_Points_Trigger[i]);
        if (!at)
        {
            sLog->outError(LOG_FILTER_BATTLEGROUND, "BattlegroundSM: Unknown trigger: %u", m_Points_Trigger[i]);
            continue;
        }
        if (!AddObject(BG_SM_OBJECT_SPEEDBUFF_FEL_REAVER + i * 3, Buff_Entries[0], at->x, at->y, at->z, 0.907571f, 0, 0, 0.438371f, 0.898794f, RESPAWN_ONE_DAY)
            || !AddObject(BG_SM_OBJECT_SPEEDBUFF_FEL_REAVER + i * 3 + 1, Buff_Entries[1], at->x, at->y, at->z, 0.907571f, 0, 0, 0.438371f, 0.898794f, RESPAWN_ONE_DAY)
            || !AddObject(BG_SM_OBJECT_SPEEDBUFF_FEL_REAVER + i * 3 + 2, Buff_Entries[2], at->x, at->y, at->z, 0.907571f, 0, 0, 0.438371f, 0.898794f, RESPAWN_ONE_DAY)
)
            sLog->outError(LOG_FILTER_BATTLEGROUND, "BattlegroundSM: Cannot spawn buff");
    }

    WorldSafeLocsEntry const* sg = NULL;
    sg = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_ALLIANCE);
    if (!sg || !AddSpiritGuide(SM_SPIRIT_MAIN_ALLIANCE, sg->x, sg->y, sg->z, 2.172720f, ALLIANCE))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundEY: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }

    sg = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_HORDE);
    if (!sg || !AddSpiritGuide(SM_SPIRIT_MAIN_HORDE, sg->x, sg->y, sg->z, 5.575674f, HORDE))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundEY: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }*/
    return true;
}

void BattlegroundSM::Reset()
{
    //call parent's class reset
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE] = 0;
    m_TeamScores[TEAM_HORDE] = 0;
    m_HonorScoreTics[TEAM_ALLIANCE] = 0;
    m_HonorScoreTics[TEAM_HORDE] = 0;
	mineCartCheckTimer = MINE_CART_CHECK_TIMER;
    bool isBGWeekend = sBattlegroundMgr->IsBGWeekend(GetTypeID());
    m_HonorTics = (isBGWeekend) ? BG_SM_SMWeekendHonorTicks : BG_SM_NotSMWeekendHonorTicks;
	m_IsInformedNearVictory = false;
	m_MineCartSpawnTimer = 90*IN_MILLISECONDS; // Firt value
	m_LastMineCart = 0;

    for (uint8 i = 0; i < SM_MINE_CART_MAX; ++i)
    {
        m_MineCartOwnedByTeam[i] = NEUTRAL;
        m_MineCartState[i] = NEUTRAL;
        m_MineCartBarStatus[i] = BG_SM_PROGRESS_BAR_STATE_MIDDLE;
        m_PlayersNearMineCart[i].clear();
        m_PlayersNearMineCart[i].reserve(15);                  //tip size
    }
}

void BattlegroundSM::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    Battleground::HandleKillPlayer(player, killer);
    EventPlayerDroppedFlag(player);
}

void BattlegroundSM::EventPlayerClickedOnNeedle(Player* Source, GameObject* target_obj)
{
    if (GetStatus() != STATUS_IN_PROGRESS || !Source->IsWithinDistInMap(target_obj, 10))
        return;
}

void BattlegroundSM::EventTeamCapturedMineCart(uint32 team, uint32 mineCart[SM_MINE_CART_MAX])
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    /*m_FlagState = BG_SM_FLAG_STATE_WAIT_RESPAWN;
    Source->RemoveAurasDueToSpell(BG_SM_NETHERSTORM_FLAG_SPELL);

    Source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);

    if (Source->GetTeam() == ALLIANCE)
        PlaySoundToAll(BG_SM_SOUND_FLAG_CAPTURED_ALLIANCE);
    else
        PlaySoundToAll(BG_SM_SOUND_FLAG_CAPTURED_HORDE);

    SpawnBGObject(BgObjectType, RESPAWN_IMMEDIATELY);

    m_FlagsTimer = BG_SM_FLAG_RESPAWN_TIME;
    m_FlagCapturedBgObjectType = BgObjectType;

    uint8 team_id = 0;
    if (Source->GetTeam() == ALLIANCE)
    {
        team_id = TEAM_ALLIANCE;
        SendMessageToAll(LANG_BG_SM_CAPTURED_FLAG_A, CHAT_MSG_BG_SYSTEM_ALLIANCE, Source);
    }
    else
    {
        team_id = TEAM_HORDE;
        SendMessageToAll(LANG_BG_SM_CAPTURED_FLAG_H, CHAT_MSG_BG_SYSTEM_HORDE, Source);
    }

    if (m_TeamPointsCount[team_id] > 0)
        AddPoints(Source->GetTeam(), BG_SM_FlagPoints[m_TeamPointsCount[team_id] - 1]);

    UpdatePlayerScore(Source, SCORE_FLAG_CAPTURES, 1);*/
}

void BattlegroundSM::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(Source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found
        return;

    switch (type)
    {
        case SCORE_CART_CONTROLLED:                           // flags captured
            ((BattlegroundSMScore*)itr->second)->MineCartCaptures += value;
            break;
        default:
            Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
            break;
    }
}

void BattlegroundSM::FillInitialWorldStates(WorldPacket& data)
{
    data << uint32(SM_ALLIANCE_RESOURCES) << uint32(m_TeamPointsCount[TEAM_ALLIANCE]);
    data << uint32(SM_HORDE_RESOURCES) << uint32(m_TeamPointsCount[TEAM_HORDE]);
}

uint32 BattlegroundSM::GetPrematureWinner()
{
    if (GetTeamScore(TEAM_ALLIANCE) > GetTeamScore(TEAM_HORDE))
        return ALLIANCE;
    else if (GetTeamScore(TEAM_HORDE) > GetTeamScore(TEAM_ALLIANCE))
        return HORDE;

    return Battleground::GetPrematureWinner();
}