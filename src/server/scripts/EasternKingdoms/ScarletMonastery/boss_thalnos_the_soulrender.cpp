/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_monastery.h"
#include "SpellScript.h"
#include "Player.h"


enum Spells
{
    /* Thalnos */
    SPELL_RAISE_FALLEN_CRUSADER     = 115139,
    SPELL_SPIRIT_GALE               = 115289,
    SPELL_EVICT_SOUL                = 115297,
    SPELL_SUMMON_EMPOWERING_SPIRITS = 115147,
    SPELL_COSMETIC_VISUAL           = 126653,

    /* Autres */
    SPELL_MIND_ROT                  = 115143,
    SPELL_EVICTED_SOUL              = 115309,
    SPELL_EVICT_SOUL_NPC            = 115304,
    SPELL_SPIRIT_GALE_AURA          = 115291,
    SPELL_EMPOWERING_SPIRIT         = 115157,
    SPELL_EMPOWER_ZOMBIE_TRANSFORM  = 115250,
    SPELL_AOE                       = 115272,
    SPELL_SIPHON_ESSENCE            = 40291
};


enum Events
{
    EVENT_RAISE_FALLEN_CRUSADER         = 1,
    EVENT_SPIRIT_GALE                   = 2,
    EVENT_EVICT_SOUL                    = 3,
    EVENT_SUMMON_EMPOWERING_SPIRITS     = 4
};


enum Texts
{
    SAY_AGGRO                       = 0,
    SAY_DEATH                       = 1,
    SAY_KILL                        = 2,
    SAY_RAISE_FALLEN_CRUSADER       = 3,
    SAY_EVICT_SOUL                  = 4,
    SAY_SUMMON_EMPOWERING_SPIRITS   = 5
};

enum Creatures
{
    NPC_ZOMBIE                  = 59884,
    NPC_EVICTED_SOUL            = 59974,
    NPC_EMPOWERING_SPIRITS      = 59893,
    NPC_EMPOWERED_ZOMBIE        = 59930,
    NPC_THALNOS                 = 59789,
    NPC_TRAQUEUR_INVISIBLE      = 200011
};

class boss_thalnos_the_soulrender : public CreatureScript
{
public:
    boss_thalnos_the_soulrender() : CreatureScript("boss_thalnos_the_soulrender") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_thalnos_the_soulrenderAI(creature);
    }

    struct boss_thalnos_the_soulrenderAI : public ScriptedAI
    {
        boss_thalnos_the_soulrenderAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();
			Cleanup();
            me->CastSpell(me, SPELL_COSMETIC_VISUAL);

            if (instance)
                instance->SetBossState(DATA_BOSS_THALNOS_THE_SOULRENDER, NOT_STARTED);
        }

        void Cleanup()
        {
            std::list<Creature*> creatures;

            GetCreatureListWithEntryInGrid(creatures, me, NPC_EVICTED_SOUL, 200.0f);
            for (std::list<Creature*>::iterator itr = creatures.begin(); itr != creatures.end(); ++itr)
                (*itr)->DespawnOrUnsummon();
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetBossState(DATA_BOSS_THALNOS_THE_SOULRENDER, IN_PROGRESS);

            events.ScheduleEvent(EVENT_RAISE_FALLEN_CRUSADER, 5*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SPIRIT_GALE, 12*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_EVICT_SOUL, 32*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SUMMON_EMPOWERING_SPIRITS, 50*IN_MILLISECONDS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_THALNOS_THE_SOULRENDER, FAIL);

            Cleanup();
            ScriptedAI::EnterEvadeMode();
        }

        void KilledUnit(Unit* /*pWho*/)
        {
            Talk(SAY_KILL);
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                for (uint8 i = 0; i < 7; ++i)
                    if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                        if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_SPIRIT_GALE)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
        }


        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH);
            Summons.DespawnAll();

            if (instance)
                instance->SetBossState(DATA_BOSS_THALNOS_THE_SOULRENDER, DONE);
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);

            if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
            {
                if(target && target->GetTypeId() == TYPEID_PLAYER)
                {
                    switch (Summoned->GetEntry())
                    {
                        case NPC_ZOMBIE:
                            Summoned->AI()->AttackStart(target);
                            break;
                        case NPC_EVICTED_SOUL :
                            Summoned->AI()->AttackStart(target);
                            break;
                        case NPC_EMPOWERING_SPIRITS:
                            Summoned->AI()->AttackStart(target);
                            break;
                        case NPC_EMPOWERED_ZOMBIE:
                            Summoned->AI()->AttackStart(target);
                            break;
                    }
                }
            }
        }


        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    if (instance)
                    {
                        case EVENT_RAISE_FALLEN_CRUSADER:                           
                            Talk(SAY_RAISE_FALLEN_CRUSADER);
                            DoCast(SPELL_RAISE_FALLEN_CRUSADER);
                            events.ScheduleEvent(EVENT_RAISE_FALLEN_CRUSADER, 60*IN_MILLISECONDS);
                            break;

                        case EVENT_SPIRIT_GALE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                DoCast(target, SPELL_SPIRIT_GALE);
                            }
                            events.ScheduleEvent(EVENT_SPIRIT_GALE, 8*IN_MILLISECONDS);
                            break;

                        case EVENT_EVICT_SOUL:
                            Talk(SAY_EVICT_SOUL);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                DoCast(target, SPELL_EVICT_SOUL);
                            }
                            events.ScheduleEvent(EVENT_EVICT_SOUL, 60*IN_MILLISECONDS);
                            break;

                        case EVENT_SUMMON_EMPOWERING_SPIRITS:
                            events.CancelEvent(EVENT_SPIRIT_GALE);
                            Talk(SAY_SUMMON_EMPOWERING_SPIRITS);
                            DoCast(SPELL_SUMMON_EMPOWERING_SPIRITS);
                            events.ScheduleEvent(EVENT_SUMMON_EMPOWERING_SPIRITS, 60*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_SPIRIT_GALE, 22*IN_MILLISECONDS);
                            break;

                        default:
                            break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};


class npc_fallen_crusader : public CreatureScript
{
public:
    npc_fallen_crusader() : CreatureScript("npc_fallen_crusader") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fallen_crusaderAI(creature);
    }

    struct npc_fallen_crusaderAI : public ScriptedAI
    {
            npc_fallen_crusaderAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
            }

            void EnterCombat(Unit* /*who*/)
            {
                me->CastSpell(me, SPELL_MIND_ROT);
            }
    };
};


class npc_evicted_soul : public CreatureScript
{
public:
    npc_evicted_soul() : CreatureScript("npc_evicted_soul") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_evicted_soulAI(creature);
    }

    struct npc_evicted_soulAI : public ScriptedAI
    {
            npc_evicted_soulAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 SiphonEssence_Timer;

            void Reset()
            {
                SiphonEssence_Timer = 2000;
            }

            void EnterCombat(Unit* /*who*/)
            {
                me->CastSpell(me, SPELL_EVICTED_SOUL);
            }

            void UpdateAI(uint32 diff)
            {
                if(!UpdateVictim())
                    return;

                if (IsHeroic())
                {
                    if (SiphonEssence_Timer <= diff)
                    {
                        me->CastSpell(me->getVictim(), SPELL_SIPHON_ESSENCE);
                        SiphonEssence_Timer = 8000;
                    }
                    else SiphonEssence_Timer -= diff;
                }

                DoMeleeAttackIfReady();
            }
    };
};

class npc_empowering_spirit : public CreatureScript
{
public:
    npc_empowering_spirit() : CreatureScript("npc_empowering_spirit") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_empowering_spiritAI(creature);
    }

    struct npc_empowering_spiritAI : public ScriptedAI
    {
            npc_empowering_spiritAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Test_timer;
            void Reset()
            {
                Test_timer = 1000;
            }

            void EnterCombat(Unit* /*who*/)
            {
                me->CastSpell(me, SPELL_EMPOWERING_SPIRIT);
            }

            void UpdateAI(uint32 diff)
            {
                if(!UpdateVictim())
                    return;

                if (Test_timer <= diff)
                {
                    if(Creature* thalnos = me->FindNearestCreature(NPC_THALNOS, 100.0f))
                    {
                        if(me->FindNearestCreature(NPC_ZOMBIE, 100.0f, false))
                        {
                            thalnos->CastSpell(me, SPELL_EMPOWER_ZOMBIE_TRANSFORM, true);
                            me->DisappearAndDie();
                        }
                    }
                    else
                        Test_timer = 1000;
                }
                else Test_timer -= diff;

                DoMeleeAttackIfReady();
            }
    };
};

class npc_traqueur_verification : public CreatureScript
{
public:
    npc_traqueur_verification() : CreatureScript("npc_traqueur_verification") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_traqueur_verificationAI(creature);
    }

    struct npc_traqueur_verificationAI : public ScriptedAI
    {
           npc_traqueur_verificationAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 m_uiCheckTimer;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                m_uiCheckTimer = 250;
            }

            void UpdateAI(uint32 diff)
            {
                if(m_uiCheckTimer <= diff)
                {
                    DoCheckPlayers();
                    m_uiCheckTimer = 250;
                }
                else
                    m_uiCheckTimer -= diff;

            }

            void DoCheckPlayers()
            {
                std::list<Creature*> stalkers ;
                GetCreatureListWithEntryInGrid(stalkers, me, NPC_TRAQUEUR_INVISIBLE, 50000.0f);
                if(Map* map = me->GetMap())
                {
                    Map::PlayerList const & playerList = map->GetPlayers();
                    if(!playerList.isEmpty())
                    {
                        for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                        {
                            if(Player* p = iter->getSource())
                            {
                                if(DoCheckPlayer(p, stalkers))
                                {
                                    if(!p->HasAura(SPELL_SPIRIT_GALE_AURA))
                                        p->CastSpell(p, SPELL_SPIRIT_GALE_AURA, true);
                                }
                                else
                                    p->RemoveAurasDueToSpell(SPELL_SPIRIT_GALE_AURA);
                             }
                        }
                    }
                }
            }

            bool DoCheckPlayer(Player *player, std::list<Creature*> const& stalkersList)
            {
                if(stalkersList.empty())
                    return false ;

                for(std::list<Creature*>::const_iterator iter = stalkersList.begin() ; iter != stalkersList.end() ; ++iter)
                {
                    Position pos ;
                    if(Creature* stalker = *iter)
                    {
                        stalker->GetPosition(&pos);
                        if(player->GetExactDist2d(&pos) <= 2.0f)
                            return true ;
                        else
                            continue ;
                     }
                }
                return false ;
            }
    };
};

class npc_traqueur_thalnos : public CreatureScript
{
public:
    npc_traqueur_thalnos() : CreatureScript("npc_traqueur_thalnos") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_traqueur_thalnosAI(creature);
    }

    struct npc_traqueur_thalnosAI : public ScriptedAI
    {
            npc_traqueur_thalnosAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->CastSpell(me, SPELL_AOE);
            }
    };
};


class spell_spirit_gale : public SpellScriptLoader
{
    public:
        spell_spirit_gale() : SpellScriptLoader("spell_spirit_gale") { }

        class spell_spirit_gale_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spirit_gale_SpellScript);

            void SummonTraqueurFlaque(SpellEffIndex /*effIndex*/)
            {                     
                if (Unit* target = GetHitPlayer())
                {                  
                    if(Creature* thalnos = target->FindNearestCreature(NPC_THALNOS, 100.0f))
                    {
                        if (target->GetTypeId() == TYPEID_PLAYER)
                        {
                            thalnos->SummonCreature(NPC_TRAQUEUR_INVISIBLE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_spirit_gale_SpellScript::SummonTraqueurFlaque, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_spirit_gale_SpellScript();
        }
};


class spell_evict_soul : public SpellScriptLoader
{
    public:
        spell_evict_soul() : SpellScriptLoader("spell_evict_soul") { }

        class spell_evict_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_evict_soul_AuraScript);

            void handleSetDisplayIdOnEffectPeriodic(AuraEffect const* /*auraEff*/)
            {
                PreventDefaultAction();
                if(Unit* player = GetUnitOwner())
                {
                    if(player->GetTypeId() == TYPEID_PLAYER)
                    {
                        if(Creature * summon = player->SummonCreature(NPC_EVICTED_SOUL, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            summon->SetDisplayId(player->GetDisplayId());
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_evict_soul_AuraScript::handleSetDisplayIdOnEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_evict_soul_AuraScript();
        }
};



void AddSC_boss_thalnos_the_soulrender()
{
    new boss_thalnos_the_soulrender();
    new npc_fallen_crusader();
    new npc_evicted_soul();
    new npc_empowering_spirit();
    new npc_traqueur_verification();
    new npc_traqueur_thalnos();
    new spell_spirit_gale();
    new spell_evict_soul();
}
