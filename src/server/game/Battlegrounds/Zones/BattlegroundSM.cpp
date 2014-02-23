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
    m_HonorScoreTics[TEAM_HORDE] = 0;
	m_mineCartCheckTimer = 1000;
    bool isBGWeekend = sBattlegroundMgr->IsBGWeekend(GetTypeID());
    m_HonorTics = (isBGWeekend) ? BG_SM_SMWeekendHonorTicks : BG_SM_NotSMWeekendHonorTicks;
	m_IsInformedNearVictory = false;
	m_MineCartSpawnTimer = 30*IN_MILLISECONDS;
	m_LastMineCart = 0;
	m_MineCartAddPointsTimer = 2000;

    for (uint8 i = 0; i < SM_MINE_CART_MAX; ++i)
	{
		m_MineCartsProgressBar[i] = BG_SM_PROGRESS_BAR_NEUTRAL;
		m_mineCartReachedDepot[i] = false;
		m_mineCartNearDepot[i] = false;
		m_MineCartSpawned[i] = false;
	}

	for (uint8 i = 0; i < 4; ++i)
	{
		m_Depot[i] = false;
		m_DepotCloseTimer[i] = 3000;
	}
}

void BattlegroundSM::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
		BattlegroundSM::SummonMineCart(diff);
		BattlegroundSM::CheckPlayerNearMineCart(diff);
		BattlegroundSM::CheckMineCartNearDepot(diff);
		BattlegroundSM::EventReopenDepot(diff);
		BattlegroundSM::MineCartAddPoints(diff);
    }
}

void BattlegroundSM::StartingEventCloseDoors()
{
    SpawnBGObject(BG_SM_OBJECT_DOOR_A_1, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_SM_OBJECT_DOOR_H_1, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_A_2, RESPAWN_IMMEDIATELY);
	SpawnBGObject(BG_SM_OBJECT_DOOR_H_2, RESPAWN_IMMEDIATELY);

    for (uint8 i = BG_SM_OBJECT_WATERFALL_DEPOT; i < (BG_SM_OBJECT_TROLL_DEPOT + 1); ++i)
        SpawnBGObject(i, RESPAWN_ONE_DAY);
}

void BattlegroundSM::StartingEventOpenDoors()
{
	for (uint8 i = BG_SM_OBJECT_DOOR_A_1; i < (BG_SM_OBJECT_DOOR_H_2 + 1); ++i)
		SpawnBGObject(i, RESPAWN_ONE_DAY);
	
	for (uint8 i = BG_SM_OBJECT_WATERFALL_DEPOT; i < (BG_SM_OBJECT_TROLL_DEPOT + 1); ++i)
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
						if (m_LastMineCart != BG_SM_MINE_CART_1)
						{
							trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f);
							m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] = true;

							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
								m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
							}

							m_LastMineCart = BG_SM_MINE_CART_1;
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
									{
										m_LastMineCart = BG_SM_MINE_CART_2;
										m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] = true;
									}
									else
									{
										m_LastMineCart = BG_SM_MINE_CART_3;
										m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_3 - 1] = true;
									}
								}
						}
					}
					break;
				}

				case BG_SM_MINE_CART_2:
				{
					if (trigger)
					{
						if (m_LastMineCart != BG_SM_MINE_CART_2)
						{
							trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f);
							m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] = true;

							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
								m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
							}

							m_LastMineCart = BG_SM_MINE_CART_2;
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
									{
										m_LastMineCart = BG_SM_MINE_CART_1;
										m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] = true;
									}
									else
									{
										m_LastMineCart = BG_SM_MINE_CART_3;
										m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_3 - 1] = true;
									}
								}
						}
					}
					break;
				}

				case BG_SM_MINE_CART_3:
				{
					if (trigger)
					{
						if (m_LastMineCart != BG_SM_MINE_CART_3)
						{
							trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f);
							m_MineCartSpawned[BG_SM_MINE_CART_3 - 1] = true;

							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
								m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
							}

							m_LastMineCart = BG_SM_MINE_CART_3;
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
									{
										m_LastMineCart = BG_SM_MINE_CART_1;
										m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] = true;
									}
									else
									{
										m_LastMineCart = BG_SM_MINE_CART_2;
										m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] = true;
									}
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
		uint8 mineCart = 0;

		if (m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = 0;

		else if (m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			!m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			!m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = RAND(BG_SM_MINE_CART_2, BG_SM_MINE_CART_3);

		else if (!m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			!m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = RAND(BG_SM_MINE_CART_1, BG_SM_MINE_CART_3);

		else if (!m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			!m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = RAND(BG_SM_MINE_CART_1, BG_SM_MINE_CART_2);

		else if (m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			!m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = BG_SM_MINE_CART_3;

		else if (m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			!m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = BG_SM_MINE_CART_2;

		else if (!m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] &&
			m_MineCartSpawned[BG_SM_MINE_CART_3 - 1])
			mineCart = BG_SM_MINE_CART_1;

		if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
		{
			switch (mineCart)
			{
				case BG_SM_MINE_CART_1:
				{
					if (trigger)
					{
						if (m_LastMineCart != BG_SM_MINE_CART_1)
						{
							trigger->SummonCreature(NPC_MINE_CART_1, 744.542053f, 183.545883f, 319.658203f, 4.356342f);
							m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] = true;

							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
								m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
							}

							m_LastMineCart = BG_SM_MINE_CART_1;
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
									{
										m_LastMineCart = BG_SM_MINE_CART_2;
										m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] = true;
									}
									else
									{
										m_LastMineCart = BG_SM_MINE_CART_3;
										m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_3 - 1] = true;
									}
								}
						}
					}
					break;
				}

				case BG_SM_MINE_CART_2:
				{
					if (trigger)
					{
						if (m_LastMineCart != BG_SM_MINE_CART_2)
						{
							trigger->SummonCreature(NPC_MINE_CART_2, 739.400330f, 203.598511f, 319.603333f, 2.308198f);
							m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] = true;

							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
								m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
							}

							m_LastMineCart = BG_SM_MINE_CART_2;
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
									{
										m_LastMineCart = BG_SM_MINE_CART_1;
										m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] = true;
									}
									else
									{
										m_LastMineCart = BG_SM_MINE_CART_3;
										m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_3 - 1] = true;
									}
								}
						}
					}
					break;
				}

				case BG_SM_MINE_CART_3:
				{
					if (trigger)
					{
						if (m_LastMineCart != BG_SM_MINE_CART_3)
						{
							trigger->SummonCreature(NPC_MINE_CART_3, 760.184509f, 198.844742f, 319.446655f, 0.351249f);
							m_MineCartSpawned[BG_SM_MINE_CART_3 - 1] = true;

							if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f))
							{
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
								cart->SetUnitMovementFlags(MOVEMENTFLAG_BACKWARD);
								cart->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								cart->SetSpeed(MOVE_WALK, 0.4f);
								m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
							}

							m_LastMineCart = BG_SM_MINE_CART_3;
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
									{
										m_LastMineCart = BG_SM_MINE_CART_1;
										m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_1 - 1] = true;
									}
									else
									{
										m_LastMineCart = BG_SM_MINE_CART_2;
										m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
										m_MineCartSpawned[BG_SM_MINE_CART_2 - 1] = true;
									}
								}
						}
					}
					break;
				}

				default:
					break;
			}
		}
		m_MineCartSpawnTimer = 30000;
	} else m_MineCartSpawnTimer -= diff;
}

void BattlegroundSM::CheckPlayerNearMineCart(uint32 diff)
{
	if (m_mineCartCheckTimer <= 0)
	{
		for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
		{
			if (Player* player = ObjectAccessor::FindPlayer(itr->first))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					if (Creature* cart = player->FindNearestCreature(NPC_MINE_CART_1, 22.0f, true))
					{
						UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] >= 100)
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = 100;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1], player);
						}
						else
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1]++;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1], player);
						}
						
						if (m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] > BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_ALLIANCE, true);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] == BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
						}
					}

					else if (Creature* cart = player->FindNearestCreature(NPC_MINE_CART_2, 22.0f, true))
					{
						UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] >= 100)
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = 100;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1], player);
						}
						else
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1]++;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1], player);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] > BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_ALLIANCE, true);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] == BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
						}
					}

					else if (Creature* cart = player->FindNearestCreature(NPC_MINE_CART_3, 22.0f, true))
					{
						UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] >= 100)
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = 100;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1], player);
						}
						else
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1]++;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1], player);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] > BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_ALLIANCE, true);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] == BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
						}
					}

					else UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_DONT_SHOW, player);
				}
				else // for GetTeam() == HORDE
				{
					if (Creature* cart = player->FindNearestCreature(NPC_MINE_CART_1, 22.0f, true))
					{
						UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] <= 0)
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] = 0;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1], player);
						}
						else
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1]--;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1], player);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] < BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_HORDE, true);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_1 - 1] == BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
						}
					}

					else if (Creature* cart = player->FindNearestCreature(NPC_MINE_CART_2, 22.0f, true))
					{
						UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] <= 0)
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] = 0;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1], player);
						}
						else
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1]--;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1], player);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] < BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_HORDE, true);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_2 - 1] == BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
						}
					}

					else if (Creature* cart = player->FindNearestCreature(NPC_MINE_CART_3, 22.0f, true))
					{
						UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_SHOW, player);

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] <= 0)
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] = 0;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1], player);
						}
						else
						{
							m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1]--;
							UpdateWorldStateForPlayer(SM_PROGRESS_BAR_STATUS, m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1], player);
						}
						
						if (m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] < BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_NEUTRAL, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_HORDE, true);
						}

						if (m_MineCartsProgressBar[BG_SM_MINE_CART_3 - 1] == BG_SM_PROGRESS_BAR_NEUTRAL)
						{
							if (cart->HasAura(BG_SM_CONTROL_VISUAL_ALLIANCE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_ALLIANCE, cart->GetGUID());

							if (cart->HasAura(BG_SM_CONTROL_VISUAL_HORDE))
								cart->RemoveAurasDueToSpell(BG_SM_CONTROL_VISUAL_HORDE, cart->GetGUID());

							if (!cart->HasAura(BG_SM_CONTROL_VISUAL_NEUTRAL))
								cart->CastSpell(cart, BG_SM_CONTROL_VISUAL_NEUTRAL, true);
						}
					}

					else UpdateWorldStateForPlayer(SM_DISPLAY_PROGRESS_BAR, BG_SM_PROGRESS_BAR_DONT_SHOW, player);
				}

					m_mineCartCheckTimer = 1000;
			}
		}
	} else m_mineCartCheckTimer -= diff;
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
        UpdateWorldState(SM_ALLIANCE_RESOURCES, m_TeamScores[TEAM_ALLIANCE]);
    else
        UpdateWorldState(SM_HORDE_RESOURCES, m_TeamScores[TEAM_HORDE]);
}

void BattlegroundSM::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    //create score and add it to map
    BattlegroundSMScore* sc = new BattlegroundSMScore;

    PlayerScores[player->GetGUID()] = sc;
}

void BattlegroundSM::RemovePlayer(Player* player, uint64 guid, uint32 /*team*/)
{

}

bool BattlegroundSM::SetupBattleground()
{
    // doors
    if (!AddObject(BG_SM_OBJECT_WATERFALL_DEPOT, BG_SM_MINE_DEPOT, BG_SM_DepotPos[0][0], BG_SM_DepotPos[0][1], BG_SM_DepotPos[0][2], BG_SM_DepotPos[0][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)    // Waterfall
		|| !AddObject(BG_SM_OBJECT_LAVA_DEPOT, BG_SM_MINE_DEPOT, BG_SM_DepotPos[1][0], BG_SM_DepotPos[1][1], BG_SM_DepotPos[1][2], BG_SM_DepotPos[1][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY) // Lava
		|| !AddObject(BG_SM_OBJECT_DIAMOND_DEPOT, BG_SM_MINE_DEPOT, BG_SM_DepotPos[2][0], BG_SM_DepotPos[2][1], BG_SM_DepotPos[2][2], BG_SM_DepotPos[2][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY) // Diamond
		|| !AddObject(BG_SM_OBJECT_TROLL_DEPOT, BG_SM_MINE_DEPOT, BG_SM_DepotPos[3][0], BG_SM_DepotPos[3][1], BG_SM_DepotPos[3][2], BG_SM_DepotPos[3][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY) // Troll
		|| !AddObject(BG_SM_OBJECT_DOOR_A_1, BG_SM_DOOR, BG_SM_DoorPos[0][0], BG_SM_DoorPos[0][1], BG_SM_DoorPos[0][2], BG_SM_DoorPos[0][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_SM_OBJECT_DOOR_A_2, BG_SM_DOOR, BG_SM_DoorPos[1][0], BG_SM_DoorPos[1][1], BG_SM_DoorPos[1][2], BG_SM_DoorPos[1][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_SM_OBJECT_DOOR_H_1, BG_SM_DOOR, BG_SM_DoorPos[2][0], BG_SM_DoorPos[2][1], BG_SM_DoorPos[2][2], BG_SM_DoorPos[2][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY)
		|| !AddObject(BG_SM_OBJECT_DOOR_H_2, BG_SM_DOOR, BG_SM_DoorPos[3][0], BG_SM_DoorPos[3][1], BG_SM_DoorPos[3][2], BG_SM_DoorPos[3][3], 0, 0, 0.710569f, -0.703627f, RESPAWN_IMMEDIATELY))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundSM: Failed to spawn some object Battleground not created!");
        return false;
    }

    WorldSafeLocsEntry const* sg = NULL;
    sg = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_ALLIANCE);
    if (!sg || !AddSpiritGuide(SM_SPIRIT_ALLIANCE, sg->x, sg->y, sg->z, 2.138462f, ALLIANCE))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundSM: Failed to spawn spirit guide! Battleground not created!");
        return false;
    }

    sg = sWorldSafeLocsStore.LookupEntry(SM_GRAVEYARD_MAIN_HORDE);
    if (!sg || !AddSpiritGuide(SM_SPIRIT_HORDE, sg->x, sg->y, sg->z, 5.570653f, HORDE))
    {
        sLog->outError(LOG_FILTER_SQL, "BatteGroundSM: Failed to spawn spirit guide! Battleground not created!");
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

void BattlegroundSM::CheckMineCartNearDepot(uint32 diff)
{
	Creature* trigger = NULL;
	if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
	{
		if (!m_mineCartNearDepot[0])
		{
			if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f, true))
			{
				if (cart->GetExactDist2d(BG_SM_DepotPos[1][0], BG_SM_DepotPos[1][1]) <= 8.0f)
				{
					m_Depot[SM_LAVA_DEPOT] = true;
					BattlegroundSM::EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), BG_SM_MINE_CART_1);
					m_mineCartNearDepot[0] = true;
				}

				if (cart->GetExactDist2d(BG_SM_DepotPos[2][0], BG_SM_DepotPos[2][1]) <= 8.0f)
				{
					m_Depot[SM_DIAMOND_DEPOT] = true;
					BattlegroundSM::EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), BG_SM_MINE_CART_1);
					m_mineCartNearDepot[0] = true;
				}
			}
		}

		if (!m_mineCartNearDepot[1])
		{
			if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f, true))
			{
				if (cart->GetExactDist2d(BG_SM_DepotPos[0][0], BG_SM_DepotPos[0][1]) <= 8.0f)
				{
					m_Depot[SM_WATERFALL_DEPOT] = true;
					BattlegroundSM::EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_2), BG_SM_MINE_CART_2);
					m_mineCartNearDepot[1] = true;
				}
			}
		}

		if (!m_mineCartNearDepot[2])
		{
			if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_3, 99999.0f, true))
			{
				if (cart->GetExactDist2d(BG_SM_DepotPos[2][0], BG_SM_DepotPos[2][1]) <= 8.0f)
				{
					m_Depot[SM_DIAMOND_DEPOT] = true;
					BattlegroundSM::EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), BG_SM_MINE_CART_3);
					m_mineCartNearDepot[2] = true;
				}

				if (cart->GetExactDist2d(BG_SM_DepotPos[3][0], BG_SM_DepotPos[3][1]) <= 8.0f)
				{
					m_Depot[SM_TROLL_DEPOT] = true;
					BattlegroundSM::EventTeamCapturedMineCart(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), BG_SM_MINE_CART_3);
					m_mineCartNearDepot[2] = true;
				}
			}
		}
	}
}

void BattlegroundSM::EventTeamCapturedMineCart(uint32 team, uint8 mineCart)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
	{
		if (Player* player = ObjectAccessor::FindPlayer(itr->first))
		{
			if (player->GetTeam() == team)
				if (player->FindNearestCreature(NPC_MINE_CART_1, 22.0f, true) ||
					player->FindNearestCreature(NPC_MINE_CART_2, 22.0f, true) ||
					player->FindNearestCreature(NPC_MINE_CART_3, 22.0f, true))
				{
					UpdatePlayerScore(player, SCORE_CART_CONTROLLED, 1);
					player->RewardHonor(player, 1, irand(10, 12));
				}
		}
	}

	Creature* trigger = NULL;
	if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
	{
		switch (mineCart)
		{
			case BG_SM_MINE_CART_1:
			{
				if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f, true))
				{
					if (m_Depot[SM_LAVA_DEPOT])
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_LAVA_DEPOT]))
						{
							cart->GetMotionMaster()->MoveIdle();
							depot->UseDoorOrButton();
							m_mineCartReachedDepot[BG_SM_MINE_CART_1 - 1] = true;
						}
					}

					if (m_Depot[SM_DIAMOND_DEPOT])
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
						{
							cart->GetMotionMaster()->MoveIdle();
							depot->UseDoorOrButton();
							m_mineCartReachedDepot[BG_SM_MINE_CART_1 - 1] = true;
						}
					}
				}
				break;
			}

			case BG_SM_MINE_CART_2:
			{
				if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f, true))
				{
					if (m_Depot[SM_WATERFALL_DEPOT])
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_WATERFALL_DEPOT]))
						{
							cart->GetMotionMaster()->MoveIdle();
							depot->UseDoorOrButton();
							m_mineCartReachedDepot[BG_SM_MINE_CART_2 - 1] = true;
						}
					}
				}
				break;
			}

			case BG_SM_MINE_CART_3:
			{
				if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_3, 99999.0f, true))
				{
					if (m_Depot[SM_DIAMOND_DEPOT])
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
						{
							cart->GetMotionMaster()->MoveIdle();
							depot->UseDoorOrButton();
							m_mineCartReachedDepot[BG_SM_MINE_CART_3 - 1] = true;
						}
					}

					if (m_Depot[SM_TROLL_DEPOT])
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_TROLL_DEPOT]))
						{
							cart->GetMotionMaster()->MoveIdle();
							depot->UseDoorOrButton();
							m_mineCartReachedDepot[BG_SM_MINE_CART_3 - 1] = true;
						}
					}
				}
				break;
			}
		}
	}
}

void BattlegroundSM::EventReopenDepot(uint32 diff)
{
	Creature* trigger = NULL;
	if (m_mineCartReachedDepot[BG_SM_MINE_CART_1 - 1])
	{
		if (m_Depot[SM_LAVA_DEPOT])
		{
			if (m_DepotCloseTimer[SM_LAVA_DEPOT] <= 0)
			{
				if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
				{
					if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f, true))
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_LAVA_DEPOT]))
						{
							BattlegroundSM::AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), POINTS_PER_MINE_CART);
							BattlegroundSM::ResetDepotsAndMineCarts(SM_LAVA_DEPOT, BG_SM_MINE_CART_1);
							depot->ResetDoorOrButton();
							cart->DespawnOrUnsummon();
						}
					}
				}
			} else m_DepotCloseTimer[SM_LAVA_DEPOT] -= diff;
		}

		if (m_Depot[SM_DIAMOND_DEPOT])
		{
			if (m_DepotCloseTimer[SM_DIAMOND_DEPOT] <= 0)
			{
				if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
				{
					if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_1, 99999.0f, true))
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
						{
							BattlegroundSM::AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_1), POINTS_PER_MINE_CART);
							BattlegroundSM::ResetDepotsAndMineCarts(SM_DIAMOND_DEPOT, BG_SM_MINE_CART_1);
							depot->ResetDoorOrButton();
							cart->DespawnOrUnsummon();
						}
					}
				}
			} else m_DepotCloseTimer[SM_LAVA_DEPOT] -= diff;
		}
	}

	if (m_mineCartReachedDepot[BG_SM_MINE_CART_2 - 1])
	{
		if (m_Depot[SM_WATERFALL_DEPOT])
		{
			if (m_DepotCloseTimer[SM_WATERFALL_DEPOT] <= 0)
			{
				if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
				{
					if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_2, 99999.0f, true))
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_WATERFALL_DEPOT]))
						{
							BattlegroundSM::AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_2), POINTS_PER_MINE_CART);
							BattlegroundSM::ResetDepotsAndMineCarts(SM_WATERFALL_DEPOT, BG_SM_MINE_CART_2);
							depot->ResetDoorOrButton();
							cart->DespawnOrUnsummon();
						}
					}
				}
			} else m_DepotCloseTimer[SM_WATERFALL_DEPOT] -= diff;
		}
	}

	if (m_mineCartReachedDepot[BG_SM_MINE_CART_3 - 1])
	{
		if (m_Depot[SM_DIAMOND_DEPOT])
		{
			if (m_DepotCloseTimer[SM_DIAMOND_DEPOT] <= 0)
			{
				if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
				{
					if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_3, 99999.0f, true))
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_DIAMOND_DEPOT]))
						{
							BattlegroundSM::AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), POINTS_PER_MINE_CART);
							BattlegroundSM::ResetDepotsAndMineCarts(SM_DIAMOND_DEPOT, BG_SM_MINE_CART_3);
							depot->ResetDoorOrButton();
							cart->DespawnOrUnsummon();
						}
					}
				}
			} else m_DepotCloseTimer[SM_DIAMOND_DEPOT] -= diff;
		}

		if (m_Depot[SM_TROLL_DEPOT])
		{
			if (m_DepotCloseTimer[SM_TROLL_DEPOT] <= 0)
			{
				if (trigger = HashMapHolder<Creature>::Find(BgCreatures[SM_MINE_CART_TRIGGER]))
				{
					if (Creature* cart = trigger->FindNearestCreature(NPC_MINE_CART_3, 99999.0f, true))
					{
						if (GameObject* depot = HashMapHolder<GameObject>::Find(BgObjects[BG_SM_OBJECT_TROLL_DEPOT]))
						{
							BattlegroundSM::AddPoints(GetMineCartTeamKeeper(BG_SM_MINE_CART_3), POINTS_PER_MINE_CART);
							BattlegroundSM::ResetDepotsAndMineCarts(SM_TROLL_DEPOT, BG_SM_MINE_CART_3);
							depot->ResetDoorOrButton();
							cart->DespawnOrUnsummon();
						}
					}
				}
			} else m_DepotCloseTimer[SM_TROLL_DEPOT] -= diff;
		}
	}
}

uint32 BattlegroundSM::GetMineCartTeamKeeper(uint8 mineCart)
{
	if (m_MineCartsProgressBar[mineCart - 1] > BG_SM_PROGRESS_BAR_NEUTRAL)
		return ALLIANCE;

	else if (m_MineCartsProgressBar[mineCart - 1] < BG_SM_PROGRESS_BAR_NEUTRAL)
		return HORDE;

	else return 0;
}

void BattlegroundSM::MineCartAddPoints(uint32 diff)
{
	if (m_MineCartAddPointsTimer <= 0)
	{
		   /* ==================  ALLIANCE  ================== */
		if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 5);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) != ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 3);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) != ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 3);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) != ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 3);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) != ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) != ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 2);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) != ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) != ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 2);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) != ALLIANCE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) != ALLIANCE)
			BattlegroundSM::AddPoints(ALLIANCE, 2);

		    /* ==================  HORDE  ================== */
		if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == HORDE)
			BattlegroundSM::AddPoints(HORDE, 5);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) != HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == HORDE)
			BattlegroundSM::AddPoints(HORDE, 3);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) != HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == HORDE)
			BattlegroundSM::AddPoints(HORDE, 3);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) != HORDE)
			BattlegroundSM::AddPoints(HORDE, 3);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) != HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) != HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) == HORDE)
			BattlegroundSM::AddPoints(HORDE, 2);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) != HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) != HORDE)
			BattlegroundSM::AddPoints(HORDE, 2);

		else if (GetMineCartTeamKeeper(BG_SM_MINE_CART_1) == HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_2) != HORDE &&
			GetMineCartTeamKeeper(BG_SM_MINE_CART_3) != HORDE)
			BattlegroundSM::AddPoints(HORDE, 2);

		m_MineCartAddPointsTimer = 2000;
	} else m_MineCartAddPointsTimer -= diff;
}

void BattlegroundSM::ResetDepotsAndMineCarts(uint8 depot, uint8 mineCart)
{
	m_DepotCloseTimer[depot] = 3000;
	m_Depot[depot] = false;
	m_MineCartsProgressBar[mineCart - 1] = BG_SM_PROGRESS_BAR_NEUTRAL;
	m_mineCartNearDepot[mineCart - 1] = false;
	m_mineCartReachedDepot[mineCart - 1] = false;
	m_MineCartSpawned[mineCart - 1] = false;
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
	data << uint32(SM_UNK1) << uint32(1);
	data << uint32(SM_ALLIANCE_RESOURCES) << uint32(m_TeamScores[TEAM_HORDE]);
	data << uint32(SM_HORDE_RESOURCES) << uint32(m_TeamScores[TEAM_ALLIANCE]);
	data << uint32(SM_UNK2) << uint32(1);
	data << uint32(SM_UNK3) << uint32(1);
	data << uint32(SM_UNK4) << uint32(1);
	data << uint32(SM_DISPLAY_ALLIANCE_RESSOURCES) << uint32(1);
	data << uint32(SM_DISPLAY_HORDE_RESSOURCES) << uint32(1);
	data << uint32(SM_DISPLAY_PROGRESS_BAR) << uint32(BG_SM_PROGRESS_BAR_DONT_SHOW); // This shows the mine cart control bar
	data << uint32(SM_PROGRESS_BAR_STATUS) << uint32(BG_SM_PROGRESS_BAR_NEUTRAL); // Neutral
	data << uint32(SM_UNK5) << uint32(0);
}

uint32 BattlegroundSM::GetPrematureWinner()
{
    if (GetTeamScore(TEAM_ALLIANCE) > GetTeamScore(TEAM_HORDE))
        return ALLIANCE;
    else if (GetTeamScore(TEAM_HORDE) > GetTeamScore(TEAM_ALLIANCE))
        return HORDE;

    return Battleground::GetPrematureWinner();
}