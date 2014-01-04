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
    SPELL_SPIRITI_GALE_AURA         = 115289,
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
            me->CastSpell(me, SPELL_COSMETIC_VISUAL);

            if (instance)
                instance->SetBossState(DATA_BOSS_THALNOS_THE_SOULRENDER, NOT_STARTED);
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

            ScriptedAI::EnterEvadeMode();
        }

        void KilledUnit(Unit* /*pWho*/)
        {
            Talk(SAY_KILL);
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

                if (TempSummon* temp = me->ToTempSummon())
                {
                    if (Unit* summoner = temp->GetSummoner())
                    {
                        me->SetDisplayId(summoner->GetDisplayId());
                    }
                }
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

            uint32 position_Timer;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                me->CastSpell(me, SPELL_AOE);
                position_Timer = 500;
            }

            void UpdateAI(uint32 diff)
            {
                if (position_Timer <= diff)
                {
                    if(me->GetMap())
                    {
                        Map::PlayerList const & playerList = me->GetMap()->GetPlayers();
                        if(!playerList.isEmpty())
                        {
                            for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                            {
                                if(Player* player =iter->getSource())
                                {
                                    Position pos ;
                                    player->GetPosition(&pos);
                                    if(me->GetExactDist2d(&pos) <= 2.0f)
                                    {
                                        if(player->HasAura(SPELL_SPIRITI_GALE_AURA))
                                            continue ;
                                        else
                                            DoCast(player, SPELL_SPIRITI_GALE_AURA, true);
                                    }
                                    else
                                    {
                                        if(player->HasAura(SPELL_SPIRITI_GALE_AURA))
                                            player->RemoveAurasDueToSpell(SPELL_SPIRITI_GALE_AURA);
                                    }
                                }
                            }
                        }
                    }
                    position_Timer = 500;
                }
                else position_Timer -= diff;
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


void AddSC_boss_thalnos_the_soulrender()
{
    new boss_thalnos_the_soulrender();
    new npc_fallen_crusader();
    new npc_evicted_soul();
    new npc_empowering_spirit();
    new npc_traqueur_thalnos();
    new spell_spirit_gale();
}
