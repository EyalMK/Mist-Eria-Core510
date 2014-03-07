#include "stormstout_brewery.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "PointMovementGenerator.h"

// #define RETURN_IF(condition) if(!condition) return ;

enum HoptallusSpells
{
    SPELL_FURLWIND      = 112992,
    SPELL_CARROT_BREATH = 112944
};

enum Events
{
    EVENT_SUMMON_VIRMEN = 1,
    EVENT_FURLWIND      = 2,
    EVENT_CARROT_BREATH = 3,
    EVENT_CHECK         = 4
};

enum Talk
{
    TALK_AGGRO          = 1,
    TALK_KILLED_PLAYER  = 2,
    TALK_FURLWIND       = 3,
    TALK_CARROT_BREATH  = 4,
    TALK_JUST_DIED      = 5
};

enum Misc
{
    GOB_GIANT_BARREL            = 211138,
    NPC_CARROT_COLLECTOR        = 200505,
    NPC_CARROT_BREATH_HELPER    = 200504,
    MOB_BOPPER                  = 59551,
    MOB_HOPPER                  = 59464,
    MOB_HOPPLING                = 60208,
    NPC_BIG_OL_HAMMER           = 59539
};

#define MAX_SUMMON_VIRMEN 5
const Position summonVirmenPosition[MAX_SUMMON_VIRMEN] =
{
    {-728.146240f, 1249.065796f, 164.800262f, 0.291780f},
    {-724.845337f, 1254.960938f, 164.800262f, 0.291780f},
    {-721.216980f, 1249.473389f, 164.800262f, 0.291780f},
    {-728.308960f, 1260.730957f, 164.800262f, 0.291780f},
    {-718.083557f, 1243.474976f, 164.800262f, 0.291780f}
	/*{-686.667542f, 1241.537993f, 162.795578f, 1.864492f},
	{-682.696228f, 1244.395874f, 162.795578f, 1.864492f},
	{-700.184814f, 1239.763672f, 162.795578f, 1.475719f},
	{-707.646057f, 1272.627075f, 162.772934f, 5.390928f},
	{-693.530945f, 1276.592407f, 162.793060f, 4.653439f}*/
};

const Position jumpVirmenPosition[MAX_SUMMON_VIRMEN] =
{
    {-702.496643f, 1248.231689f, 162.794922f, 1.271960f},
    {-706.126038f, 1266.666748f, 162.779785f, 0.039670f},
    {-699.487671f, 1275.684326f, 162.770660f, 4.822747f},
    {-690.849060f, 1244.437622f, 162.794586f, 1.847660f},
    {-696.724609f, 1256.291992f, 162.785858f, 6.239606f}
};

const Position jumpPosition = {-695.928467f, 1261.399414f, 162.780762f, 0.291780f};

class boss_hoptallus : public CreatureScript
{
public :
    boss_hoptallus() : CreatureScript("boss_hoptallus")
    {

    }

    class boss_hoptallusAI : public ScriptedAI
    {
    public :
        boss_hoptallusAI(Creature* creature) : ScriptedAI(creature)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Hoptallus : AI Constructed");
            instance = creature->GetInstanceScript();
            m_bHasStartedOnce = true ;
            m_bReady = true ;
        }

        void Reset()
        {
            if(instance)
                instance->SetData(INSTANCE_DATA_HOPTALLUS_STATUS, NOT_STARTED);
            events.Reset();
            if(m_bHasStartedOnce)
                events.ScheduleEvent(EVENT_CHECK, 500);
        }

        void DoAction(const int32 action)
        {
            if(action == 0)
            {
                if(GameObject* go = me->FindNearestGameObject(GOB_GIANT_BARREL, 50000.0f))
                    go->SetGoState(GO_STATE_ACTIVE);
                me->GetMotionMaster()->MoveJump(jumpPosition, 1.0f, 1.0f);
                m_bReady = true ;
            }
        }

        void EnterCombat(Unit *aggro)
        {
            if(!m_bHasStartedOnce)
                m_bHasStartedOnce = true ;

            if(instance)
            {
                instance->SetData(INSTANCE_DATA_HOPTALLUS_STATUS, IN_PROGRESS);
                if(GameObject* door = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_HOPTALLUS_ENTRANCE_GUID)))
                    door->SetGoState(GO_STATE_READY);
            }
            events.ScheduleEvent(EVENT_SUMMON_VIRMEN, IsHeroic() ? urand(10000, 11000) : urand(12000, 14000));
            events.ScheduleEvent(EVENT_FURLWIND, IsHeroic() ? 6000 : 8000);
            events.ScheduleEvent(EVENT_CARROT_BREATH, IsHeroic() ? 25000 : 35000);

            Talk(TALK_AGGRO);
        }

        void EnterEvadeMode()
        {
            if(instance)
            {
                instance->SetData(INSTANCE_DATA_HOPTALLUS_STATUS, FAIL);
                if(GameObject* door = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_HOPTALLUS_ENTRANCE_GUID)))
                    door->SetGoState(GO_STATE_ACTIVE);
            }

            if(!m_bReady)
                ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit *killer)
        {
            if(instance)
            {
                instance->SetData(INSTANCE_DATA_HOPTALLUS_STATUS, DONE);
                if(GameObject* entrance = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_HOPTALLUS_ENTRANCE_GUID)))
                    entrance->SetGoState(GO_STATE_ACTIVE);
                if(GameObject* exit = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_HOPTALLUS_EXIT_GUID)))
                    exit->SetGoState(GO_STATE_ACTIVE);
            }

            if(Creature* c = me->SummonCreature(NPC_CARROT_COLLECTOR, -702.496643f, 1248.231689f, 162.794922f, 1.271960f))
            {
                if(Creature* carrot = me->FindNearestCreature(500002, 50000.0f, true))
                    c->GetMotionMaster()->MoveFollow(carrot, 0.5f, carrot->GetOrientation() - M_PI / 4);
            }

            Talk(TALK_JUST_DIED);
        }

        void KilledUnit(Unit *killed)
        {
            Talk(TALK_KILLED_PLAYER);
            if(!UpdateVictim())
                m_bReady = false ;
        }

        void UpdateAI(const uint32 diff)
        {
			// sLog->outDebug(LOG_FILTER_NETWORKIO, "Hoptallus : Update AI using diff %u", diff);
            if(!m_bReady && m_bHasStartedOnce)
            {
                events.Update(diff);

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                    case EVENT_CHECK :
                        if(DoCheckForPlayers())
                        {
                            me->GetMotionMaster()->MoveJump(jumpPosition, 1, 1);
                            events.Reset();
                            m_bReady = true ;
                            break;
                        }
                        else
                        {
                            events.ScheduleEvent(EVENT_CHECK, 500);
                            return ;
                        }
                        break ;

                    default :
                        break ;
                    }
                }

                return ;
            }

            if(m_bReady && !UpdateVictim())
                return ;

            events.Update(diff);

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_SUMMON_VIRMEN :
                    if(m_uiSummonTimes == 0)
                        if(instance)
                            instance->DoSendNotifyToInstance("Hoptallus emits a shrill cry : more virmens are coming !");
                    SummonVirmens();
                    ++m_uiSummonTimes;
                    if(m_uiSummonTimes < 3)
                        events.ScheduleEvent(EVENT_SUMMON_VIRMEN, 1000);
                    else
                    {
                        events.ScheduleEvent(EVENT_SUMMON_VIRMEN, IsHeroic() ? 10000 : 15000);
                        m_uiSummonTimes = 0 ;
                    }
                    break ;

                case EVENT_FURLWIND :
                    if(me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        events.ScheduleEvent(EVENT_FURLWIND, 100);
                        return ;
                    }
                    Talk(TALK_FURLWIND);
                    DoCastAOE(SPELL_FURLWIND);
                    events.ScheduleEvent(EVENT_FURLWIND, IsHeroic() ? urand(12000, 14000) : urand(14000, 17000));
                    break ;

                case EVENT_CARROT_BREATH :
                    DoCast(SPELL_CARROT_BREATH);
                    events.ScheduleEvent(EVENT_CARROT_BREATH, IsHeroic() ? 25000 : 35000);
                    break ;


                default :
                    break ;
                }
            }
			DoMeleeAttackIfReady();
        }

        bool DoCheckForPlayers()
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
                            if(p->isDead())
                                return false ;
                            else
                                continue ;
                        }
                    }
                }
                else
                    return false ;
            }
            else
                return false ;

            return true ;
        }

        void SummonVirmens()
        {
            // sLog->outDebug(LOG_FILTER_NETWORKIO, "Entered SummonVirmens");
            for(uint8 i = 0 ; i < 5 ; ++i)
            {
                // sLog->outDebug(LOG_FILTER_NETWORKIO, "Looping");
				uint32 index = urand(0, MAX_SUMMON_VIRMEN - 1);
				const Position posSummon = summonVirmenPosition[index];
				index = urand(0, MAX_SUMMON_VIRMEN - 1);
				const Position posJump = jumpVirmenPosition[index];
				uint32 entry = RAND(MOB_HOPPER, MOB_HOPPLING, MOB_BOPPER);
                if(Creature* summon = me->SummonCreature(entry, posSummon))
				{
                    // sLog->outDebug(LOG_FILTER_NETWORKIO, "Summoned virmen ; motion master");
                    summon->GetMotionMaster()->MoveJump(posJump, 8.0f, 8.0f);
                    // sLog->outDebug(LOG_FILTER_NETWORKIO, "Jumped");
				}
            }
        }

    private :
        EventMap events ;
        InstanceScript * instance ;
        bool m_bHasStartedOnce ;
        bool m_bReady ;
        uint8 m_uiSummonTimes ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
		sLog->outDebug(LOG_FILTER_NETWORKIO, "Hoptallus : GetAI");
        return new boss_hoptallusAI(creature);
    }
};

class mob_virmen : public CreatureScript
{
public :
    mob_virmen() : CreatureScript("mob_virmen")
    {

    }

    class mob_virmenAI : public ScriptedAI
    {
    public :
        mob_virmenAI(Creature* creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            m_uiCheckTimer = 500 ;
        }

        void JustDied(Unit *killer)
        {
            if(me->GetEntry() == MOB_BOPPER)
            {
                Position pos ;
                me->GetPosition(&pos);

                me->SummonCreature(NPC_BIG_OL_HAMMER, pos, TEMPSUMMON_TIMED_DESPAWN, 20000);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(me->GetEntry() == MOB_HOPPER)
            {
                if(m_uiCheckTimer <= diff)
                {
                    if(DoCheckPlayers())
                        me->Kill(me);
                    else
                        m_uiCheckTimer = 500 ;
                }
                else
                    m_uiCheckTimer -= diff ;
            }
        }

        bool DoCheckPlayers()
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
                            float dist = me->GetExactDist2d(p->GetPositionX(), p->GetPositionY());
                            if(dist <= 1.0f)
                            {
                                DoCast(p, 114291);
                                return true ;
                            }
                        }
                    }
                }
            }

            return false ;
        }

    private :
        uint32 m_uiCheckTimer ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_virmenAI(creature);
    }
};

class npc_big_ol_hammer : public CreatureScript
{
public :
    npc_big_ol_hammer() : CreatureScript("npc_big_ol_hammer")
    {

    }

    bool OnGossipHello(Player *p, Creature *me)
    {
        if(!p->HasAura(111662))
        {
            me->CastSpell(p, 116662, true);
            me->Kill(me);
            return false ;
        }

        return false ;
    }
};

class stalker_carrot_breath : public CreatureScript
{
public :
    stalker_carrot_breath() : CreatureScript("stalker_carrot_breath")
    {

    }

    class stalker_carrot_breathAI : public ScriptedAI
    {
    public :
        /// Constructor
        stalker_carrot_breathAI(Creature* creature) : ScriptedAI(creature)
        {
			
            b_init = false ;
        }

        /**
         * @brief IsSummonedBy
         * Overrides ScriptedAI::IsSummonedBy
         * Replace constructor
         *
         * @param summoner
         * Pointer to the summoner
         */
        void IsSummonedBy(Unit *summoner)
        {
            if(!summoner)
            {
                sLog->outFatal(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY : No summoner available for Carrot Breath Helper !");
                return ;
            }

            if(!summoner->ToCreature())
            {
                sLog->outFatal(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY : Summoner is not a creature for CBH !");
                return ;
            }

            // Quelques init
            m_uiMSTimeDiff = m_uiId = 0 ;
            x = y = z = 0.0f ;

            p_master = summoner->ToCreature();
            p_instance = summoner->GetInstanceScript();

            center.Relocate(summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());
            rayon = me->GetExactDist2d(&center);
            angle = me->GetOrientation();

            m_uiFirstMoveTimer = MIN_MAP_UPDATE_DELAY ; // Ca devrait faire l'affaire

            // Vitesse
            SpellInfo const* carrotBreathSI = sSpellMgr->GetSpellInfo(SPELL_CARROT_BREATH);
            if(!carrotBreathSI)
                m_uiTimeToDoATurn = 15000 ; // En 5.1.0a
            else
                m_uiTimeToDoATurn = carrotBreathSI->DurationEntry->Duration[0];

            /// Remember only one thing :
            /// CHANGE THE FUCKING LIMIT IN MOVE SPLINE ! (DONE)
            me->SetSpeed(MOVE_RUN, 2 * M_PI * rayon / m_uiTimeToDoATurn, true);
            me->SetSpeed(MOVE_FLIGHT, 2 * M_PI * rayon / m_uiTimeToDoATurn, true);

            // Log pour voir
            sLog->outDebug(LOG_FILTER_NETWORKIO, "STOMRSTOUT BREWERY : CBH run speed set to %f, flight speed set to %f",
                           me->GetSpeedRate(MOVE_RUN), me->GetSpeedRate(MOVE_FLIGHT));
        }

        void MovementInform(uint32 motionType, uint32 id)
        {
            /// This needs some documentation ; MovementInform is a function very special, called in many places
            /// but here we will focus only on a special case, when PointMovementGenerator::DoFinalize is called.
            /// When PMG::DoFinalize is called, there is a check done on the movespline of the owner of the movement
            /// generator. If the spline is finalized, MovementInform is called.
            ///
            /// DoFinalize is a subsequent call from MovementGeneratorMedium::Finalize, which is a subsequent
            /// call from MotionMaster::DirectDelete. This is a bit complicated isn't it ? DirectDelete is called
            /// in many places, but if we deal with each of them correctly, it is easy to understand.
            ///
            /// When the world ticks, the motion master is updated ; if the movement generator is finalized,
            /// *MovementGenerator::Update returns false, so there is a MovementExpired, with DirectExpire
            /// and DirectDelete. This is one of the calls.
            ///
            /// DirectDelete is also called in DirectClean, which is only called during MotionMaster::Clear. This
            /// call is not usefull right here.
            ///
            /// In UpdateMotion, the DirectDelete is called during the clear of the ExpiredList, which we don't care
            /// right here, because we only proceed with direct methods, so the core doesn't wait for a tick of the
            /// world to clean the generators.
            ///
            /// Finally, DirectDelete is called during Mutate, if some condition are validated. But since, the MMCF_UPDATE
            /// flag is almost NEVER used, the condition is almost everytime validated. So each time we call Mutate,
            /// using MotionMaster::MovePoint, we clean the movement already cleaned. This is perfect.

            if(motionType != POINT_MOTION_TYPE)
            {
                sLog->outFatal(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY : MovementInform called on CBH, but motion type is not POINT_MOTION_TYPE, it is %u", motionType);
                return ;
            }

            // Violent mais bon
            ASSERT(me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE);

            // Log avant de tout calculer (au cas où)


            /// Calcul à partir de la somme de time diff

            // On check le spline (useless mais sait-on jamais)
            if(!me->movespline->Finalized())
                me->movespline->_Finalize();

            //! On va considérer que le MotionMaster n'a pas pu fail...

            ++m_uiId ;
            // m_uiMSTimeDiff is used only to prevent too many calls to the motion master
            angle -= ((2 * M_PI / m_uiTimeToDoATurn) * m_uiMSTimeDiff) ;
            x = center.GetPositionX() + cos(angle) * rayon ;
            y = center.GetPositionY() + sin(angle) * rayon ;
            z = center.GetPositionZ() ;

            sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY : CBH supposed to go to %f, %f, %f, with id %u", x, y, z, m_uiId);

            // On envoie
            me->GetMotionMaster()->MovePoint(m_uiId, x, y, z, true);
            /*PointMovementGenerator* mGen = static_cast<PointMovementGenerator*>(me->GetMotionMaster()->top());
            float mx, my, mz ;
            mGen->GetDestination(mx, my, mz);
            sLog->outDebug(LOG_FILTER_NETWORKIO, "Datas computed : x %f, y %f, z %f", mx, my, mz);*/

            // Reset des timers
            m_uiMSTimeDiff = 0 ;
        }

        /**
         * @brief UpdateAI
         * Overload ScriptedAI::UpdateAI
         * Only used to compute m_uiMSTimeDiff correctly
         *
         * @param diff
         * Time between current and previous tick of the world
         */
        void UpdateAI(const uint32 diff)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY : UpdateAI (stalker CB)");
            // RETURN_IF(!b_init);

            m_uiMSTimeDiff += diff ;

            // First movement, nothing will crash because UpdateAI is called only after IsSummonedBy
            // As a matter of facts : Map::SummonCreature, Object::AddToMap, TS::AddToWorld, Creature::AddToWorld, Creature::AIM_Initialize
            if(m_uiFirstMoveTimer <= diff)
            {
                angle -= ((2 * M_PI / m_uiTimeToDoATurn) * diff) ;
                x = center.GetPositionX() + cos(angle) * rayon ;
                y = center.GetPositionY() + sin(angle) * rayon ;
                z = center.GetPositionZ() ;

                me->GetMotionMaster()->MovePoint(m_uiId, x, y, z);
            }
        }

    private :
        /// Used in MovementInform
        uint32 m_uiMSTimeDiff ;

        /// Used in MotionMaster, id of the point
        uint32 m_uiId ;

        /// Coordinates
        float x, y, z ;

        /// Pointer to the master
        Creature* p_master ;

        /// Pointer to InstanceScript in case
        InstanceScript* p_instance ;

        /// Distance from the caster
        float rayon ;

        /// Angle
        float angle ;

        /// Position of the master
        Position center ;

        /// Duration of a turn
        uint32 m_uiTimeToDoATurn ;

        /// Timer before first movement
        uint32 m_uiFirstMoveTimer ;

        /// Prevents calling UpdateAI if AI IsSummondeBy has not yet been called
        bool b_init ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new stalker_carrot_breathAI(creature);
    }
};

class spell_hoptallus_carrot_breath : public SpellScriptLoader
{
public :
    spell_hoptallus_carrot_breath() : SpellScriptLoader("spell_hoptallus_carrot_breath")
    {

    }

    class spell_hoptallus_carrot_breath_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hoptallus_carrot_breath_AuraScript);

        bool Validate(const SpellInfo *spellInfo)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS : Carrot Breath : VALIDATE");
            return true ;
        }

        bool Load()
        {
            return true ;
        }

        void HandleEffectApply(AuraEffect const* auraEf, AuraEffectHandleModes mode)
        {
            if(Unit* caster = GetCaster())
            {
                target = caster->getVictim();
                // See if I care to use a spell !
                if(TempSummon* stalker = caster->SummonCreature(NPC_CARROT_BREATH_HELPER,
                                                                     0.5f * cos(caster->GetOrientation()),
                                                                     0.5f * sin(caster->GetOrientation()),
                                                                     caster->GetPositionZ(),
                                                                     0, TEMPSUMMON_TIMED_DESPAWN, 15000))
                {
					sLog->outDebug(LOG_FILTER_NETWORKIO, "SPELLS : Carrot Breath : Summoned");
                    caster->SetTarget(stalker->GetGUID()); //! Core guid !
                    caster->SetFacingToObject(stalker);
                    stalker->SetTarget(caster->GetGUID()); //! Core guid !
                    stalker->SetFacingToObject(caster);
                }
            }
        }

        void HandleEffectRemove(AuraEffect const* auraEf, AuraEffectHandleModes mode)
        {
            if(target)
            {
                if(GetCaster())
                {
                    GetCaster()->SetTarget(target->GetGUID());
                    GetCaster()->SetFacingToObject(target);
                }
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_hoptallus_carrot_breath_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_hoptallus_carrot_breath_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }

    private :
        Unit* target ;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_hoptallus_carrot_breath_AuraScript();
    }
};

void AddSC_boss_hoptallus()
{
    new boss_hoptallus();
    new mob_virmen();
    new stalker_carrot_breath();
    new npc_big_ol_hammer();
    new spell_hoptallus_carrot_breath();
}
