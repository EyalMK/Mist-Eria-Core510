#include "stormstout_brewery.h"

/********************************************************/
/********************* OOK OOK **************************/
/********************************************************/

/** Dialogue :
 *  Auntie : Oh hello Zan, it is good of you...
 *  Chen : I am not Zan - I am Chen
 *  Auntie : Oh Zan, you remind
 *  Chen : Tell me what has happened here
 *  Auntie : it is certainly a nice day outside
 *  Chen : Where are the others Stormstout
 *  Auntie : Have you seen the size of
 *  Chen : Auntie Stormstout... why is the brewery abandonned ?
 *  Auntie : Abandonned ? Oh heavens
 *  Chen : I see and where is uncle gao ?
 *  Auntie : I have some cookies for you
 *  Chen : There is no time for cookies
 *  Chen : Wait these are...
 */

#define DIALOGUE_STOP_TIMER 0xFFFFFF

class npc_chen_stormstout : public CreatureScript
{
public :
    npc_chen_stormstout() : CreatureScript("npc_chen_stormstout")
    {

    }

    class npc_chen_stormstoutAI : public ScriptedAI
    {
    public :
        npc_chen_stormstoutAI(Creature* creature) : ScriptedAI(creature)
        {
            m_uiTalkId = 0 ;
            m_bCanTalk = true ;
        }

        void DoAction(const int32 action)
        {
            if(action == 1 && m_bCanTalk)
            {
                m_bCanTalk = false ;
                m_uiTalkTimer = 6000 ;
                if(Creature* auntie = me->FindNearestCreature(59822, 40.0f, true))
                    auntie->AI()->DoAction(1);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            // Return since the dialogue is ended
            if(m_uiTalkTimer == DIALOGUE_STOP_TIMER)
                return ;

            if(m_uiTalkTimer <= diff)
            {
                Talk(m_uiTalkId);
                switch(m_uiTalkId)
                {
                case 0 : // After "I am not"
                    m_uiTalkTimer = 16000 ;
                    break ;

                case 1 : // After "Tell me"
                    m_uiTalkTimer = 10000 ;
                    break ;

                case 2 : // After "Where are"
                    m_uiTalkTimer = 11000 ;
                    break ;

                case 3 : // After "Auntie Stormstout"
                    m_uiTalkTimer = 18000 ;
                    break ;

                case 4 : // After "I see"
                    m_uiTalkTimer = 9000 ;
                    break ;

                case 5 : // After "there is no time"
                    m_uiTalkTimer = 30000 ;
                    break ;

                case 6 :
                    m_uiTalkTimer = DIALOGUE_STOP_TIMER ;
                    break ;

                default :
                    break ;
                }
                ++m_uiTalkId;
            }
            else
                m_uiTalkTimer -= diff ;
        }

    private :
        uint32 m_uiTalkTimer ;
        uint32 m_uiTalkId ;
        bool m_bCanTalk ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_chen_stormstoutAI(creature);
    }
};

class npc_auntie_stormstout : public CreatureScript
{
public :
    npc_auntie_stormstout() : CreatureScript("npc_auntie_stormstout")
    {

    }

    class npc_auntie_stormstoutAI : public ScriptedAI
    {
    public :
        npc_auntie_stormstoutAI(Creature* creature) : ScriptedAI(creature)
        {
            m_uiTalkTimer = DIALOGUE_STOP_TIMER ;
        }

        void DoAction(const int32 action)
        {
            if(action == 1)
            {
                m_uiTalkTimer = 200 ;
                m_uiTalkId = 0 ;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            // Return since the dialogue is not started
            if(m_uiTalkTimer == DIALOGUE_STOP_TIMER)
                return ;

            if(m_uiTalkTimer <= diff)
            {
                Talk(m_uiTalkId);
                switch(m_uiTalkId)
                {
                case 0 : // After "Oh Hello"
                    m_uiTalkTimer = 12000 ;
                    break ;

                case 1 : // After "Oh Zan"
                    m_uiTalkTimer = 10000 ;
                    break ;

                case 2 : // After "It is certainly"
                    m_uiTalkTimer = 11000 ;
                    break ;

                case 3 : // After "Have you seen"
                    m_uiTalkTimer = 10000 ;
                    break ;

                case 4 : // After "Abandonned"
                    m_uiTalkTimer = 17000 ;
                    break ;

                case 5 : // After "I have"
                    m_uiTalkTimer = DIALOGUE_STOP_TIMER ;
                    break ;

                default :
                    break ;
                }

                ++m_uiTalkId;
            }
            else
                m_uiTalkTimer -= diff ;
        }

    private :
        uint32 m_uiTalkTimer;
        uint32 m_uiTalkId ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_auntie_stormstoutAI(creature);
    }
};

class npc_sb_illusioner : public CreatureScript
{
public :
    npc_sb_illusioner() : CreatureScript("npc_sb_illusioner")
    {

    }

    class npc_sb_illusionerAI : public ScriptedAI
    {
    public :
        npc_sb_illusionerAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            m_bWater = (me->GetEntry() == 56865);
            CheckForTarget();
        }

        void UpdateAI(const uint32 diff)
        {
            if(InstanceScript* instance = me->GetInstanceScript())
                if(instance->GetData(INSTANCE_DATA_OOK_OOK_STATUS) == DONE)
                    return ; // return since there is no need to continue

            if(!target)
            {
                if(m_uiCheckForTarget <= diff)
                {
                    bool targetB = CheckForTarget() ;
                    if(!targetB)
                        m_uiCheckForTarget = 1000 ;
                }
                else
                    m_uiCheckForTarget -= diff ;
            }
        }

        void JustDied(Unit *killer)
        {
            if(target && target->IsInWorld())
            {
                target->RemoveAurasDueToSpell(m_bWater ? 107044 : 107175);
                target->CastSpell(target, 114656);
            }
        }

        bool CheckForTarget()
        {
            CreatureList drunkenHozens ;
            GetCreatureListWithEntryInGrid(drunkenHozens, me, m_bWater ? 56862 : 56924, 50000.0f);

            for(CreatureListConstIter iter = drunkenHozens.begin() ; iter != drunkenHozens.end() ; ++iter)
            {
                if(Creature* hozen = *iter)
                {
                    if(!hozen->HasAura(m_bWater ? 107044 : 107175))
                    {
                        DoCast(hozen, m_bWater ? 107044 : 107175, true);
                        target = hozen ;
                        return true ;
                    }
                }
            }

            return false ;
        }

    private :
        Creature* target ; // bool m_bNeedTarget ;
        uint32 m_uiCheckForTarget ;
        bool m_bWater ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_sb_illusionerAI(creature);
    }
};

class mob_habanero_brew : public CreatureScript
{
public :
    mob_habanero_brew() : CreatureScript("mob_habanero_brew")
    {

    }

    class mob_habanero_brewAI : public ScriptedAI
    {
    public :
        mob_habanero_brewAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void JustDied(Unit *killer)
        {
            DoCastAOE(106769, true);
            DoCastAOE(107016, true);
        }
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_habanero_brewAI(creature);
    }
};

const Position hozenFleePosition = {0.0f, 0.0f, 0.0f, 0.0f} ;

class mob_ook_ook_hozen : public CreatureScript
{
public :
    mob_ook_ook_hozen() : CreatureScript("mob_ook_ook_hozen")
    {

    }

    class mob_ook_ook_hozenAI : public ScriptedAI
    {
    public :
        mob_ook_ook_hozenAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {

        }

        void MovementInform(uint32 type, uint32 id)
        {
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Entering MovementInform (Hozen Party Animal), using type %u, and id %u", type, id);
            if(type == POINT_MOTION_TYPE)
            {
                if(id == 0)
                {
                    me->DespawnOrUnsummon();
                }
            }
        }

        void DoAction(const int32 action)
        {
            if(action == 1)
            {
                if(me->GetEntry() == 56927)
                    Flee();
            }
        }

        void JustDied(Unit *killer)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Creature entered JustDied");
            if(InstanceScript* instance = me->GetInstanceScript())
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "Creature entered JustDied");
                instance->SetData64(INSTANCE_DATA64_KILLED_HOZENS, 1);
				sLog->outDebug(LOG_FILTER_NETWORKIO, "Data set in instancescript ; ready to change power");
                AddPowerOnPlayers(1);
            }
        }

        void AddPowerOnPlayer(Player* p, int32 amount)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "entered addpoweronplayer");
            if(p)
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "player not null ; seting power");
                p->ModifyPower(POWER_ALTERNATE_POWER, amount);
			}
        }

        void AddPowerOnPlayers(int32 amount)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Entered addpoweronplayers");
            if(Map* map = me->GetMap())
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "map not null");
                Map::PlayerList const & playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                {
					sLog->outDebug(LOG_FILTER_NETWORKIO, "playerlist not empty");
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
						sLog->outDebug(LOG_FILTER_NETWORKIO, "ready to apply change");
                        AddPowerOnPlayer(iter->getSource(), amount);
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return ;

            DoMeleeAttackIfReady();
        }

        void Flee()
        {
            me->GetMotionMaster()->MovePoint(0, hozenFleePosition);
        }
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_ook_ook_hozenAI(creature);
    }
};

class npc_rolling_barrel : public CreatureScript
{
public :
    npc_rolling_barrel() : CreatureScript("npc_rolling_barrel")
    {

    }

    class npc_rolling_barrelAI : public ScriptedAI
    {
    public :
        npc_rolling_barrelAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if(type == POINT_MOTION_TYPE)
            {
                if(instance)
                    instance->ProcessEvent(NULL, m_uiIndex);
                DoCastAOE(115875);
                me->Kill(me);
                return ;
            }
        }

        void PassengerBoarded(Unit *passenger, int8 seatId, bool boolean)
        {
            me->GetMotionMaster()->Clear(false);
        }

        void SetData(uint32 uiData, uint32 uiValue)
        {
            if(uiData == NPC_ROLLING_BARREL_DATA_SUMMONING_ID)
                m_uiIndex = uiValue ;
        }

        void Reset()
        {
            if(me->GetMapId() == 961)
                m_uiCheckTimer = 1000 ;
        }

        void JustDied(Unit *killer)
        {
            if(Vehicle* vec = me->GetVehicleKit())
                vec->RemoveAllPassengers();
        }

        void UpdateAI(const uint32 diff)
        {
            if(me->GetMapId() != 961)
                return ; // Return since we are not ine the right map
            if(m_uiCheckTimer <= diff)
            {
                if(instance)
                {
                    instance->GetData(INSTANCE_DATA_OOK_OOK_STATUS) == IN_PROGRESS ? DoCheckOokOok() : DoCheckHozens();
                    m_uiCheckTimer = 1000 ;
                }
            }
            else
                m_uiCheckTimer -= diff ;
        }

        void DoCheckOokOok()
        {
            if(instance)
            {
                if(Creature* ookOok = ObjectAccessor::GetCreature(*me, instance->GetData64(INSTANCE_DATA64_OOK_OOK_GUID)))
                {
                    float dist = me->GetExactDist2d(ookOok->GetPositionX(), ookOok->GetPositionY());
                    if(dist <= 1.0f)
                    {
                        DoCastAOE(115875, true);
                        me->Kill(me);
                        return ;
                    }
                }
            }
        }

        void DoCheckHozens()
        {
            CreatureList hozens[3] ;
            GetCreatureListWithEntryInGrid(hozens[0], me, 56927, 50000.0f);
            GetCreatureListWithEntryInGrid(hozens[1], me, 59684, 50000.0f);
            GetCreatureListWithEntryInGrid(hozens[2], me, 57097, 50000.0f);

            for(uint8 i = 0 ; i < 3 ; ++i)
            {
                CreatureList creatures = hozens[i];
                for(CreatureListConstIter iter = creatures.begin() ; iter != creatures.end() ; ++iter)
                {
                    if(Creature* c = *iter)
                    {
                        float dist = me->GetExactDist2d(c->GetPositionX(), c->GetPositionZ());
                        if(dist <= 1.0f)
                        {
                            DoCastAOE(115875, true);
                            me->Kill(me);;
                            return ;
                        }
                    }
                }
            }
        }

    private :
        uint32 m_uiIndex ;
        uint32 m_uiCheckTimer ;
        InstanceScript* instance ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_rolling_barrelAI(creature);
    }
};

#define STALKER_BOUNCER 500001

class mob_hozen_bouncer : public CreatureScript
{
public :
    mob_hozen_bouncer() : CreatureScript("mob_hozen_bouncer")
    {

    }

    class mob_hozen_bouncerAI : public ScriptedAI
    {
    public :
        mob_hozen_bouncerAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            DoCast(me, 107019, true);
        }

        void DoAction(const int32 action)
        {
            if(action == 1)
            {
                me->RemoveAurasDueToSpell(107019);
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_hozen_bouncerAI(creature);
    }
};

#define MAX_BOUNCERS_POSITIONS 2
const Position bouncersPositions[MAX_BOUNCERS_POSITIONS] =
{
    {-755.124268f, 1315.427979f, 146.711365f, 1.831958f},
    {-737.382507f, 1320.407959f, 146.711990f, 1.831958f}
};

#define MAX_SUMMON_DANCERS_POSITIONS 2
const Position summonDancers[MAX_SUMMON_DANCERS_POSITIONS] =
{
    {-709.934143f, 1305.966553f, 146.692627f, 2.746932f},
    {-770.244080f, 1291.118652f, 146.693665f, 1.178493f}
};

#define MAX_SUMMON_DANCERS 11
#define MAX_MOVE_DANCERS_POSITIONS 3
#define DANCER_X 5.09552f
#define DANCER_Y 1.065163f
const Position dancersPositions[MAX_MOVE_DANCERS_POSITIONS] =
{
    {-764.121338f, 1339.369751f, 146.756865f, 1.831958f},
    {-761.018616f, 1333.645264f, 146.725555f, 1.831958f},
    {-762.754883f, 1326.869995f, 146.719757f, 1.831958f}
};

class stalker_ook_ook_end_event : public CreatureScript
{
public :
    stalker_ook_ook_end_event() : CreatureScript("stalker_ook_ook_end_event")
    {

    }

    class stalker_ook_ook_end_eventAI : public ScriptedAI
    {
    public :
        stalker_ook_ook_end_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            firstBouncer = NULL ;
            secondBouncer = NULL ;

            m_uiTalkId = 0 ;
            m_uiTalkTimer = DIALOGUE_STOP_TIMER ;
        }

        void DoAction(const int32 action)
        {
            if(action == 0)
            {
                DoMoveBouncers();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            // Return since we are not yet ready
            if(m_uiTalkTimer == DIALOGUE_STOP_TIMER)
                return ;

            if(m_uiTalkTimer <= diff)
            {
                switch(m_uiTalkId)
                {
                case 0 :
                    if(firstBouncer)
                        firstBouncer->AI()->Talk(m_uiTalkId);
                    m_uiTalkTimer = 3000 ;
                    break ;

                case 1 :
                    if(secondBouncer)
                    {
                        secondBouncer->AI()->Talk(m_uiTalkId);
                        secondBouncer->SetReactState(REACT_PASSIVE);
                        secondBouncer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        secondBouncer->GetMotionMaster()->MovePoint(0, bouncersPositions[1]);
                    }
                    m_uiTalkTimer = 2000 ;
                    break ;

                case 2 :
                    if(firstBouncer)
                        firstBouncer->AI()->Talk(m_uiTalkId);
                    DoPrepareDance();
                    m_uiTalkTimer = 3000 ;
                    break ;

                case 3 :
                    if(secondBouncer)
                        secondBouncer->AI()->Talk(m_uiTalkId);
                    m_uiTalkTimer = 3000 ;
                    break ;

                case 4 :
                    DoDance();
                    break ;

                default :
                    break ;
                }
                ++m_uiTalkId;
            }
            else
                m_uiTalkTimer -= diff ;
        }

        void DoMoveBouncers()
        {
            CreatureList bouncers ;
            GetCreatureListWithEntryInGrid(bouncers, me, 56849, 5000.0f);

            CreatureListConstIter iter = bouncers.begin() ;
            firstBouncer = *iter ;
            std::advance(iter, 1);
            secondBouncer = *iter ;

            m_uiTalkTimer = 200 ;

            if(firstBouncer)
            {
                firstBouncer->GetMotionMaster()->MovePoint(0, bouncersPositions[0]);
                firstBouncer->SetReactState(REACT_PASSIVE);
                firstBouncer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }
        }

        void DoPrepareDance()
        {
            CreatureList hozens ;
            bool left = false ;
            for(uint8 i = 0 ; i < MAX_SUMMON_DANCERS ; ++i)
            {
                if(left)
                {
                    if(Creature* c = me->SummonCreature(59684, summonDancers[0]))
                    {
                        c->SetReactState(REACT_PASSIVE);
                        c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        hozens.push_back(c);
                    }
                    left = false ;
                }
                else
                {
                    if(Creature* c = me->SummonCreature(59684, summonDancers[1]))
                    {
                        c->SetReactState(REACT_PASSIVE);
                        c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        hozens.push_back(c);
                    }
                    left = true ;
                }
            }

            DoMoveHozens(hozens);
        }

        void DoMoveHozens(CreatureList const& hozens)
        {
            if(hozens.empty())
                return ;

            CreatureListConstIter iter = hozens.begin();

            Move(0, hozens, iter, 4);
            Move(1, hozens, iter, 3);
            Move(2, hozens, iter, 4);
        }

        void Move(const uint8 index, CreatureList const& source, CreatureListConstIter& iter, uint8 summonNumber)
        {
            float x = dancersPositions[index].GetPositionX() ;
            float y = dancersPositions[index].GetPositionY() ;
            float z = dancersPositions[index].GetPositionZ() + 1 ;

            for(uint8 i = 0 ; i < summonNumber ; ++i)
            {
                x += DANCER_X * i ;
                y += DANCER_Y * i ;
                std::advance(iter, 1);
                if(iter != source.end())
                    if(Creature* c = *iter)
                        c->GetMotionMaster()->MovePoint(0, x, y, z);
            }
        }

        void DoDance()
        {
            CreatureList dancers[2];
            GetCreatureListWithEntryInGrid(dancers[0], me, 56849, 50000.0f);
            GetCreatureListWithEntryInGrid(dancers[1], me, 59684, 50000.0f);

            for(uint8 i = 0 ; i < 2 ; ++i)
            {
                for(CreatureListConstIter iter = dancers[i].begin() ; iter != dancers[i].end() ; ++iter)
                {
                    if(Creature* c = *iter)
                    {
                        c->CastSpell(c, 128271, true);
                    }
                }
            }
        }

    private :
        Creature* firstBouncer ;
        Creature* secondBouncer ;

        uint32 m_uiTalkTimer ;
        uint32 m_uiTalkId ;
    };
};

class spell_sb_doorguard : public SpellScriptLoader
{
public :
    spell_sb_doorguard() : SpellScriptLoader("spell_sb_doorguard")
    {

    }

    class spell_sb_doorguard_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sb_doorguard_AuraScript);

        bool Validate(const SpellInfo *spellInfo)
        {
            return true ;
        }

        bool Load()
        {
            return true ;
        }


        /// Since the spell is casted event if no player in range, it would trigger a visual event if not needed ;
        /// So if there is at least one player that can be affected by the periodic, we return ;
        /// Else, we prevent the periodic, so it cancels visual (and by the way, there is no need to check during UpdateAI
        void HandlePeriodic(AuraEffect const* auraEff)
        {
            bool canPeriodic = false ;
            if(WorldObject* owner = GetOwner())
            {
                if(Creature* unit = owner->ToCreature())
                {
                    if(Map* map = unit->GetMap())
                    {
                        Map::PlayerList const& playerList = map->GetPlayers();
                        if(!playerList.isEmpty())
                        {
                            for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                            {
                                if(Player* player = iter->getSource())
                                {
                                    if(unit)
                                    {
                                        float dist = unit->GetExactDist2d(player->GetPositionX(), player->GetPositionY());
                                        if(dist <= 5.0f)
                                        {
                                            canPeriodic = true ;
                                            return ;
                                        }
                                        else
                                            continue ;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            PreventDefaultAction();
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_sb_doorguard_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_sb_doorguard_AuraScript();
    }
};

/**********************************************************/
/********************* HOPTALLUS **************************/
/**********************************************************/

#define MAX_TRASH_POINT_ID 6

const Position TrashPoints[MAX_TRASH_POINT_ID] =
{
    {-710.459900f, 1310.748657f, 162.627029f, 2.754009f}, // First move after summon
    {-748.471802f, 1318.885498f, 162.630890f, 4.244696f}, // Entre tonneaux
    {-769.136719f, 1278.357056f, 162.695511f, 5.070935f}, // Devant la sortie
    {-761.958252f, 1247.301147f, 162.754562f, 3.387041f}, // Avant la pente
    {-796.072205f, 1238.755615f, 154.774200f, 1.951332f}, // Milieu de la pente
    {-809.778992f, 1273.019653f, 146.665207f, 0.371111f} // En bas de la pente
};

class mob_hoptallus_trash : public CreatureScript
{
public :
    mob_hoptallus_trash() : CreatureScript("mob_hoptallus_trash")
    {

    }

    class mob_hoptallus_trashAI : public ScriptedAI
    {
    public :
        mob_hoptallus_trashAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {

        }

        void MovementInform(uint32 type, uint32 id)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY: Virmen entering MovementInform, using type %u and point %u", type, id);
            if(type == POINT_MOTION_TYPE)
            {
                Position pos ;
                switch(id)
                {
                case 0 :
                    pos = TrashPoints[1];
                    break ;
                    
                case 1 :
                    pos = TrashPoints[2];
                    break ;
                    
                case 2 :
                    pos = TrashPoints[3];
                    break ;
                    
                case 3 :
                    pos = TrashPoints[4];
                    break ;
                    
                case 4 :
                    pos = TrashPoints[5];
                    break ;
                    
                case 5 :
                    me->GetMotionMaster()->MovePoint(id + 1, me->GetPositionX() + rand() % 4, me->GetPositionY() + rand() % 4, me->GetPositionZ());
					return ;
                default :
                    return ;
                }
				
				float x = pos.GetPositionX();
				float y = pos.GetPositionY();
				float z = pos.GetPositionZ();
				
				sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY: Virmen MotionMaster ; next point id set to %u, with x = %f, y = %f, z= %f", (id + 1), x, y , z);
				me->GetMotionMaster()->MovePoint(id + 1, x, y, z);
            }
        }

        void JustDied(Unit *killer)
        {
            if(InstanceScript* instance = me->GetInstanceScript())
                instance->SetData64(INSTANCE_DATA64_KILLED_HOPTALLUS_TRASH, 1);
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return ;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_hoptallus_trashAI(creature);
    }
};

#define NPC_CARROT_STALKER 500002
#define MAX_CARROT_POSITIONS 8
const Position carrotPositions[MAX_CARROT_POSITIONS] =
{
    {-657.059753f, 1256.340332f, 154.814529f, 4.925640f}, // milieu de pente
    {-639.456424f, 1192.562988f, 139.153397f, 0.247026f}, // Avant le saut
    {-628.554871f, 1195.119751f, 139.153397f, 0.247026f}, // Fin de plate-forme
    {-606.720825f, 1212.027222f, 138.538086f, 5.125917f}, // Après le saut
    {-598.784302f, 1192.206055f, 138.510284f, 7.434788f}, // Avant le pont
    {-597.464111f, 1133.276611f, 138.510178f, 5.664701f}, // A côte de la charette
    {-540.785522f, 1088.266846f, 151.069290f, 6.217621f}, // Coline
    {-481.942719f, 1074.370605f, 156.120697f, 5.696114f} // Fin de parcours
};

class stalker_carrot_door : public CreatureScript
{
public :
    stalker_carrot_door() : CreatureScript("stalker_carrot_door")
    {

    }

    class stalker_carrot_doorAI : public ScriptedAI
    {
    public :
        stalker_carrot_doorAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            DoCast(me, 116005, true);
            m_bActive = false ;
            m_uiAfterJumpTimer = 2500 ;
        }

        void UpdateAI(const uint32 diff)
        {
            if(m_bActive)
            {
                if(m_uiAfterJumpTimer <= diff)
                {
                    me->GetMotionMaster()->MovePoint(4, carrotPositions[5]);
                    m_bActive = false ;
                }
                else
                    m_uiAfterJumpTimer -= diff ;
            }
        }

        void DoAction(const int32 action)
        {
            if(action == 0) // Aka, start
            {
                m_bActive = true ;
                me->GetMotionMaster()->MovePoint(0, carrotPositions[0]);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Stormstout Brewery: Carrot door: Entering MovementInform (type %u, id %u)", type, id);
            if(type == POINT_MOTION_TYPE)
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Stormstout Brewery: Carrot door: MovementInform: switching");
                switch(id)
                {
                case 0 :
                    me->GetMotionMaster()->MovePoint(1, carrotPositions[1]);
                    break ;

                case 1 :
                    me->GetMotionMaster()->MovePoint(2, carrotPositions[2]);
                    break ;

                case 2 :
                    me->GetMotionMaster()->MovePoint(3, carrotPositions[3]);
                    break;

                case 3 :
                    me->GetMotionMaster()->MoveJump(carrotPositions[4], 1.0f, 1.0f);
                    break;

                case 4 :
                    me->GetMotionMaster()->MovePoint(5, carrotPositions[6]);
                    break ;

                case 5 :
                    me->GetMotionMaster()->MovePoint(6, carrotPositions[7]);
                    break ;

                case 6 :
                    sLog->outDebug(LOG_FILTER_NETWORKIO, "Carrot Path Ended");
                    break ;

                default :
                    break ;
                }
            }
        }

    private :
        uint32 m_uiAfterJumpTimer ;
        bool m_bActive ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new stalker_carrot_doorAI(creature);
    }
};

/*******************************************************/
/********************* YANZHU **************************/
/*******************************************************/

#define STALKER_TARGET 500000

class stalker_gushing_brew : public CreatureScript
{
public :
    stalker_gushing_brew() : CreatureScript("stalker_gushing_brew")
    {

    }

    class stalker_gushing_brewAI : public ScriptedAI
    {
    public :
        stalker_gushing_brewAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            m_uiCheckTimer = 500;
            if(Creature* target = me->FindNearestCreature(STALKER_TARGET, 500.0f, true))
            {
                DoCast(target, 114379, true);
                _target = target ;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(m_uiCheckTimer <= diff)
            {
                if(_target && _target->IsInWorld())
                    DoCast(_target, 114380);
                DoCheckForPlayers();
                m_uiCheckTimer = 500 ;
            }
        }

        void DoCheckForPlayers()
        {
            if(Map* map = me->GetMap())
            {
                Map::PlayerList const& playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                {
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
                        if(Player* p = iter->getSource())
                        {
                            if(_target)
                            {
                                if(p->IsInBetween(me, _target))
                                {
                                    DoCast(p, 114381, true);
                                }
                            }
                        }
                    }
                }
            }
        }

    private :
        uint32 m_uiCheckTimer ;
        Unit* _target ;
    };
};

class stalker_gushing_brew_target : public CreatureScript
{
public :
    stalker_gushing_brew_target() : CreatureScript("stalker_gushing_brew_target")
    {

    }

    class stalker_gushing_brew_targetAI : public ScriptedAI
    {
    public :
        stalker_gushing_brew_targetAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            m_uiMoveTimer = 5000 ;
            me->GetMotionMaster()->MoveRandom(1.0f);
        }

        void UpdateAI(const uint32 diff)
        {
            if(m_uiMoveTimer <= diff)
            {
                me->GetMotionMaster()->MoveRandom(1.0f);
                m_uiMoveTimer = 5000 ;
            }
            else
                m_uiMoveTimer -= diff ;
        }

    private :
        uint32 m_uiMoveTimer ;
    };
};

#define MAX_SUMMON_GAO_WAVE 8
#define PROGRESS_X 2.038879f
#define PROGRESS_Y 8.39978f

#define PROGRESS_LINE_X 8.077647f
#define PROGRESS_LINE_Y -2.111451f
#define PROGRESS_Z 5.0f

class npc_uncle_gao : public CreatureScript
{
public :
    npc_uncle_gao() : CreatureScript("npc_uncle_gao")
    {

    }

    class npc_uncle_gaoAI : public ScriptedAI
    {
    public :
        npc_uncle_gaoAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            m_uiCheckTimer = 0 ;
            m_uiWaveId = 1 ;
            m_uiSummoned = 0 ;
            m_bStarted = false ;
            m_uiWaitTimer = 1000 ;

            m_uiFirstAlemental = RAND(59519, 59518);
            m_uiSecondAlemental = RAND(59521, 66413);
            m_uiThirdAlemental = RAND(59520, 59522);
        }

        void JustSummoned(Creature *creature)
        {
            ++m_uiSummoned;
        }

        void SummonedCreatureDies(Creature *creature, Unit *killer)
        {
            --m_uiSummoned;
            if(m_uiSummoned == 0)
            {
                Talk(m_uiWaveId + 1);
                m_uiWaitTimer = 7000 ;
                StartNextWave();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(instance)
            {
                if(instance->GetData(INSTANCE_DATA_HOPTALLUS_STATUS) == DONE && !m_bStarted)
                {
                    if(m_uiCheckTimer <= diff)
                    {
                        if(!DoCheckForPlayer())
                            m_uiCheckTimer = 1000 ;
                        else
                            Start();
                    }
                    else
                        m_uiCheckTimer -= diff ;
                    return ;
                }

                if(m_uiWaitTimer > diff)
                {
                    if(m_uiSummoned == 0)
                        m_uiWaitTimer -= diff ;
                }
                else
                    StartNextWave();
            }
        }

        bool DoCheckForPlayer()
        {
            if(Map* map = me->GetMap())
            {
                Map::PlayerList const & playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                {
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
                        if(Player* p = iter->getSource())
                        {
                            Position pos ;
                            p->GetPosition(&pos);
                            if(me->GetExactDist2d(&pos) <= 10.0f)
                                return true ;
                        }
                    }
                }
                else
                    return false ;
            }

            return false ;
        }

        void Start()
        {
            m_bStarted = true ;
            Talk(1); // Introduction
            StartNextWave();
        }

        void StartNextWave()
        {
            switch(m_uiWaveId)
            {
            case 1 :
                //Talk(2);
                SummonFirstWave();
                break ;

            case 2 :
                //Talk(3);
                SummonSecondWave();
                break ;

            case 3 :
                //Talk(4);
                SummonThirdWave();
                break ;

            case 4 :
                //Talk(5);
                SummonYanZhu();
                break ;

            case 5:
            {
                Talk(6);
                Position const movePoint = {-703.415771f, 1162.801025f, 166.141693f, 0.273505f};
                me->GetMotionMaster()->MovePoint(0, movePoint);
                break ;
            }

            default :
                break ;
            }

            ++m_uiWaveId;
        }

        void SummonFirstWave()
        {
            Position start = {-704.517517f, 1166.889648f, 166.141571f, 0.275305f};
            for(uint8 i = 0 ; i < 2 ; ++i)
            {
                if(Creature* summon = me->SummonCreature(m_uiFirstAlemental, start.GetPositionX() + PROGRESS_X * i, start.GetPositionY() + PROGRESS_Y * i, start.GetPositionZ()))
                    summon->SetOrientation(0.0f);
            }
        }

        void SummonSecondWave()
        {
            Position start = {-713.708191f, 1168.700195f, 166.145874f, 0.275305f};

            for(uint8 i = 0 ; i < 3 ; ++i)
            {
                float x = start.GetPositionX() + PROGRESS_LINE_X * i ;
                float y = start.GetPositionY() + PROGRESS_LINE_Y * i ;
                for(uint8 j = 0 ; j < 3 ; ++j)
                {
                    if(Creature* summon = me->SummonCreature(m_uiSecondAlemental, x + PROGRESS_X * j, y + PROGRESS_Y * j, start.GetPositionZ()))
                        summon->SetOrientation(0.0f);
                }
            }
        }

        void SummonThirdWave()
        {
            Position start = {-713.708191f, 1168.700195f, 166.145874f, 0.275305f};

            for(uint8 i = 0 ; i < 2 ; ++i)
            {
                float x = start.GetPositionX() + 2 * PROGRESS_LINE_X * i ;
                float y = start.GetPositionY() + 2 * PROGRESS_LINE_Y * i ;
                for(uint8 j = 0 ; j < 2 ; ++j)
                {
                    if(Creature* summon = me->SummonCreature(m_uiThirdAlemental, x + 2 * PROGRESS_X * j, y + 2 * PROGRESS_Y * j, start.GetPositionZ()))
                        summon->SetOrientation(0.0f);
                }
            }
        }

        void SummonYanZhu()
        {
            Position const summon = {-703.415771f, 1162.801025f, 166.141693f, 0.273505f};
            if(Creature* yanZhu = me->SummonCreature(BOSS_YAN_ZHU, summon, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 24 * HOUR, 0))
            {
                yanZhu->AI()->DoAction(0);
                yanZhu->AI()->SetData(BOSS_YAN_ZHU_DATA_WHEAT_SPELL, m_uiFirstAlemental);
                yanZhu->AI()->SetData(BOSS_YAN_ZHU_DATA_ALE_SPELL, m_uiSecondAlemental);
                yanZhu->AI()->SetData(BOSS_YAN_ZHU_DATA_STOUT_SPELL, m_uiThirdAlemental);
            }
        }

    private :
        InstanceScript* instance ;
        uint32 m_uiSummoned ;
        uint32 m_uiWaveId ;
        uint32 m_uiCheckTimer ;
        uint32 m_uiWaitTimer ;
        bool m_bStarted ;

        uint32 m_uiFirstAlemental ;
        uint32 m_uiSecondAlemental ;
        uint32 m_uiThirdAlemental ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_uncle_gaoAI(creature);
    }
};

void AddSC_stormstout_brewery()
{
    // Ook Ook
    new npc_chen_stormstout();
    new npc_auntie_stormstout();
    //new npc_sb_illusioner();
    new mob_habanero_brew();
    new mob_ook_ook_hozen();
    new npc_rolling_barrel();
    new mob_hozen_bouncer();
    new stalker_ook_ook_end_event();
    new spell_sb_doorguard();

    // Hoptallus
    new mob_hoptallus_trash();
    new stalker_carrot_door();

    // YanZhu
    new stalker_gushing_brew();
    new stalker_gushing_brew_target();
    new npc_uncle_gao();
}
