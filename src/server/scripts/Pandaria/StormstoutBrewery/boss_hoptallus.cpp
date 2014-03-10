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
    EVENT_RESET_SPEED   = 4,
	EVENT_RESET_STALKER	= 5
};

enum Talk
{
    TALK_AGGRO          = 1,
    TALK_KILLED_PLAYER  = 2,
    TALK_FURLWIND       = 3,
    TALK_CARROT_BREATH  = 4,
    TALK_JUST_DIED      = 5,
    TALK_SUMMON_VIRMEN  = 6
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
            stalker = NULL ;
        }

        void Reset()
        {
            if(instance)
                instance->SetData(INSTANCE_DATA_HOPTALLUS_STATUS, NOT_STARTED);
            events.Reset();
        }

        void DoAction(const int32 action)
        {
            if(action == 0)
            {
                if(GameObject* go = me->FindNearestGameObject(GOB_GIANT_BARREL, 50000.0f))
                    go->SetGoState(GO_STATE_ACTIVE);
                me->GetMotionMaster()->MoveJump(jumpPosition, 10.0f, 10.0f);
                me->SetHomePosition(jumpPosition); // So it will not return into the barrel
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
                if(Creature* carrot = me->FindNearestCreature(200502, 50000.0f, true))
                    c->GetMotionMaster()->MoveFollow(carrot, 0.5f, carrot->GetOrientation() - M_PI / 4);
            }

            Talk(TALK_JUST_DIED);
        }

        void KilledUnit(Unit *killed)
        {
            Talk(TALK_KILLED_PLAYER);
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return ;
				
			if(b_carrotBreath) {
				/// Event if the client doesn't see the update (because we do not use MSG_START_TURN_LEFT ?), we need to update 
				/// the orientation inside the core, in order to let the SpellScript of CarrotBreath correctly filter the targets
				/// In one ms, the boss should have turned of 2 * M_PI (circumference of a 1 meter circle) divided by 15000 ms (duration of the spell) degrees
				float turn = 2 * M_PI / 15000.0f ; // This is the distance in one ms
				turn *= float(diff); // Since last tick of the world
				
				float orientation = me->GetOrientation(); // Current orientation
				orientation -= turn ; // Rotate it
				
				me->SetOrientation(orientation) ; // Update
			}

            events.Update(diff);

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_SUMMON_VIRMEN :
                    if(m_uiSummonTimes == 0) {
                        Talk(TALK_SUMMON_VIRMEN);
                        DoCast(me, 114367, true);
                    }
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
                        break ;
                    }
                    Talk(TALK_FURLWIND);
                    DoCastAOE(SPELL_FURLWIND);
                    events.ScheduleEvent(EVENT_FURLWIND, IsHeroic() ? urand(12000, 14000) : urand(14000, 17000));
					events.ScheduleEvent(EVENT_RESET_SPEED, 10000);
                    break ;

                case EVENT_CARROT_BREATH :
                    if(me->HasUnitState(UNIT_STATE_CASTING)) {
                        events.ScheduleEvent(EVENT_CARROT_BREATH, 100);
                        break ;
                    }
					b_carrotBreath = true ;
					DoCast(SPELL_CARROT_BREATH);
					Talk(TALK_CARROT_BREATH);
                    events.ScheduleEvent(EVENT_CARROT_BREATH, IsHeroic() ? 25000 : 35000);
                    break ;
				
				case EVENT_RESET_SPEED :
					me->SetSpeed(MOVE_RUN, 7.0f);
					me->GetMotionMaster()->MoveChase(me->getVictim());
					break ;
				
				case EVENT_RESET_STALKER :
					b_carrotBreath = false ;
					break ;
					
                default :
                    break ;
                }
            }
			DoMeleeAttackIfReady();
        }

        void SummonVirmens()
        {
            for(uint8 i = 0 ; i < 5 ; ++i)
            {
                const Position posSummon = summonVirmenPosition[urand(0, MAX_SUMMON_VIRMEN - 1)],
                        posJump = jumpVirmenPosition[urand(0, MAX_SUMMON_VIRMEN - 1)];

				uint32 entry = RAND(MOB_HOPPER, MOB_HOPPLING, MOB_BOPPER);

                if(Creature* summon = me->SummonCreature(entry, posSummon))
                    summon->GetMotionMaster()->MoveJump(posJump, 8.0f, 8.0f);
            }
        }

    private :
        EventMap events ;
        InstanceScript * instance ;
        uint8 m_uiSummonTimes ;
		float orientation ;

        bool b_carrotBreath ;
        TempSummon* stalker ;
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
			// Init
			m_uiId = 0 ;
			m_uiWaitTimer = 0;
			x = y = z = 0.0f ;
			
			p_master = me->ToTempSummon()->GetSummoner() ;
            p_instance = p_master->GetInstanceScript();
			
			me->SetFacingToObject(p_master);
			me->SetTarget(p_master->GetGUID());
            angle = me->GetOrientation();
			
			// Positions
			rayon = me->GetExactDist2d(p_master->GetPositionX(), p_master->GetPositionY());
            center.Relocate(p_master->GetPositionX(), p_master->GetPositionY(), p_master->GetPositionZ());
			
			// Speed
			me->SetSpeed(MOVE_RUN, (2 * M_PI * rayon / 15.0f), true);
            me->SetSpeed(MOVE_FLIGHT, (2 * M_PI * rayon / 15.f) , true);
		}
		
		void UpdateAI(const uint32 uiDiff)
        {
			m_uiWaitTimer += uiDiff ;
			if(m_uiWaitTimer <= 2000)
				return ;
				
			angle -= (2 * M_PI / 15000.0f) * uiDiff ;
			
			x = p_master->GetPositionX() + rayon * cos(angle);
			y = p_master->GetPositionY() + rayon * sin(angle);
            z = p_master->GetPositionZ() ;
			++m_uiId ;
			
			me->GetMotionMaster()->MovePoint(m_uiId, x, y, z);
		}

    private :

        /// Used in MotionMaster, id of the point
        uint32 m_uiId ;

        /// Coordinates
        float x, y, z ;

        /// Pointer to the master
        Unit* p_master ;

        /// Pointer to InstanceScript in case
        InstanceScript* p_instance ;

        /// Distance from the caster
        float rayon ;

        /// Angle
        float angle ;

        /// Position of the master
        Position center ;
		
		/// Enable timer
		uint32 m_uiWaitTimer ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new stalker_carrot_breathAI(creature);
    }
};

typedef boss_hoptallus::boss_hoptallusAI HoptallusAI ;

class spell_hoptallus_carrot_breath : public SpellScriptLoader
{
public :
    spell_hoptallus_carrot_breath() : SpellScriptLoader("spell_hoptallus_carrot_breath")
    {

    }

    class spell_hoptallus_carrot_breath_AuraScript : public AuraScript {
		PrepareAuraScript(spell_hoptallus_carrot_breath_AuraScript);
		
		bool Validate(const SpellInfo* spellInfo) {
			return true ;
		}
		
		bool Load() {
			return true ; 
		}
		
		void HandleApply(AuraEffect const* auraEff, AuraEffectHandleModes mode) {
			if(!GetCaster())
				return ;
				
			Unit* caster = GetCaster();
			
			if(TempSummon* summon = caster->SummonCreature(NPC_CARROT_BREATH_HELPER, 
															caster->GetPositionX() + 30 * cos(caster->GetOrientation()),
															caster->GetPositionY() + 30 * sin(caster->GetOrientation()),
															caster->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
				caster->CastSpell(summon, 74758, true);
		}
		
		void Register() {
			OnEffectApply += AuraEffectApplyFn(spell_hoptallus_carrot_breath_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		}
	};
	
	AuraScript* GetAuraScript() const {
		return new spell_hoptallus_carrot_breath_AuraScript();
	}
};

class spell_hoptallus_carrot_breath_periodic : public SpellScriptLoader {
public :
    spell_hoptallus_carrot_breath_periodic() : SpellScriptLoader("spell_hoptallus_carrot_breath_periodic") {
        
    }
    
    class InArcCheckPredicate {
    public :
        InArcCheckPredicate(Creature* source) : p_creatureSource(source) {
            
        }
        
        bool operator()(WorldObject * target) {
            Position* pos ;
            target->GetPosition(pos);
            
            if(p_creatureSource->HasInArc(static_cast<float>(M_PI / 3.0f), pos))
                return false ;
            
            return true ;
        }

    private :
        Creature* p_creatureSource ;
    };
    
    class spell_hoptallus_carrot_breath_periodic_SpellScript : public SpellScript {
        PrepareSpellScript(spell_hoptallus_carrot_breath_periodic_SpellScript)
        
        bool Validate(const SpellInfo* spellInfo) {
            return true ;
        }
        
        bool Load() {
            return true ;
        }
        
        typedef std::list<WorldObject*> WorldObjectList ;
        
        void FilterTargets(WorldObjectList& targets) {
            targets.remove_if(InArcCheckPredicate(GetCaster()->ToCreature()));
        }
        
        void Register() {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hoptallus_carrot_breath_periodic_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hoptallus_carrot_breath_periodic_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hoptallus_carrot_breath_periodic_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_CONE_ENEMY_24);
        }
    };
    
    SpellScript* GetSpellScript() const {
        return new spell_hoptallus_carrot_breath_periodic_SpellScript() ;
    }
};

class spell_hoptallus_furlwind : public SpellScriptLoader {
public :
    spell_hoptallus_furlwind() : SpellScriptLoader("spell_hoptallus_furlwind") {

    }

    class spell_hoptallus_furlwind_SpellScript : public SpellScript {
        PrepareSpellScript(spell_hoptallus_furlwind_SpellScript) ;

        bool Validate(const SpellInfo *spellInfo) {
            return true ;
        }

        bool Load() {
            return true ;
        }

        Unit* victim ;

        void HandleAfterCast() {
            Unit* caster = GetCaster();
            if(!caster) return ;

            victim = caster->getVictim();
            float x, y, z ;
			if(victim) {
				victim->GetPosition(x, y, z);
				caster->SetSpeed(MOVE_RUN, caster->GetSpeedRate(MOVE_RUN) * 2.5f);
				caster->GetMotionMaster()->MovePoint(0, x, y, z);
			}
			else
				caster->GetMotionMaster()->MoveRandom();
        }

        void Register() {
            AfterCast += SpellCastFn(spell_hoptallus_furlwind_SpellScript::HandleAfterCast);
        }
    };
	
    SpellScript* GetSpellScript() const {
        return new spell_hoptallus_furlwind_SpellScript();
    }
};

void AddSC_boss_hoptallus()
{
    new boss_hoptallus();
    new mob_virmen();
    new stalker_carrot_breath();
    new npc_big_ol_hammer();
    new spell_hoptallus_carrot_breath();
	new spell_hoptallus_carrot_breath_periodic();
    new spell_hoptallus_furlwind();
}
