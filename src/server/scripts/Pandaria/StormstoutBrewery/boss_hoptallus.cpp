#include "stormstout_brewery.h"

#define SUMMONING_RAYON 30.0f

enum Spells
{
    // Hoptallus
    Spell_Hoptallus_Furlwind = 112992,
    Spell_Hoptallus_FurlwindPeriodic = 112993,
    Spell_Hoptallus_CarrotBreath = 112944,
    Spell_Hoptallus_CarrotBreathPeriodic = 112945,

    // Hopper
    Spell_Hopper_ExplosiveBrew = 114291,

    // Big Ol' Hammer
    Spell_Hammer_SmashOverrider = 111662, /** @todo : look carefully those two */
    Spell_Hammer_SmashDamager = 111666
};

enum Creatures
{
    Npc_CarrotBreathStalker = 56716,
    Npc_BigOlHammer = 59539,
    Mob_Hoppling = 60208, // Do nothing
    Mob_Hopper = 59464, // Explosive Brew
    Mob_Bopper = 59551 // Hammer
};

enum Events
{
    // Hoptallus
    Event_Hoptallus_Furlwind = 1,
    Event_Hoptallus_CarrotBreath = 2,
    Event_Hoptallus_SummonVirmen = 3
};

enum Actions
{
    Action_Hoptallus_StartSummoning = 0
};

enum Says
{
    Talk_Hoptallus_Aggro = 1,
    Talk_Hoptallus_Furlwind = 2,
    Talk_Hoptallus_CarrotBreath = 3,
    Talk_Hoptallus_KilledPlayer = 4,
    Talk_Hoptallus_Death = 5
};

#define MAX_SUMMON_VIRMEN_POSITIONS 8
static const Position summonVirmenPositions[MAX_SUMMON_VIRMEN_POSITIONS] =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f}
};

#define MAX_JUMP_VIRMEN_POSITIONS 6
static const Position jumpVirmenPositions[MAX_JUMP_VIRMEN_POSITIONS] =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f}
};

#define MAX_PRE_EVENT_ADDS 3
static const uint32 preEventAdds[MAX_PRE_EVENT_ADDS] = {56718, 59426, 59460};

#define MAX_POINT_PATH_ADDS 8
static const Position preEventAddsPaths[MAX_POINT_PATH_ADDS] =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f}
};

class boss_hoptallus : public CreatureScript
{
public :
    boss_hoptallus() : CreatureScript("boss_hoptallus")
    {

    }

    struct boss_hoptallus_AIScript : public ScriptedAI
    {
    public :
        boss_hoptallus_AIScript(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            b_hasStartedOnce = false ;
            m_uiSummonTimer = 1000 ;
        }

        void Reset()
        {
            if(instance)
                instance->SetData(Data_HoptallusEventProgress, NOT_STARTED);
            events.Reset();
        }

        void EnterEvadeMode()
        {
            if(instance)
                instance->SetData(Data_HoptallusEventProgress, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void DoAction(const int32 actionId)
        {
            if(actionId == Action_Hoptallus_StartSummoning)
                DoPreEvent(0);
        }

        void EnterCombat(Unit *who)
        {
            if(instance)
                instance->SetData(Data_HoptallusEventProgress, IN_PROGRESS);

            if(!b_hasStartedOnce)
                b_hasStartedOnce = true ;

            Talk(Talk_Hoptallus_Aggro);
            DoZoneInCombat();
            events.ScheduleEvent(Event_Hoptallus_Furlwind, IsHeroic() ? urand(6000, 8000) : urand(7000, 10000));
            events.ScheduleEvent(Event_Hoptallus_CarrotBreath, IsHeroic() ? 10000 : 15000);
            events.ScheduleEvent(Event_Hoptallus_SummonVirmen, IsHeroic() ? 15000 : 20000);
        }

        void KilledUnit(Unit *killed)
        {
            Talk(Talk_Hoptallus_KilledPlayer);
        }

        void JustDied(Unit *killer)
        {
            if(instance)
                instance->SetData(Data_HoptallusEventProgress, DONE);

            Talk(Talk_Hoptallus_Death);
            ClearZone();
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(!b_hasStartedOnce)
                DoPreEvent(uiDiff);

            if(!UpdateVictim())
                return ;

            events.Update(uiDiff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return ;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case Event_Hoptallus_CarrotBreath :
                    DoCast(Spell_Hoptallus_CarrotBreath);
                    events.ScheduleEvent(Event_Hoptallus_CarrotBreath, IsHeroic() ? 10000 : 15000);
                    break ;

                case Event_Hoptallus_Furlwind :
                    DoCast(Spell_Hoptallus_Furlwind);
                    events.ScheduleEvent(Event_Hoptallus_Furlwind, IsHeroic() ? urand(5000, 6500) : urand(6000, 8500));
                    break ;

                case Event_Hoptallus_SummonVirmen :
                    SummonVirmen();
                    events.ScheduleEvent(IsHeroic() ? 15000 : 20000);
                    break ;

                default :
                    break ;
                }
            }

            DoMeleeAttackIfReady();
        }

        void SummonVirmen()
        {
            uint32 counts[3] = {urand(1, 5), urand(1, 4), urand(1, 3)};
            uint32 entry ;
            for(uint8 i = 0 ; i < 3 ; ++i)
            {
                switch(i)
                {
                case 0 :
                    entry = Mob_Hoppling ;
                    break ;

                case 1:
                    entry = Mob_Hopper ;
                    break ;

                case 2:
                    entry = Mob_Bopper ;
                    break ;
                }

                for(uint32 j = 0 ; j < counts[i] ; ++j)
                {

                    Creature* virmen = me->SummonCreature(entry, summonVirmenPositions[urand(0, MAX_SUMMON_VIRMEN_POSITIONS - 1)], TEMPSUMMON_DEAD_DESPAWN);
                    if(virmen)
                        virmen->GetMotionMaster()->MoveJump(jumpVirmenPositions[urand(0, MAX_JUMP_VIRMEN_POSITIONS - 1)], 1.0f, 1.0f);
                }
            }
        }

        void DoPreEvent(const uint32 uiDiff)
        {
            if(instance && instance->GetData(Data_HoptallusEventProgress) == NOT_STARTED && !b_hasStartedOnce)
            {
                if(m_uiSummonTimer <= diff)
                {
                    Creature* add = me->SummonCreature(preEventAdds[urand(0, MAX_PRE_EVENT_ADDS - 1)], preEventAddsPaths[0]);
                    if(add)
                        add->GetMotionMaster()->MovePoint(1, preEventAddsPaths[1]);

                    m_uiSummonTimer = 1000 ;
                }
                else
                    m_uiSummonTimer = 1000 ;
            }
        }

        void DespawnCreatures(uint32 entry)
        {
            std::list<Creature*> creatures ;
            GetCreatureListWithEntryInGrid(creatures, me, entry, 50000.0f);

            if(!creatures.empty())
            {
                for(std::list<Creature*>::const_iterator iter = creatures.begin() ; iter != creatures.end() ; ++iter)
                {
                    if(Creature* c = *iter)
                    {
                        c->DisappearAndDie();
                    }
                }
            }
        }

        inline void ClearZone()
        {
            DespawnCreatures(Mob_Hopper);
            DespawnCreatures(Mob_Hoppling);
            DespawnCreatures(Mob_Bopper);
            DespawnCreatures(Npc_BigOlHammer);
        }

    private :
        EventMap events ;
        InstanceScript* instance ;
        bool b_hasStartedOnce ;
        uint32 m_uiSummonTimer ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new boss_hoptallus_AIScript(creature);
    }
};

class mob_virmen : public CreatureScript
{
public :
    mob_virmen() : CreatureScript("mob_virmen")
    {

    }

    struct mob_virmen_AIScript : public ScriptedAI
    {
    public :
        mob_virmen_AIScript(Creature *creature) : ScriptedAI(creature)
        {

        }

        void Reset()
        {
            if(me->GetEntry() == Mob_Hopper)
            {
                target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                if(target)
                    me->GetMotionMaster()->MoveChase(target);
            }
            else
                target = NULL ;
        }

        void JustDied(Unit *killer)
        {
            if(me->GetEntry() == Mob_Bopper)
            {
                if(killer)
                {
                    Position pos ;
                    killer->GetPosition(&pos);
                    me->SummonCreature(Npc_BigOlHammer, pos);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(!UpdateVictim())
                return ;

            if(me->GetEntry() == Mob_Hopper)
            {
                if(target && me->getVictim())
                {
                    if(target != me->getVictim())
                    {
                        me->AddThreat(me->getVictim(), -me->getThreatManager().getThreat(me->getVictim()));
                        me->AddThreat(target, 1000000.0f);
                        me->GetMotionMaster()->MoveChase(target);
                    }
                    else
                    {
                        Position pos ;
                        target->GetPosition(&pos);
                        if(me->GetExactDist2d(&pos) <= 2.0f)
                        {
                            DoCast(Spell_Hopper_ExplosiveBrew);
                            if(me && me->isAlive())
                                me->Kill(me);
                        }
                    }
                }
            }
        }

    private :
        Unit* target ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_virmen_AIScript(creature);
    }
};

class mob_pre_event_add : public CreatureScript
{
public :
    mob_pre_event_add() : CreautreScript("mob_pre_event_add")
    {

    }

    struct mob_pre_event_add_AIScript : public ScriptedAI
    {
    public :
        mob_pre_event_add_AIScript(Creature* creature) : ScriptedAI(creature)
        {

        }

        void MovementInform(uint32 type, uint32 id)
        {
            if(id != 7)
                me->GetMotionMaster()->MovePoint(preEventAddsPaths[id + 1]);
            else
            {
                me->GetMotionMaster()->Clear(false);
                me->DisappearAndDie();
            }
        }
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new mob_pre_event_add_AIScript(creature);
    }
};

class npc_big_ol_hammer : public CreatureScript
{
public :
    npc_big_ol_hammer() : CreatureScript("npc_big_ol_hammer")
    {
        m_ullLastFailedPlayer = 0 ;
    }

    bool OnGossipHello(Player *player, Creature *me)
    {
        if(!player || !me || (player && player->GetGUID() == m_ullLastFailedPlayer))
            return false ;

        if(!player->HasAura(Spell_Hammer_SmashOverrider))
        {
            me->CastSpell(player, Spell_Hammer_SmashOverrider, true);
            me->DisappearAndDie();
        }
        else
        {
            player->PlayerTalkClass->SendCloseGossip();
            m_ullLastFailedPlayer = player->GetGUID();
        }

        return true ;
    }

private :
    uint64 m_ullLastFailedPlayer ;
};

class npc_carrot_breath_stalker : public CreatureScript
{
public :
    npc_carrot_breath_stalker() : CreatureScript("npc_carrot_breath_stalker")
    {

    }

    struct npc_carrot_breath_stalker_AIScript : public ScriptedAI
    {
    public :
        npc_carrot_breath_stalker_AIScript(Creature* creature) : ScriptedAI(creature)
        {

        }

        void IsSummonedBy(Unit *summoner)
        {
            SpellInfo const* carrotBreathSpellInfo = sSpellMgr->GetSpellInfo(112944);

            if(summoner && carrotBreathSpellInfo)
            {
                me->SetFacingToObject(summoner);
                me->SetTarget(summoner->GetGUID());

                me->SetSpeed(MOVE_RUN, 2 * M_PI * m_fRayon / ((carrotBreathSpellInfo->DurationEntry->Duration)));

                m_posHoptallus.Relocate(summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());
                m_fRayon = me->GetExactDist2d(summoner->GetPositionX(), summoner->GetPositionY());
                m_fAngle = me->GetOrientation();
                m_uiPoint = 0 ;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if(SpellInfo const* carrotBreathSpellInfo = sSpellMgr->GetSpellInfo(112944))
            {
                ++m_uiPoint;
                m_fAngle -= ((2 * M_PI / (carrotBreathSpellInfo->DurationEntry->Duration)) * uiDiff );
                m_fx = m_posHoptallus.GetPositionX() + m_fRayon * cos(m_fAngle);
                m_fy = m_posHoptallus.GetPositionY() + m_fRayon * sin(m_fAngle);
                m_fz = m_posHoptallus.GetPositionZ() ;

                me->GetMotionMaster()->MovePoint(m_uiPoint, m_fx, m_fy, m_fz);
            }
        }

    private :
        float m_fx, m_fy, m_fz, m_fo, m_fRayon, m_fAngle ;
        uint32 m_uiPoint ;

        Position m_posHoptallus ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_carrot_breath_stalker_AIScript(creature);
    }
};

class spell_hoptallus_carrot_breath : public SpellScriptLoader
{
public:
    spell_hoptallus_carrot_breath() : SpellScriptLoader("spell_hoptallus_carrot_breath")
    {

    }

    class spell_hoptallus_carrot_breath_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hoptallus_carrot_breath_AuraScript)

        bool Validate(const SpellInfo *spellInfo)
        {
            return true ;
        }

        bool Load()
        {
            return true ;
        }

        void handleMiscOnEffectApply(AuraEffect const* auraEff, AuraEffectHandleModes mode)
        {
            caster = GetCaster();
            if(caster)
            {
                victim = caster->getVictim();

                caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                Position summoningPosition ;
                summoningPosition.Relocate(caster->GetPositionX() + SUMMONING_RAYON * cos(caster->GetOrientation()),
                                           caster->GetPositionY() + SUMMONING_RAYON * sin(caster->GetOrientation()),
                                           caster->GetPositionZ());

                Creature* stalker = caster->SummonCreature(Npc_CarrotBreathStalker, summoningPosition, TEMPSUMMON_TIMED_DESPAWN, 15000, 0);
                if(stalker)
                {
                    caster->SetTarget(stalker->GetGUID());
                    caster->SetFacingToObject(stalker);
                }
            }

        }

        void handleMiscOnEffectRemove(AuraEffect const* auraEff, AuraEffectHandleModes mode)
        {
            if(caster)
            {
                caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                if(victim)
                {
                    caster->SetTarget(victim->GetGUID());
                    caster->SetFacingToObject(victim);
                    caster->GetAI()->AttackStart(victim);
                }
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_hoptallus_carrot_breath_AuraScript::handleMiscOnEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_hoptallus_carrot_breath_AuraScript::handleMiscOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }

        Unit* caster ;
        Unit* victim ;
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
    new mob_pre_event_add();
    new npc_big_ol_hammer();
    new npc_carrot_breath_stalker();
    new spell_hoptallus_carrot_breath();
}
