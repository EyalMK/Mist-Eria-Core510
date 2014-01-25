#include "stormstout_brewery.h"

enum Spells
{

};

enum Actions
{

};

enum Events
{
    EVENT_ALE           = 1,
    EVENT_STOUT         = 2,
    EVENT_WHEAT         = 3,
    EVENT_CHECK_JUMP    = 4,
    EVENT_CHECK_DIST    = 5
};

enum Creatures
{
    MOB_BUBBLE_SHIELD = 65522
};

#define MIN_GROUND_Z 170.0f

#define PROGRESS_EAST_X 5.0f
#define PROGRESS_EAST_Y 5.0f

#define PROGRESS_NORTH_X 5.0f
#define PROGRESS_NORTH_Y 5.0f

const Position eastPosition = {0.0f, 0.0f, 0.0f, 0.0f};
const Position northPosition = {0.0f, 0.0f, 0.0f, 0.0f};

class boss_yan_zhu_the_uncasked : public CreatureScript
{
public :
    boss_yan_zhu_the_uncasked() : CreatureScript("boss_yan_zhu_the_uncasked")
    {

    }

    class boss_yan_zhu_the_uncaskedAI : public ScriptedAI
    {
    public :
        boss_yan_zhu_the_uncaskedAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            m_uiAleAbility = 106563 ;
            m_uiStoutAbility = 115003 ;
            m_uiWheatAbility = 106546 ;
            m_uiSudsLoop = 0 ;
        }

        void Reset()
        {
            events.Reset();
            if(instance)
                instance->SetData(INSTANCE_DATA_YAN_ZHU_STATUS, NOT_STARTED);
            m_uiSudsLoop = 0 ;
        }

        void EnterCombat(Unit *aggro)
        {
            if(instance)
            {
                instance->SetData(INSTANCE_DATA_YAN_ZHU_STATUS, IN_PROGRESS);
                if(GameObject* entrance = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_YAN_ZHU_ENTRANCE_GUID)))
                    entrance->SetGoState(GO_STATE_READY);
            }
        }

        void EnterEvadeMode()
        {
            if(instance)
            {
                instance->SetData(INSTANCE_DATA_YAN_ZHU_STATUS, FAIL);
                if(GameObject* entrance = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_YAN_ZHU_ENTRANCE_GUID)))
                    entrance->SetGoState(GO_STATE_ACTIVE);
            }

            ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit *killer)
        {
            if(instance)
            {
                instance->SetData(INSTANCE_DATA_YAN_ZHU_STATUS, DONE);
                if(GameObject* entrance = ObjectAccessor::GetGameObject(*me, instance->GetData64(INSTANCE_DATA64_YAN_ZHU_ENTRANCE_GUID)))
                    entrance->SetGoState(GO_STATE_ACTIVE);
            }
        }

        void SummonedCreatureDies(Creature *summon, Unit *killer)
        {
            if(summon->GetEntry() == MOB_BUBBLE_SHIELD)
            {
                if(Aura* shield = me->GetAura(106563))
                {
                    if(shield->GetStackAmount() == 1)
                    {
                        me->RemoveAura(106563);
                        return ;
                    }
                    else
                        shield->SetStackAmount(--shield->GetStackAmount());
                }
            }
        }

        void SetData(uint32 data, uint32 value)
        {
            switch(data)
            {
            case BOSS_YAN_ZHU_DATA_ALE_SPELL :
                if(value == 66413)
                {
                    m_uiAleAbility = value ; // Have a strong look at this
                }
                else
                {

                }
                break ;

            case BOSS_YAN_ZHU_DATA_STOUT_SPELL :
                if(value == 59522)
                {
                    m_uiStoutAbility = 114466 ;
                }
                else
                {

                }
                break ;

            case BOSS_YAN_ZHU_DATA_WHEAT_SPELL :
                if(value == 59519)
                {
                    m_uiWheatAbility = 106851 ;
                }
                else
                {

                }
                break ;
            }
        }

        void DoAction(const int32 action)
        {
            if(action == 0)
            {
                events.Reset();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return ;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return ;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_ALE :
                    if(m_uiAleAbility == 66413)
                    {
                        for(uint8 i = 0 ; i < 2 ; ++i)
                            me->SummonCreature(MOB_YEASTY_BREW_ALEMENTAL, me->GetPositionX() + rand() % 10, me->GetPositionY() + rand() % 10,
                                               me->GetPositionZ() + 0.5f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60 * MINUTE);
                    }
                    else
                    {
                        for(float i = 0.0f ; i < 360.0f ; i += float(360/8))
                            if(Creature* bubble = me->SummonCreature(MOB_BUBBLE_SHIELD, me->GetPositionX() + 0.5 * cos(i),
                                                                     me->GetPositionY() + 0.5 * sin(i), me->GetPositionZ() + 5.0f,
                                                                     0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 100))
                                bubble->CastSpell(me, m_uiAleAbility, true);
                    }
                    events.ScheduleEvent(EVENT_ALE, IsHeroic() ? urand(15000, 20000) : urand(25000, 35000));
                    break ;

                case EVENT_STOUT :
                    if(m_uiStoutAbility == 115003)
                    {
                        DoCastAOE(m_uiStoutAbility);
                        SummonBubbles();
                    }
                    else
                    {
                        if(Creature* sud = me->SummonCreature(NPC_SUD, eastPosition.GetPositionX() + PROGRESS_EAST_X * m_uiSudsLoop,
                                                              eastPosition.GetPositionY() + PROGRESS_EAST_Y * m_uiSudsLoop, me->GetPositionZ(),
                                                              0, TEMPSUMMON_TIMED_DESPAWN, 1500))
                            sud->CastSpell((Unit*)NULL, m_uiStoutAbility);
                        if(Creature* sud = me->SummonCreature(NPC_SUD, northPosition.GetPositionX() + PROGRESS_NORTH_X * m_uiSudsLoop,
                                                              northPosition.GetPositionY() + PROGRESS_NORTH_Y * m_uiSudsLoop, me->GetPositionZ(),
                                                              0, TEMPSUMMON_TIMED_DESPAWN, 1500))
                            sud->CastSpell((Unit*)NULL, m_uiStoutAbility);

                        ++m_uiSudsLoop ;

                        if(m_uiSudsLoop < 9)
                        {
                            events.ScheduleEvent(EVENT_STOUT, 1500);
                            if(m_uiSudsLoop == 1)
                                events.ScheduleEvent(EVENT_JUMP, 500);
                            break ;
                        }
                    }
                    events.ScheduleEvent(EVENT_STOUT, IsHeroic() ? urand(35000, 55000) : urand(60000, 90000));
                    break ;

                case EVENT_WHEAT :
                    if(m_uiWheatAbility == 106546)
                    {
                        if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            DoCast(target, m_uiWheatAbility, true);
                    }
                    else
                        DoCastAOE(m_uiWheatAbility);
                    events.ScheduleEvent(EVENT_WHEAT, IsHeroic() ? urand(12000, 14000) : urand(15000, 20000));
                    break ;

                case EVENT_CHECK_DIST :
                    if(!DoCheckPlayers())
                        if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            DoCast(target, 114548);
                    events.ScheduleEvent(EVENT_CHECK_DIST, 1000);
                    break ;

                case EVENT_JUMP :
                    DoCheckPlayers(false);
                    if(m_uiSudsLoop < 9)
                        events.ScheduleEvent(EVENT_JUMP, 500);
                    break ;
                }
            }
        }

        bool DoCheckPlayers(bool melee = true)
        {
            bool toReturn = true ;
            if(Map* map = me->GetMap())
            {
                Map::PlayerList const& playerList = map->GetPlayers();
                if(!playerList.isEmpty())
                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                    {
                        if(melee)
                            toReturn = DoCheckMelee(iter->getSource()) ;
                        else
                            DoCheckJump(iter->getSource());
                    }
            }

            return toReturn ;
        }

        bool DoCheckMelee(Player* player)
        {
            if(player)
                return (player->GetExactDist2d(me->GetPositionX(), me->GetPositionY() <= 5.0f)) ;
            else
                return false ;
        }

        void DoCheckJump(Player* player)
        {
            if(player)
                if((player->GetPositionZ() > MIN_GROUND_Z + 0.2f))
                    if(Aura* aura = player->GetAura(114468))
                        player->CastSpell(player, aura->GetSpellInfo()->Effects[0].TriggerSpell, true);
        }

        void SummonBubbles()
        {

        }

    private :
        uint32 m_uiAleAbility;
        uint32 m_uiStoutAbility;
        uint32 m_uiWheatAbility;

        uint32 m_uiSudsLoop ;

        EventMap events ;
        InstanceScript* instance ;
    };
};

class WallOfSudsTargetSelectorPredicate
{
public :
    WallOfSudsTargetSelectorPredicate(Unit* caster) : _caster(caster)
    {

    }

    bool operator()(WorldObject* target)
    {
        if(!target || !_caster)
            return true ;

        if(target->GetTypeId() != TYPEID_PLAYER)
            return true ;

        if(!target->isInFront(_caster))
                return true ;

        return false ;
    }

private :
    Unit* _caster ;
};

class spell_sb_wall_of_suds : public SpellScriptLoader
{
public :
    spell_sb_wall_of_suds() : SpellScriptLoader("spell_sb_wall_of_suds")
    {

    }

    class spell_sb_wall_of_suds_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_sb_wall_of_suds_SpellScript);

        bool Validate(const SpellInfo *spellInfo)
        {
            return true ;
        }

        bool Load()
        {
            return true ;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if(WallOfSudsTargetSelectorPredicate(GetCaster()));
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sb_wall_of_suds_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ANY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sb_wall_of_suds_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_TARGET_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_sb_wall_of_suds_SpellScript();
    }
};


void AddSC_boss_yan_zhu_the_uncasked()
{
    new boss_yan_zhu_the_uncasked();
    new spell_sb_wall_of_suds();
}
