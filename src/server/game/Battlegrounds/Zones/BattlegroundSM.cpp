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

void BattlegroundSM::Reset()
{
    //call parent's class reset
    Battleground::Reset();

    m_TeamScores[TEAM_ALLIANCE] = 0;
    m_TeamScores[TEAM_HORDE] = 0;
    m_HonorScoreTics[TEAM_ALLIANCE] = 0;
	m_TeamPointsCount[TEAM_ALLIANCE] = 0;
    m_TeamPointsCount[TEAM_HORDE] = 0;
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

    for (uint8 i = BG_SM_OBJECT_MINE_DEPOT_1; i < BG_SM_OBJECT_MINE_DEPOT_4 + 1; ++i)
        SpawnBGObject(i, RESPAWN_ONE_DAY);
}

void BattlegroundSM::StartingEventOpenDoors()
{
    SpawnBGObject(BG_SM_OBJECT_DOOR_A_1, RESPAWN_ONE_DAY);
    SpawnBGObject(BG_SM_OBJECT_DOOR_A_2, RESPAWN_ONE_DAY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_H_1, RESPAWN_ONE_DAY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_H_2, RESPAWN_ONE_DAY);
	
	for (uint8 i = BG_SM_OBJECT_MINE_DEPOT_1; i < BG_SM_OBJECT_MINE_DEPOT_4 + 1; ++i)
		SpawnBGObject(i, RESPAWN_IMMEDIATELY);

	Creature* trigger = NULL;
	if (trigger = AddCreature(NPC_MINE_CART_TRIGGER, SM_MINE_CART_TRIGGER, 0, 748.360779f, 195.203018f, 331.861938f, 2.428625f))
	{
		if (uint8 mineCart = urand(BG_SM_MINE_CART_1, BG_SM_MINE_CART_3))
		{
			switch (mineCart)
			{
				case BG_SM_MINE_CART_1:
				{
					if (trigger)
					{
						if (m_LastMineCart != mineCart)
						{
							trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f);
							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
							}

							m_LastMineCart = mineCart;
						}
						else
						{
							if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f),
															trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
								if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
									cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									cart->SetSpeed(MOVE_WALK, 0.4f);

									if (chosenCart->GetEntry() == NPC_MINE_CART_2)
										m_LastMineCart = BG_SM_MINE_CART_2;
									else m_LastMineCart = BG_SM_MINE_CART_3;
								}
						}
					}
					break;
				}

				case BG_SM_MINE_CART_2:
				{
					if (trigger)
					{
						if (m_LastMineCart != mineCart)
						{
							trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f);
							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
							}

							m_LastMineCart = mineCart;
						}
						else
						{
							if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
															trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
								if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
									cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									cart->SetSpeed(MOVE_WALK, 0.4f);

									if (chosenCart->GetEntry() == NPC_MINE_CART_1)
										m_LastMineCart = BG_SM_MINE_CART_1;
									else m_LastMineCart = BG_SM_MINE_CART_3;
								}
						}
					}
					break;
				}

				case BG_SM_MINE_CART_3:
				{
					if (trigger)
					{
						if (m_LastMineCart != mineCart)
						{
							trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f);
							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
							}

							m_LastMineCart = mineCart;
						}
						else
						{
							if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
															trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f)))
								if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
									cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									cart->SetSpeed(MOVE_WALK, 0.4f);

									if (chosenCart->GetEntry() == NPC_MINE_CART_1)
										m_LastMineCart = BG_SM_MINE_CART_1;
									else m_LastMineCart = BG_SM_MINE_CART_2;
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
}

void BattlegroundSM::SummonMineCart(uint32 diff)
{
	if (m_MineCartSpawnTimer <= 0)
	{
		Creature* trigger = NULL;
		if (trigger = AddCreature(NPC_MINE_CART_TRIGGER, SM_MINE_CART_TRIGGER, 0, 748.360779f, 195.203018f, 331.861938f, 2.428625f))
		{
			if (uint8 mineCart = urand(BG_SM_MINE_CART_1, BG_SM_MINE_CART_3))
			{
				switch (mineCart)
				{
					case BG_SM_MINE_CART_1:
					{
						if (trigger)
						{
							if (m_LastMineCart != mineCart)
							{
								trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f);
								if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
									cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									cart->SetSpeed(MOVE_WALK, 0.4f);
								}

								m_LastMineCart = mineCart;
							}
							else
							{
								if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f),
																trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
									if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
									{
										cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
										cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
										cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
										cart->SetSpeed(MOVE_WALK, 0.4f);

										if (chosenCart->GetEntry() == NPC_MINE_CART_2)
											m_LastMineCart = BG_SM_MINE_CART_2;
										else m_LastMineCart = BG_SM_MINE_CART_3;
									}
							}
						}
						break;
					}

					case BG_SM_MINE_CART_2:
					{
						if (trigger)
						{
							if (m_LastMineCart != mineCart)
							{
								trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f);
								if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
									cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									cart->SetSpeed(MOVE_WALK, 0.4f);
								}

								m_LastMineCart = mineCart;
							}
							else
							{
								if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
																trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f)))
									if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
									{
										cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
										cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
										cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
										cart->SetSpeed(MOVE_WALK, 0.4f);

										if (chosenCart->GetEntry() == NPC_MINE_CART_1)
											m_LastMineCart = BG_SM_MINE_CART_1;
										else m_LastMineCart = BG_SM_MINE_CART_3;
									}
							}
						}
						break;
					}

					case BG_SM_MINE_CART_3:
					{
						if (trigger)
						{
							if (m_LastMineCart != mineCart)
							{
								trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f);
								if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
								{
									cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
									cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
									cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									cart->SetSpeed(MOVE_WALK, 0.4f);
								}

								m_LastMineCart = mineCart;
							}
							else
							{
								if (Creature* chosenCart = RAND(trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f),
																trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f)))
									if (Creature* cart = trigger->FindNearestCreature(chosenCart->GetEntry(), 99999.0f))
									{
										cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
										cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
										cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
										cart->SetSpeed(MOVE_WALK, 0.4f);

										if (chosenCart->GetEntry() == NPC_MINE_CART_1)
											m_LastMineCart = BG_SM_MINE_CART_1;
										else m_LastMineCart = BG_SM_MINE_CART_2;
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

void BattlegroundSM::CheckPlayerNearMineCart(uint32 diff) // Testing all worldstates here
{
	if (mineCartCheckTimer <= 0)
	{
		for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
			if (Player* player = ObjectAccessor::FindPlayer(itr->first))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					if (player->FindNearestCreature(NPC_MINE_CART_1, 24.0f, true))
					{
						UpdateWorldStateForPlayer(SM_UNK1, 1, player);
						UpdateWorldStateForPlayer(SM_ALLIANCE_RESOURCES, 1, player);
					}

					if (player->FindNearestCreature(NPC_MINE_CART_2, 24.0f, true))
						UpdateWorldStateForPlayer(SM_UNK2, 1, player);

					if (player->FindNearestCreature(NPC_MINE_CART_3, 24.0f, true))
						UpdateWorldStateForPlayer(SM_UNK3, 1, player);
				}
				else
				{
					if (player->FindNearestCreature(NPC_MINE_CART_1, 24.0f, true))
					{
						UpdateWorldStateForPlayer(SM_UNK4, 1, player);
						UpdateWorldStateForPlayer(SM_HORDE_RESOURCES, 1, player);
					}

					if (player->FindNearestCreature(NPC_MINE_CART_2, 24.0f, true))
						UpdateWorldStateForPlayer(SM_UNK5, 1, player);

					if (player->FindNearestCreature(NPC_MINE_CART_3, 24.0f, true))
						UpdateWorldStateForPlayer(SM_UNK6, 1, player);

					if (player->FindNearestCreature(64048, 24.0f, true))
						UpdateWorldStateForPlayer(SM_UNK7, 1, player);

					if (player->FindNearestCreature(64049, 24.0f, true))
						UpdateWorldStateForPlayer(SM_UNK8, 1, player);

					if (player->FindNearestCreature(64050, 24.0f, true))
					{
						UpdateWorldStateForPlayer(SM_UNK9, 1, player);
						UpdatePlayerScore(player, SCORE_CART_CONTROLLED, 1); // TEST
					}
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

}

void BattlegroundSM::CheckSomeoneLeftMineCartArea()
{

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
    // doors
    if (!AddObject(BG_SM_OBJECT_MINE_DEPOT_1, BG_SM_MINE_DEPOT, BG_SM_DepotPos[0][0], BG_SM_DepotPos[0][1], BG_SM_DepotPos[0][2], BG_SM_DepotPos[0][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY) // Waterfall
		|| !AddObject(BG_SM_OBJECT_MINE_DEPOT_2, BG_SM_MINE_DEPOT, BG_SM_DepotPos[1][0], BG_SM_DepotPos[1][1], BG_SM_DepotPos[1][2], BG_SM_DepotPos[1][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY) // Lava
		|| !AddObject(BG_SM_OBJECT_MINE_DEPOT_3, BG_SM_MINE_DEPOT, BG_SM_DepotPos[2][0], BG_SM_DepotPos[2][1], BG_SM_DepotPos[2][2], BG_SM_DepotPos[2][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY) // Diamond
		|| !AddObject(BG_SM_OBJECT_MINE_DEPOT_4, BG_SM_MINE_DEPOT, BG_SM_DepotPos[3][0], BG_SM_DepotPos[3][1], BG_SM_DepotPos[3][2], BG_SM_DepotPos[3][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)) // Troll
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundEY: Failed to spawn some object Battleground not created!");
        return false;
    }

    WorldSafeLocsEntry const* sg = NULL;
    sg = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_ALLIANCE);
    if (!sg || !AddSpiritGuide(SM_SPIRIT_ALLIANCE, sg->x, sg->y, sg->z, 3.0f, ALLIANCE))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundEY: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }

    sg = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_HORDE);
    if (!sg || !AddSpiritGuide(SM_SPIRIT_HORDE, sg->x, sg->y, sg->z, 5.9f, HORDE))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundEY: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }
    return true;
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

    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
		if (Player* player = ObjectAccessor::FindPlayer(itr->first))
		{
			if (player->GetTeam() == team)
				if (player->FindNearestCreature(NPC_MINE_CART_1, 24.0f) ||
					player->FindNearestCreature(NPC_MINE_CART_2, 24.0f) ||
					player->FindNearestCreature(NPC_MINE_CART_3, 24.0f))
					UpdatePlayerScore(player, SCORE_CART_CONTROLLED, 1);
		}
}

uint32 GetMineCartTeamKeeper(uint8 mineCart)
{
	return 0;
}

void BattlegroundSM::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(Source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found
        return;

    switch (type)
    {
        case SCORE_CART_CONTROLLED:                           // Mine Carts captures
            ((BattlegroundSMScore*)itr->second)->MineCartCaptures += value;
            break;
        default:
            Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
            break;
    }
}

void BattlegroundSM::FillInitialWorldStates(WorldPacket& data)
{
	data << uint32(SM_UNK9) << uint32(0);
	data << uint32(SM_UNK8) << uint32(0);
	data << uint32(SM_UNK7) << uint32(0);
	data << uint32(SM_UNK6) << uint32(0);
	data << uint32(SM_UNK5) << uint32(0);
	data << uint32(SM_UNK4) << uint32(0);
	data << uint32(SM_UNK3) << uint32(0);
	data << uint32(SM_UNK2) << uint32(0);
    data << uint32(SM_HORDE_RESOURCES) << uint32(m_TeamPointsCount[TEAM_HORDE]);
	data << uint32(SM_ALLIANCE_RESOURCES) << uint32(m_TeamPointsCount[TEAM_ALLIANCE]);
	data << uint32(SM_UNK1) << uint32(0);
}

uint32 BattlegroundSM::GetPrematureWinner()
{
    if (GetTeamScore(TEAM_ALLIANCE) > GetTeamScore(TEAM_HORDE))
        return ALLIANCE;
    else if (GetTeamScore(TEAM_HORDE) > GetTeamScore(TEAM_ALLIANCE))
        return HORDE;

    return Battleground::GetPrematureWinner();
}