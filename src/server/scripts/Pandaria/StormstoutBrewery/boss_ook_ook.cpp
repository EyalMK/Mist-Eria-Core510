#include "stormstout_brewery.h"
#include "ScriptedEscortAI.h"

enum Spells
{
    // Ook Ook
    Spell_OokOok_GroundPound = 106807,
    Spell_OokOok_GroundPoundPeriodic = 106808,
    Spell_OokOok_GoingBananas = 106651,
    Spell_OokOok_GoindBananasPeriodic = 115978,

    // Barrel
    Spell_Barrel_BrewExplosion = 106769,
    Spell_Barrel_BrewExplosionHozen = 107351,
    Spell_Barrel_BrewExplosionOokOok = 106784
};

enum Creatures
{
    Npc_RollingBarrel = 56682,
    Vehicle_Barrel = 0 /** @todo : find that thing !*/
};

enum Events
{
    // Ook Ook
    Event_OokOok_SpawnBarrel = 1,
    Event_OokOok_GroundPound = 2
};

enum Actions
{
    // Ook Ook
    Action_OokOok_Land = 0
};

enum Says
{
    Talk_OokOok_Land = 1,
    Talk_OokOok_EnterCombat = 2,
    Talk_OokOok_GoingBananas_One = 3,
    Talk_OokOok_GoingBananas_Two = 4,
    Talk_OokOok_GoingBananas_Three = 5,
    Talk_OokOok_KilledPlayer = 6,
    Talk_OokOok_JustDied = 7
};

// This is where OokOok will land when the hozen party is disturbed ; also, it is his reset position ;
static const Position ookOokLandingPosition = {0.0f, 0.0f, 0.0f, 0.0f};


// First line : starting position ;
// Second line : ending position ;
static const Position barrelsPositions[2][5] =
{
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f}
    },
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f}
    }
};

bool PositionsEgalityCheck(Position const* pos, Position const* otherPos)
{
    if(!pos || !otherPos)
        return false ;

    return (pos->GetPositionX() == otherPos->GetPositionX() && pos->GetPositionY() == otherPos->GetPositionY() && pos->GetPositionZ() == otherPos->GetPositionZ());
}

class boss_ook_ook : public CreatureScript
{
public :
    boss_ook_ook() : CreatureScript("boss_ook_ook")
    {

    }

    struct boss_ook_ook_AIScript : public ScriptedAI
    {
    public :
        boss_ook_ook_AIScript(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            m_bIsStarted = false ; // Since we construct the object before reset() (this is logic), we must initialize the boolean here
            m_ucFirstSummonCount = 0 ; // We do not initialize it during Reset() because once the boss is started he will never summon barrels by himself
            m_uiNextGoingBananasPercent = 90 ;
            m_ucGoingBananasCount = 0 ;
        }

        void Reset()
        {
            // Always reset the EventMap before anything ;
            events.Reset();
            if(instance)
                instance->SetData(Data_OokOokEventProgress, NOT_STARTED);

            if(!m_bIsStarted)
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : OokOok Reset() vut he is not yet started ; however, if he was spawned this is normal");
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE); // The boss shouldn't do anything before he starts ;
                events.ScheduleEvent(Event_OokOok_SpawnBarrel, 5000); // We summon a barrel every five seconds
                /** @warning : this events is scheduled only five times, to summon five barrels ; otherwise, it is handled by the barrels  */
                return ; // Nothing more
            }

            m_uiNextGoingBananasPercent = 90 ;
        }

        // Since the boss is not supposed to be immediately attackable, EnterEvadeMode() would lead him out of players range
        // So, we need to override the function (and by the way, to call Reset() here, instead of ScriptedAI::EnterEvadeMode();
        void EnterEvadeMode()
        {
            if(instance)
                instance->SetData(Data_OokOokEventProgress, FAIL);
            if(!m_bIsStarted)
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Instance Stormstout Brewery : OokOok EnterEvadeMode() but he is not yet started !");
                return ; // Prevents resetting if not needed
            }

            if(me->GetMotionMaster())
                me->GetMotionMaster()->MovePoint(0, ookOokLandingPosition);
            Reset();
        }

        void DoAction(const int32 iActionId)
        {
            switch(iActionId)
            {
            case Action_OokOok_Land :
                m_bIsStarted = true ;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE);
                if(me->GetMotionMaster())
                    me->GetMotionMaster()->MoveJump(ookOokLandingPosition, 1, 1);
                Talk(Talk_OokOok_Land);
                break ;
            }
        }

        void EnterCombat(Unit *attacker)
        {
            if(instance)
                instance->SetData(Data_OokOokEventProgress, IN_PROGRESS);
            DoZoneInCombat();
            Talk(Talk_OokOok_EnterCombat);
            events.CancelEvent(Event_OokOok_SpawnBarrel);
            events.ScheduleEvent(Event_OokOok_GroundPound, IsHeroic() ? urand(5000, 7000) : urand(6000, 9000));
        }

        void DamageTaken(Unit *doneBy, uint32 &amount)
        {
            uint32 life = me->GetHealth() - amount ;
            uint32 percent = me->CountPctFromMaxHealth(life);

            if(life <= m_uiNextGoingBananasPercent)
            {
                DoCast(me, Spell_OokOok_GoingBananas, true);
                Talk(Talk_OokOok_GoingBananas_One + m_ucGoingBananasCount);
                m_uiNextGoingBananasPercent -= 30 ;
                ++m_ucGoingBananasCount;
            }
        }

        void KilledUnit(Unit *killed)
        {
            if(killed->GetTypeId() == TYPEID_PLAYER)
                Talk(Talk_OokOok_KilledPlayer);
        }

        void JustDied(Unit *killer)
        {
            if(instance)
                instance->SetData(Data_OokOokEventProgress, DONE);
            Talk(Talk_OokOok_JustDied);
            RemoveSpawned(Npc_RollingBarrel);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            // Even if he is not yet started, he must spawn barrels
            if(!m_bIsStarted)
            {
                events.Update(uiDiff);

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                    case Event_OokOok_SpawnBarrel :
                        if(m_ucFirstSummonCount < 5) // The first time, he summons five barrel, at a 5 seconds interval
                        {
                            SummonBarrel(m_ucFirstSummonCount);
                            events.ScheduleEvent(Event_OokOok_SpawnBarrel, 5000);
                            ++m_ucFirstSummonCount;
                        }
                        break ;

                    default :
                        break ;
                    }
                }

                return ;
            }

            // Return since we have no target
            if(!UpdateVictim())
                return ;

            events.Update(uiDiff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return ;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case Event_OokOok_GroundPound :
                    DoCast(Spell_OokOok_GroundPound);
                    // Watch timers, with the Going Bananas buff it can become hard for players to handle damages
                    events.ScheduleEvent(Event_OokOok_GroundPound, IsHeroic() ? urand(4000, 5000) : urand(5000, 7000));
                    break ;

                default :
                    break ;
                }
            }

            DoMeleeAttackIfReady();
        }

        void SummonBarrel(const uint8 ucId)
        {
            Creature* barrel = me->SummonCreature(Npc_RollingBarrel, barrelsPositions[ucId][0], TEMPSUMMON_MANUAL_DESPAWN, 0, Vehicle_Barrel);
            if(barrel)
                if(barrel->GetMotionMaster())
                    barrel->GetMotionMaster()->MovePoint(0, barrelsPositions[ucId][1]);
        }

        void RemoveSpawned(const uint32 entry)
        {
            std::list<Creature*> spawned ;
            GetCreatureListWithEntryInGrid(spawned, me, entry, 50000.0f);

            for(std::list<Creature*>::iterator iter = spawned.begin() ; iter != spawned.end() ; ++iter)
            {
                if(Creature* summoned = *iter)
                {
                    summoned->DisappearAndDie();
                    summoned->DespawnOrUnsummon(); // Just in case
                }
            }
        }

        void EndEvent()
        {
            // Lots of things to do right here, but not very important
        }

    private :
        EventMap events ;
        InstanceScript* instance ;

        bool m_bIsStarted ;
        uint8 m_ucFirstSummonCount ;
        uint8 m_ucGoingBananasCount ;
        uint32 m_uiNextGoingBananasPercent ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new boss_ook_ook_AIScript(creature);
    }
};

class npc_rolling_barrel : public CreatureScript
{
public :
    npc_rolling_barrel() : CreatureScript("npc_rolling_barrel")
    {

    }

    struct npc_rolling_barrel_AIScript : public npc_escortAI
    {
    public:
        npc_rolling_barrel_AIScript(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
            m_uiCheckTimer = 500 ;
        }

        void PassengerBoarded(Unit *passenger, int8 seat, bool boolean)
        {
            if(me->GetMotionMaster())
                me->GetMotionMaster()->Clear(false);
        }

        void JustDied(Unit *killer)
        {
            for(uint8 i = 0 ; i < 5 ; ++i)
            {
                if(PositionsEgalityCheck(&homePosition, &barrelsPositions[i][0]))
                {
                    if(Creature* ookOok = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(Data_BossOokOok) : 0))
                        CAST_AI(boss_ook_ook::boss_ook_ook_AIScript, ookOok->AI())->SummonBarrel(i);
                }
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if(me->GetVehicleKit())
            {
                if(!me->GetVehicleKit()->IsVehicleInUse())
                {
                    for(uint8 i = 0 ; i < 5 ; ++i)
                    {
                        Position pos ;
                        me->GetPosition(&pos);

                        if(PositionsEgalityCheck(&pos, &barrelsPositions[i][1]))
                        {
                            DoCast(Spell_Barrel_BrewExplosion);
                            me->Kill(me);
                            return ;
                        }
                    }
                    return ;
                }
            }

            if(m_uiCheckTimer <= uiDiff)
            {
                if(!DoCheckZone())
                    m_uiCheckTimer = 500 ;
                else
                    me->Kill(me);
            }
            else
                m_uiCheckTimer -= uiDiff ;
        }
		
		void WaypointReached(uint32 m_uiWaypointId)
		{
			//! FIX THAT THING !!!
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Something happened") ;
		}

        bool DoCheckZone()
        {
            // Remember : implicit targets do not allow player to be hit by the spell !

            // If Ook Ook is not started, it is pointless to check him
            if(instance && instance->GetData(Data_OokOokEventProgress) == NOT_STARTED)
            {
                for(uint8 i = 0 ; i < MAX_OOKOOK_ENTOURAGE ; ++i)
                {
                    uint32 entry = OokOokEntourage[i];
                    std::list<Creature*> minions = GetMinions(entry);

                    if(CheckDistanceWithMinions(minions))
                        return true ;
                }
            }
            else
            {
                if(instance)
                {
                    if(Creature* ookOok = ObjectAccessor::GetCreature(*me, instance->GetData64(Data_BossOokOok)))
                    {
                        if(me->GetExactDist2d(ookOok->GetPositionX(), ookOok->GetPositionY()))
                        {
                            DoCast(ookOok, Spell_Barrel_BrewExplosion);
                            return true ;
                        }
                    }
                }
            }

            return false ;
        }

        std::list<Creature*> GetMinions(const uint32 entry)
        {
            std::list<Creature*> minions ;
            GetCreatureListWithEntryInGrid(minions, me, entry, 1000.0f);

            return minions ;
        }

        bool CheckDistanceWithMinions(std::list<Creature*> const& minions)
        {
            if(!minions.empty())
            {
                for(std::list<Creature*>::const_iterator iter = minions.begin() ; iter != minions.end() ; ++iter)
                {
                    if(Creature* minion = *iter)
                    {
                        if(me->GetExactDist2d(minion->GetPositionX(), minion->GetPositionY()) <= 2.0f)
                        {
                            DoCast(minion, Spell_Barrel_BrewExplosion);
                            return true;
                        }
                    }
                }
            }

            return false ;
        }

        void SetHomePosition(Position const& position)
        {
            homePosition.Relocate(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ());
        }

    private :
        InstanceScript* instance ;
        uint32 m_uiCheckTimer;
        Position homePosition ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_rolling_barrel_AIScript(creature);
    }
};

void AddSC_boss_ook_ook()
{
    new boss_ook_ook();
    new npc_rolling_barrel();
}
