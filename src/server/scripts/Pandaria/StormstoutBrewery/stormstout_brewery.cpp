#include "stormstout_brewery.h"
#include "MoveSpline.h"

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

namespace StormstoutBrewery {
    class DistanceOrderPred {
    public :
        DistanceOrderPred(Creature* source) : _source(source) {

        }

        bool operator()(Creature* first, Creature* second) {
            return _source->GetExactDist2d(first) < _source->GetExactDist2d(second);
        }

    private :
        Creature* _source ;
    };
} // Stormstout Brewery

class npc_sb_illusioner : public CreatureScript {
public :
    npc_sb_illusioner() : CreatureScript("npc_sb_illusioner") {

    }

    class npc_sb_illusioner_AI : public ScriptedAI {
    public :
        npc_sb_illusioner_AI(Creature* creature) : ScriptedAI(creature) {
            _target = NULL ;
            _spellId = 0 ;
        }

        void SetDatas(Creature* target, uint32 spellId) { // Can't do it another way
            _target = target ;
            _spellId = spellId ;

            DoCast(_target, _spellId);
        }

        void JustDied(Unit *killer) {
            if(_target)
                _target->RemoveAurasDueToSpell(_spellId);
        }

    private :
        Creature* _target ;
        uint32 _spellId ;
    };

    CreatureAI* GetAI(Creature *creature) const {
        return new npc_sb_illusioner_AI(creature);
    }
};

typedef npc_sb_illusioner::npc_sb_illusioner_AI IllusionerAI ;

class stalker_illusioner_master : public CreatureScript {
public :
    stalker_illusioner_master() : CreatureScript("stalker_illusioner_master") {

    }

    class stalker_illusioner_master_AI : public ScriptedAI {
    public:
        stalker_illusioner_master_AI(Creature* creature) : ScriptedAI(creature) {

        }

        void Reset() {
            std::list<Creature*> waterIllusioners, hozensWater, fieryIllusioners, hozensFire ;

            GetCreatureListWithEntryInGrid(waterIllusioners, me, 56865, 50000.0f); // Aqua dancers
            GetCreatureListWithEntryInGrid(fieryIllusioners, me, 56867, 50000.0f); // Fiery Tricksters

            GetCreatureListWithEntryInGrid(hozensWater, me, 59605, 50000.0f); // Sodden Hozen Brawlers
            GetCreatureListWithEntryInGrid(hozensFire, me, 56924, 50000.0f); // Inflammed Hozen Brawlers

            for(std::list<Creature*>::iterator water = waterIllusioners.begin() ; water != waterIllusioners.end() ; ++water ) {
                hozensWater.sort(StormstoutBrewery::DistanceOrderPred(*water));
                hozensWater.remove_if(Trinity::UnitAuraCheck(true, 107044));

                // Can't do it otherwise, if SetData could receive a void* pointer, it could work, but for now...
                if(IllusionerAI* ai = dynamic_cast<IllusionerAI*>((*water)->AI()))
                    ai->SetDatas(hozensWater.front(), 107044);
            }

            for(std::list<Creature*>::iterator fire = fieryIllusioners.begin() ; fire != fieryIllusioners.end() ; ++fire) {
                hozensFire.sort(StormstoutBrewery::DistanceOrderPred(*fire));
                hozensFire.remove_if(Trinity::UnitAuraCheck(true, 107175));

                if(IllusionerAI* ai = dynamic_cast<IllusionerAI*>((*fire)->AI()))
                    ai->SetDatas(hozensFire.front(), 107044);
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const {
        return new stalker_illusioner_master_AI(creature);
    }
};

class spell_sb_mind_illusion : public SpellScriptLoader {
public :
    spell_sb_mind_illusion() : SpellScriptLoader("spell_sb_mind_illusion") {

    }

    class spell_sb_mind_illusion_AuraScript : public AuraScript {
        PrepareAuraScript(spell_sb_mind_illusion_AuraScript);

        bool Validate(const SpellInfo *spellInfo) {
            return true ;
        }

        bool Load() {
            return true ;
        }

        void HandleEffectRemove(AuraEffect const* auraEff, AuraEffectHandleModes mode) {
            if(!GetOwner() || !GetOwner()->ToCreature())
                return ;

            Creature* owner = GetOwner()->ToCreature();

            switch(GetSpellInfo()->Id) {
            case 107044 :
                owner->CastSpell(owner, 114655);
                break ;

            case 107125 :
                owner->CastSpell(owner, 114656);
                break ;

            default :
                break ;
            }
        }

        void Register() {
            OnEffectRemove += AuraEffectRemoveFn(spell_sb_mind_illusion_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const {
        return new spell_sb_mind_illusion_AuraScript();
    }
};

class mob_hozen_illusioned : public CreatureScript {
public :
    mob_hozen_illusioned() : CreatureScript("mob_hozen_illusioned") {

    }

    class mob_hozen_illusioned_AI : public ScriptedAI {
    public :
        mob_hozen_illusioned_AI(Creature* creature) : ScriptedAI(creature) {

        }

        void UpdateAI(const uint32 diff) {
            if(!UpdateVictim())
                return ;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            Unit* victim = me->getVictim();
            //Make sure our attack is ready and we aren't currently casting before checking distance
            if (me->isAttackReady() && me->IsWithinMeleeRange(victim))
            {
                me->AttackerStateUpdate(victim);
                me->resetAttackTimer();
            }

            if (me->haveOffhandWeapon() && me->isAttackReady(OFF_ATTACK) && me->IsWithinMeleeRange(victim))
            {
                me->AttackerStateUpdate(victim, OFF_ATTACK);
                me->resetAttackTimer(OFF_ATTACK);
            }

            uint32 spellId = me->HasAura(107044) ? 107046 : me->HasAura(107175) ? 107176 : 0 ;
            if(spellId)
                DoCastAOE(spellId);
        }
    };

    CreatureAI* GetAI(Creature *creature) const {
        return new mob_hozen_illusioned_AI(creature);
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

const Position hozenFleePosition = {-774.154231f, 1420.184014f, 139.579605f, 3.463936f} ;

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
            if(InstanceScript* instance = me->GetInstanceScript())
            {
                instance->SetData64(INSTANCE_DATA64_KILLED_HOZENS, 1);
                AddPowerOnPlayers(1);
            }
        }

        void AddPowerOnPlayer(Player* p, int32 amount)
        {
            if(p)
            {
				p->SetMaxPower(POWER_ALTERNATE_POWER, 40);
				p->SetPower(POWER_ALTERNATE_POWER, p->GetPower(POWER_ALTERNATE_POWER) + 1);
			}
        }

        void AddPowerOnPlayers(int32 amount)
        {
            if(Map* map = me->GetMap())
            {
                Map::PlayerList const & playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                {
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
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

#define MAX_SUMMONING_BARRELS_POSITIONS 6

const Position PointBarrelsPositions[MAX_SUMMONING_BARRELS_POSITIONS] =
{
    {-779.830566f, 1382.367676f, 146.725952f, 2.957428f},
    {-795.707092f, 1360.484253f, 146.713882f, 3.337561f},
    {-750.099548f, 1303.637207f, 146.692581f, 4.484246f},
    {-727.915039f, 1367.620117f, 146.710236f, 0.289437f},
    {-735.368713f, 1310.069702f, 146.700073f, 5.531971f},
    {-753.769287f, 1391.448730f, 146.722473f, 0.981375f}
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
            DoCast(me, 106647, true); // Let's roll !
        }
		
		void DoAction(const int32 actionId)
		{
			if(actionId == 0)
			{
				me->GetMotionMaster()->MovePoint(0, PointBarrelsPositions[m_uiIndex]);
			}
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

        void PassengerBoarded(Unit *passenger, int8 seatId, bool enter)
        {
            if(enter)
                me->StopMoving();
            else {
                if(instance)
                    instance->ProcessEvent(NULL, m_uiIndex);
                me->Kill(me);
                return ;
            }
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
            if(m_uiCheckTimer <= diff)
                DoCheckForExplosion();
            else
                m_uiCheckTimer -= diff ;
        }

        void DoCheckForExplosion() {
            // Check instance boss status
            if(instance && instance->GetData(INSTANCE_DATA_OOK_OOK_STATUS) == IN_PROGRESS) { // Check for OokOok
                Creature* ookOok = ObjectAccessor::GetCreature(*me, instance->GetData64(INSTANCE_DATA64_OOK_OOK_GUID));
                if(ookOok) {
                    float dist = me->GetExactDist2d(ookOok);
                    if(dist <= 2.0f) {
                        CastExplosion();
                        return ;
                    }
                }
            } else { // Check for hozens
                std::list<Creature*> hozens ;
                GetCreatureListWithEntryInGrid(hozens, me, 56927, 50000.0f);
                GetCreatureListWithEntryInGrid(hozens, me, 59684, 50000.0f);
                GetCreatureListWithEntryInGrid(hozens, me, 57097, 50000.0f);

                if(hozens.empty())
                    return ;

                for(std::list<Creature*>::iterator iter = hozens.begin() ; iter != hozens.end() ; ++iter) {
                    if(Creature* hozen = *iter) {
                        float dist = me->GetExactDist2d(hozen);
                        if(dist <= 2.0f) {
                            CastExplosion();
                            return ;
                        }
                    }
                }
            }

            // Didn't return yet, so it means nobody was found, so reschedule the timer
            m_uiCheckTimer = 500 ;
        }

        inline void CastExplosion() {
            DoCastAOE(115875);
            instance->ProcessEvent(NULL, m_uiIndex);
            if(Vehicle* vec = me->GetVehicleKit())
                vec->RemoveAllPassengers();
            me->Kill(me);
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
			m_uiPoint = 1 ;
			m_uiMoveTimer = 24 * 60 * 60 ;
			Position const movePosition = {-710.459900f, 1310.748657f, 162.627029f, 2.754009f}; // Aka, pos 0 (Motion Master)
            me->GetMotionMaster()->MovePoint(0, movePosition);
        }

        void MovementInform(uint32 type, uint32 id)
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY: Virmen entering MovementInform, using type %u and point %u", type, id);
            if(type == POINT_MOTION_TYPE)
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY: Virmen MovementInfo : setting timer !");
                m_uiMoveTimer = 100 ;
            }
        }

        void JustDied(Unit *killer)
        {
            if(InstanceScript* instance = me->GetInstanceScript())
                instance->SetData64(INSTANCE_DATA64_KILLED_HOPTALLUS_TRASH, 1);
        }

        void UpdateAI(const uint32 diff)
        {
			if(m_uiMoveTimer <= diff)
			{
				if(m_uiPoint < MAX_TRASH_POINT_ID)
				{
					Position pos = TrashPoints[m_uiPoint];
					float x, y, z ;
					x = pos.GetPositionX();
					y = pos.GetPositionY();
					z = pos.GetPositionZ();
					
					++m_uiPoint ;
					sLog->outDebug(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY: Virmen UpdateAI : next move point set to id %u, using x %f, y %f, z %f", m_uiPoint, x, y, z);
					me->GetMotionMaster()->MovePoint(m_uiPoint, x, y, z);
					m_uiMoveTimer = 24 * 60 * 60 ;
				}
				else
				{
					me->Kill(me);
				}
			}
			else
				m_uiMoveTimer -= diff ;
			
            if(!UpdateVictim())
                return ;

            DoMeleeAttackIfReady();
        }
		
	private :
		uint32 m_uiPoint ;
		uint32 m_uiMoveTimer ;
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

#define PROGRESS_X 2.038879f
#define PROGRESS_Y 8.39978f

#define PROGRESS_LINE_X 8.077647f
#define PROGRESS_LINE_Y -2.111451f
#define PROGRESS_Z 5.0f

class npc_uncle_gao : public CreatureScript {

public :
    npc_uncle_gao() : CreatureScript("npc_uncle_gao") {

    }

    class npc_uncle_gao_AI : public ScriptedAI {
    public :
        npc_uncle_gao_AI(Creature* creature) : ScriptedAI(creature) {
			_instance = creature->GetInstanceScript();
        }

        /** ScriptedAI functions **/

        void Reset() {
            /** Initialize everything **/
            _events.Reset();

            _aleAlamental = _stoutAlamental = _wheatAlamental = 0 ;
            _numberOfSummons = 0 ;

            _phase = PHASE_NOT_STARTED ;

            /** And start **/
            _events.ScheduleEvent(EVENT_CHECK_FOR_PLAYERS, 500);
        }

        void UpdateAI(const uint32 diff) {
            // Not start if Hoptallus not done
            if(_instance)
                if(_instance->GetData(INSTANCE_DATA_HOPTALLUS_STATUS) != DONE)
                    return ;

            // Update EventMap
            _events.Update(diff);

            // And switch
            while(uint32 eventId = _events.ExecuteEvent()) {
                switch(eventId) {
                case EVENT_CHECK_FOR_PLAYERS :
                    CheckForPlayers();
                    break ;

                case EVENT_SUMMON_FIRST_WAVE :
                    SummonFirstWave();
                    break ;

                case EVENT_SUMMON_SECOND_WAVE :
                    SummonSecondWave();
                    break ;

                case EVENT_SUMMON_THIRD_WAVE :
                    SummonThirdWave();
                    break ;

                case EVENT_SUMMON_YAN_ZHU :
                    SummonYanZhu();
                    break ;

                case EVENT_PERFECT_BREW :
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    Talk(TALK_PERFECT_BREW);
                    break ;

                default :
                    break ;
                }
            }
        }

        void SummonedCreatureDies(Creature *summoned, Unit *killer) {

            // Yan Zhu not yet started
            if(_phase < PHASE_YAN_ZHU) {
                --_numberOfSummons ;

                // Everybody killed, start next wave
                if(_numberOfSummons == 0) {
                    switch(_phase) {
                    case PHASE_FIRST_WAVE :
                        Talk(TALK_SUMMON_SECOND_WAVE);
                        _events.ScheduleEvent(EVENT_SUMMON_SECOND_WAVE, 4000);
                        break ;

                    case PHASE_SECOND_WAVE :
                        Talk(TALK_SUMMON_THIRD_WAVE);
                        _events.ScheduleEvent(EVENT_SUMMON_THIRD_WAVE, 4000);
                        break ;

                    case PHASE_THIRD_WAVE :
                        _events.ScheduleEvent(EVENT_SUMMON_YAN_ZHU, 4000);
                        break ;

                    default :
                        break ;
                    }
                }
            } else if(_phase == PHASE_YAN_ZHU) {
                // Yan Zhu has been killed, schedule final event
                _numberOfSummons = 0 ;
                _phase = PHASE_END ;
                Creature* yanZhu = me->FindNearestCreature(BOSS_YAN_ZHU, 200.0f, false);
                if(!yanZhu)
                    return ;

                Talk(TALK_YAN_ZHU_DEATH);
                me->GetMotionMaster()->MovePoint(POINT_YAN_ZHU_CORPSE, *yanZhu);
            } else return ;
        }

        void MovementInform(uint32 motionType, uint32 pointId) {
            if(motionType != POINT_MOTION_TYPE
                    || pointId != POINT_YAN_ZHU_CORPSE)
                return ;

            _events.ScheduleEvent(EVENT_PERFECT_BREW, 2000);
        }

        /** Non ScriptedAI functions **/

        void CheckForPlayers() {
            if(Map* map = me->GetMap()) {
                Map::PlayerList const& playerList = map->GetPlayers();

                if(playerList.isEmpty())
                    return ;

                for(Map::PlayerList::const_iterator c_iter = playerList.begin() ; c_iter != playerList.end() ; ++c_iter) {
                    if(Player* player = c_iter->getSource()) {
                        float dist = me->GetExactDist2d(player);

                        // Player found, start next wave then return ;
                        if(dist <= 10.0f) {
                            _phase = PHASE_FIRST_WAVE ;

                            Talk(TALK_SUMMON_FIRST_WAVE);
                            _events.ScheduleEvent(EVENT_SUMMON_FIRST_WAVE, 4000);

                            /// @todo : remove this as soon as possible
                            if(_instance)
                                _instance->DoSendNotifyToInstance("Starting Gao Event");

                            return ;
                        }
                    }
                }

                // No player found, check again
                _events.ScheduleEvent(EVENT_CHECK_FOR_PLAYERS, 500);
            }
        }

        void SummonFirstWave() {
            _wheatAlamental = RAND(MOB_WHEAT_ALAMENTAL_BLOATED_BREW, MOB_WHEAT_ALAMENTAL_STOUT_BREW);

            Position start = {-704.517517f, 1166.889648f, 166.141571f, 0.275305f};
            for(uint8 i = 0 ; i < 2 ; ++i)
                if(Creature* summon = me->SummonCreature(_wheatAlamental, start.GetPositionX() + PROGRESS_X * i, start.GetPositionY() + PROGRESS_Y * i, start.GetPositionZ()))
                    summon->SetOrientation(0.0f);

            _numberOfSummons = 2 ;
        }

        void SummonSecondWave() {
            _aleAlamental = RAND(MOB_ALE_ALAMENTAL_BUBBLING_BREW, MOB_ALE_ALAMENTAL_YEASTY_BREW);

            Position start = {-713.708191f, 1168.700195f, 166.145874f, 0.275305f};

            for(uint8 i = 0 ; i < 3 ; ++i) {
                float x = start.GetPositionX() + PROGRESS_LINE_X * i ;
                float y = start.GetPositionY() + PROGRESS_LINE_Y * i ;
                for(uint8 j = 0 ; j < 3 ; ++j)
                    if(Creature* summon = me->SummonCreature(_aleAlamental, x + PROGRESS_X * j, y + PROGRESS_Y * j, start.GetPositionZ()))
                        summon->SetOrientation(0.0f);
            }

            _numberOfSummons = 9 ;
        }

        void SummonThirdWave() {
            _stoutAlamental = RAND(MOB_STOUT_ALAMENTAL_FIZZY_BREW, MOB_STOUT_ALAMENTAL_SUDSY_BREW);

            Position start = {-713.708191f, 1168.700195f, 166.145874f, 0.275305f};

            for(uint8 i = 0 ; i < 2 ; ++i) {
                float x = start.GetPositionX() + 2 * PROGRESS_LINE_X * i ;
                float y = start.GetPositionY() + 2 * PROGRESS_LINE_Y * i ;
                for(uint8 j = 0 ; j < 2 ; ++j)
                    if(Creature* summon = me->SummonCreature(_stoutAlamental, x + 2 * PROGRESS_X * j, y + 2 * PROGRESS_Y * j, start.GetPositionZ()))
                        summon->SetOrientation(0.0f);
            }

            _numberOfSummons = 4 ;
        }

        void SummonYanZhu() {
            Talk(TALK_SUMMON_YAN_ZHU);

            Position const summon = {-703.415771f, 1162.801025f, 163.141693f, 0.273505f}; // Correct height is 160, but YanZhu must "grow" from ground
            Creature* yanZhu = me->SummonCreature(BOSS_YAN_ZHU, summon, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 24 * HOUR * IN_MILLISECONDS);

            if(!yanZhu) {
                sLog->outFatal(LOG_FILTER_NETWORKIO, "STORMSTOUT BREWERY : Unable to summon Yan Zhu the Uncasked !");
                if(_instance)
                    _instance->DoSendNotifyToInstance("DEBUG LOG : Unable to summon Yan Zhu the Uncasked");
            }

            // Cast the spells on YanZhu, based on which creatures were summoned
            if(_stoutAlamental == MOB_STOUT_ALAMENTAL_FIZZY_BREW)
                DoCast(yanZhu, SPELL_FIZZY_BREW);
            else
                DoCast(yanZhu, SPELL_SUDSY_BREW);

            if(_aleAlamental == MOB_ALE_ALAMENTAL_BUBBLING_BREW)
                DoCast(yanZhu, SPELL_BUBBLING_BREW);
            else
                DoCast(yanZhu, SPELL_YEASTY_BREW);

            if(_wheatAlamental == MOB_WHEAT_ALAMENTAL_BLOATED_BREW)
                DoCast(yanZhu, SPELL_BLACKOUT_BREW);
            else
                DoCast(yanZhu, SPELL_BLOATING_BREW);

        }

    private :
        /// May be usefull
        EventMap _events ;

        /// Pointer to the instance
        InstanceScript* _instance ;

        /// Entries of the alamentals to be summoned ; also determines the spells YanZhu will cast
        uint32 _aleAlamental, _stoutAlamental, _wheatAlamental ;

        /// Npcs summoned ; when it reaches 0, it means we have to start the next phase
        uint32 _numberOfSummons ;
		
		/// Points to go to
		enum MovePoints {
			POINT_YAN_ZHU_CORPSE = 1
		};
		
		enum Phases {
			PHASE_NOT_STARTED   = 0,
			PHASE_FIRST_WAVE    = 1,
			PHASE_SECOND_WAVE   = 2,
			PHASE_THIRD_WAVE    = 3,
			PHASE_YAN_ZHU       = 4,
			PHASE_END           = 5
		};
		
        /// Current phase
        Phases _phase ;
    };

    CreatureAI* GetAI(Creature *creature) const {
        return new npc_uncle_gao_AI(creature);
    }

private :
    /// Creatures to be summoned
    enum Creatures {
        // Stout alamentals
        MOB_STOUT_ALAMENTAL_SUDSY_BREW      = 59522,
        MOB_STOUT_ALAMENTAL_FIZZY_BREW      = 59520,

        // Ale alamentals
        MOB_ALE_ALAMENTAL_BUBBLING_BREW     = 59521,
        MOB_ALE_ALAMENTAL_YEASTY_BREW       = 59494,

        // Wheat alamentals
        MOB_WHEAT_ALAMENTAL_STOUT_BREW      = 59519,
        MOB_WHEAT_ALAMENTAL_BLOATED_BREW    = 59518
    };

    /// Spells to be casted
    enum Spells {
        SPELL_SUDSY_BREW    = 114933,
        SPELL_FIZZY_BREW    = 114934,

        SPELL_BLOATING_BREW = 114929,
        SPELL_BLACKOUT_BREW = 114930,

        SPELL_BUBBLING_BREW = 114931,
        SPELL_YEASTY_BREW   = 114932
    };

    /// Dialogs
    enum Talk {
        TALK_SUMMON_FIRST_WAVE  = 1,
        TALK_SUMMON_SECOND_WAVE = 2,
        TALK_SUMMON_THIRD_WAVE  = 3,
        TALK_SUMMON_YAN_ZHU     = 4,
        TALK_YAN_ZHU_DEATH      = 5,
        TALK_PERFECT_BREW       = 6
    };

    /// Events
    enum Events {
        EVENT_CHECK_FOR_PLAYERS     = 1,
        EVENT_SUMMON_FIRST_WAVE     = 2,
        EVENT_SUMMON_SECOND_WAVE    = 3,
        EVENT_SUMMON_THIRD_WAVE     = 4,
        EVENT_SUMMON_YAN_ZHU        = 5,
        EVENT_PERFECT_BREW          = 6
    };
};

void AddSC_stormstout_brewery()
{
    // Ook Ook
    new npc_chen_stormstout();
    new npc_auntie_stormstout();
    new npc_sb_illusioner();
    new stalker_illusioner_master();
    new spell_sb_mind_illusion();
    new mob_hozen_illusioned();
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
