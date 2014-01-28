#include "stormstout_brewery.h"

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
            instance = creature->GetInstanceScript();
            m_bHasStartedOnce = false ;
            m_bReady = false ;
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
            if(!m_bReady && !UpdateVictim() && m_bHasStartedOnce)
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
            }

            if(m_bReady && !UpdateVictim())
                return ;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return ;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_FURLWIND :
                    Talk(TALK_FURLWIND);
                    DoCastAOE(SPELL_FURLWIND);
                    events.ScheduleEvent(EVENT_FURLWIND, IsHeroic() ? urand(6000, 7000) : urand(7000, 8500));
                    break ;

                case EVENT_CARROT_BREATH :
                    DoCast(SPELL_CARROT_BREATH);
                    events.ScheduleEvent(EVENT_CARROT_BREATH, IsHeroic() ? 25000 : 35000);
                    break ;

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
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Entered SummonVirmens");
            for(uint8 i = 0 ; i < 5 ; ++i)
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "Looping");
				uint32 index = urand(0, MAX_SUMMON_VIRMEN - 1);
				const Position posSummon = summonVirmenPosition[index];
				index = urand(0, MAX_SUMMON_VIRMEN - 1);
				const Position posJump = jumpVirmenPosition[index];
				uint32 entry = RAND(MOB_HOPPER, MOB_HOPPLING, MOB_BOPPER);
                if(Creature* summon = me->SummonCreature(entry, posSummon))
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "Summoned virmen ; motion master");
                    summon->GetMotionMaster()->MoveJump(posJump, 8.0f, 8.0f);
					sLog->outDebug(LOG_FILTER_NETWORKIO, "Jumped");
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
        stalker_carrot_breathAI(Creature* creature) : ScriptedAI(creature)
        {
			instance = creature->GetInstanceScript();
        }
		
		void Reset()
		{
			if(Unit* owner = me->ToTempSummon()->GetSummoner())
				{
					sLog->outDebug(LOG_FILTER_NETWORKIO, "owner not null ; guid = %u ; hoptallus guid = %u", owner->GetGUID(), instance ? instance->GetData64(INSTANCE_DATA64_HOPTALLUS_GUID) : 0);
					center.Relocate(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ());
					m_rayon = me->GetExactDist2d(center.GetPositionX(), center.GetPositionY());
					
					sLog->outDebug(LOG_FILTER_NETWORKIO, "rayon calculated %f", m_rayon);
					
					me->SetFacingToObject(owner);
					me->SetTarget(me->GetOwnerGUID());
					angle = me->GetOrientation();
					m_id = 0 ;
					
					me->SetSpeed(MOVE_RUN, 2 * M_PI * m_rayon / 500, true);
					me->SetSpeed(MOVE_FLIGHT, 2 * M_PI * m_rayon / 500, true);
				}
		}
		
		void MovementInform(uint32 type, uint32 id)
		{
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Carrot Breath Helper : Entering MovementInform using type %u, id %u", type, id);
		}
		
		void DoAction(const int32 action)
		{
			if(action == 0)
			{
				sLog->outDebug(LOG_FILTER_NETWORKIO, "Carrot Breath Helper : DO ACTION");
				DoUpdatePosition(450);
			}
		}
		
		void DoUpdatePosition(const uint32 diff)
		{
			++m_id ;
            angle -= (2 * M_PI / 500)*diff ;
			
            x = center.GetPositionX() + cos(angle) * m_rayon ;
            y = center.GetPositionY() + sin(angle) * m_rayon ;
            z = center.GetPositionZ() ;
			
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Carrot Breath Helper : actual coords : x = %f, y = %f, z= %f", me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Carrot Breath Helper : coords computed, x = %f, y = %f, z = %f", x, y, z);
			sLog->outDebug(LOG_FILTER_NETWORKIO, "Carrot Breath Helper : MOTION MASTER");
			
            me->GetMotionMaster()->MovePoint(m_id, x, y, z);
			
		}

    private :
		InstanceScript* instance ;
        Position center ;
        float m_rayon ;
        float x, y, z ;
        float angle ;
        int m_id ;
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

                Position posSummon ;
                float x = 35 * cos(caster->GetOrientation());
                float y = 35 * sin(caster->GetOrientation());
                posSummon.Relocate(caster->GetPositionX() + x, caster->GetPositionY() + y, caster->GetPositionZ());

                if(TempSummon* summoned = caster->SummonCreature(NPC_CARROT_BREATH_HELPER, posSummon, TEMPSUMMON_TIMED_DESPAWN, 15100))
                {
					summon = summoned ;
					sLog->outDebug(LOG_FILTER_NETWORKIO, "Summon Guid Is %u", summoned->GetGUID());
                    caster->SetTarget(summoned->GetGUID());
                    caster->SetFacingToObject(summoned);
                }
            }
        }
		
		void HandlePeriodicTick(AuraEffect const* auraEff)
        {
            if(summon)
            {
                summon->AI()->DoAction(0);
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
			OnEffectPeriodic += AuraEffectPeriodicFn(spell_hoptallus_carrot_breath_AuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }

    private :
        Unit* target ;
		TempSummon* summon ;
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
    new npc_big_ol_hammer();
    new stalker_carrot_breath();
    new spell_hoptallus_carrot_breath();
}
