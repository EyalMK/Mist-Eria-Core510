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


/* High = Correction Rez masse + stop cast sinon nikel */
/* Durrand = Corriger cest 2 spell + attaque au sol */

enum Spells
{
    /* Commander Durand */
    SPELL_FLASH_OF_STEEL            = 115629,
    SPELL_DASHING_STRIKE            = 115739,
    SPELL_FURIOUS_RESOLVE           = 115876,

    /* Inquisitor Whitemane */
    SPELL_POWER_WORD_SHIELD         = 127399,
    SPELL_HOLY_SMITE                = 114848,
    SPELL_MASS_RESURRECTION         = 113134,
    SPELL_DEEP_SLEEP                = 9256,
    SPELL_SCARLET_RESURRECTION      = 9232
};


enum Events
{
    /* Commander Durand */
    EVENT_FLASH_OF_STEEL            = 1,
    EVENT_FLASH_OF_STEEL_TEST       = 2,
    EVENT_DASHING_STRIKE            = 3,
    EVENT_FURIOUS_RESOLVE           = 4,

    /* Inquisitor Whitemane */
    EVENT_POWER_WORD_SHIELD         = 5,
    EVENT_HOLY_SMITE                = 6,
    EVENT_MASS_RESURRECTION         = 7,
    EVENT_DEEP_SLEEP                = 8,
    EVENT_SCARLET_RESURRECTION      = 9
};


enum Texts_Durand
{
    /* Commander Durand */
    SAY_AGGRO_DURAND                = 0,
    SAY_DEATH_DURAND                = 1,
    SAY_KILL_DURAND                 = 2,
    SAY_INTRO_DURAND                = 3

};

enum Texts_Whitemane
{
    /* Inquisitor Whitemane */
    SAY_AGGRO_WHITEMANE             = 0,
    SAY_DEATH_WHITEMANE             = 1,
    SAY_KILL_WHITEMANE              = 2,
    SAY_RESSURECTION_WHITEMANE      = 3
};


enum Actions
{
    ACTION_INTRO    = 1
};

class at_commander_durand_intro : public AreaTriggerScript
{
    public:
        at_commander_durand_intro() : AreaTriggerScript("at_commander_durand_intro") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* durand = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_BOSS_COMMANDER_DURAND)))
                    durand->AI()->DoAction(ACTION_INTRO);
            return true;
        }
};


class boss_commander_durand : public CreatureScript
{
public:
    boss_commander_durand() : CreatureScript("boss_commander_durand") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_commander_durandAI(creature);
    }

    struct boss_commander_durandAI : public ScriptedAI
    {
        boss_commander_durandAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            Intro = false;
        }

        bool CheckDurand;
        bool Intro;
        uint32 FlashSteel;
        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();
            CheckDurand = true;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetStandState(UNIT_STAND_STATE_STAND);

            if (instance)
            {
                instance->SetBossState(DATA_BOSS_COMMANDER_DURAND, NOT_STARTED);

                if (Creature* whitemane = me->GetCreature(*me, instance->GetData64(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE)))
                {
                    if(!whitemane->isAlive())
                    {
                        whitemane->RemoveCorpse();
                        whitemane->Respawn();
                    }
                }
            }
        }

        void DoAction(int32 action)
        {
            switch (action)
            {
                case ACTION_INTRO:
                    if (!Intro)
                    {
                        Talk(SAY_INTRO_DURAND);
                        Intro = true;
                    }
                    break;
            }
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO_DURAND);

            if (instance)
                instance->SetBossState(DATA_BOSS_COMMANDER_DURAND, IN_PROGRESS);

            events.ScheduleEvent(EVENT_FLASH_OF_STEEL_TEST, 9*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_DASHING_STRIKE, 25*IN_MILLISECONDS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_COMMANDER_DURAND, FAIL);

            ScriptedAI::EnterEvadeMode();
        }

        void KilledUnit(Unit* /*pWho*/)
        {
            Talk(SAY_KILL_DURAND);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_COMMANDER_DURAND, DONE);

            if(Creature* whitemane = me->GetCreature(*me, instance->GetData64(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE)))
                if(whitemane->isAlive())
                    me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        void DamageTaken(Unit* /*doneBy*/, uint32 &damage)
        {
            if (damage < me->GetHealth())
                return;

            if (!instance)
                return;

            if (damage >= me->GetHealth() && CheckDurand)
            {
                damage = 0;
                me->InterruptNonMeleeSpells(false);
                me->SetHealth(0);
                me->StopMoving();
                me->ClearComboPointHolders();
                me->RemoveAllAurasOnDeath();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->ClearAllReactives();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
                me->SetStandState(UNIT_STAND_STATE_DEAD);

                if (instance)
                    instance->SetBossState(DATA_BOSS_COMMANDER_DURAND, SPECIAL);

                if (Unit* Whitemane = Unit::GetUnit(*me, instance->GetData64(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE)))
                {
                    Whitemane->GetMotionMaster()->MovePoint(1, 747.77f, 602.39f, 16.00f);
                }
                CheckDurand = false;
            }
        }

        void SpellHit(Unit* /*who*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SCARLET_RESURRECTION)
            {
                events.ScheduleEvent(EVENT_FURIOUS_RESOLVE, 1*IN_MILLISECONDS);
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
                        case EVENT_FLASH_OF_STEEL_TEST:
                            FlashSteel = 0;
                            events.ScheduleEvent(EVENT_FLASH_OF_STEEL, 1*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_FLASH_OF_STEEL_TEST, 25*IN_MILLISECONDS);
                            break;
                        case EVENT_FLASH_OF_STEEL:
                            if(FlashSteel < 5)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                {
                                    DoCast(target, SPELL_FLASH_OF_STEEL);
                                    FlashSteel += 1;
                                    events.ScheduleEvent(EVENT_FLASH_OF_STEEL, 1*IN_MILLISECONDS);
                                }
                            }
                            else
                               events.CancelEvent(EVENT_FLASH_OF_STEEL);
                            break;

                        case EVENT_DASHING_STRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                me->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 10.0f);
                                if(Map* map = me->GetMap())
                                {
                                    Map::PlayerList const & playerList = map->GetPlayers();
                                    if(!playerList.isEmpty())
                                    {
                                        for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                                        {
                                            if(Player* p = iter->getSource())
                                            {
                                                if(p->FindNearestCreature(NPC_COMMANDER_DURAND, 2.0f, true))
                                                    p->CastSpell(p, SPELL_DASHING_STRIKE, true);
                                            }
                                        }
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_DASHING_STRIKE, 25*IN_MILLISECONDS);
                            break;


                        case EVENT_FURIOUS_RESOLVE:
                            DoCast(SPELL_FURIOUS_RESOLVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            me->SetReactState(REACT_AGGRESSIVE);

                            events.ScheduleEvent(EVENT_DASHING_STRIKE, 25*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_FLASH_OF_STEEL_TEST, 10*IN_MILLISECONDS);
                            events.CancelEvent(EVENT_FURIOUS_RESOLVE);
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

class boss_high_inquisitor_whitemane : public CreatureScript
{
public:
    boss_high_inquisitor_whitemane() : CreatureScript("boss_high_inquisitor_whitemane") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_inquisitor_whitemaneAI(creature);
    }

    struct boss_high_inquisitor_whitemaneAI : public ScriptedAI
    {
        boss_high_inquisitor_whitemaneAI(Creature* creature) : ScriptedAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        bool CheckWhitemane;
        bool CheckWhitemane2;
        InstanceScript* instance;
        SummonList Summons;
        EventMap events;

        void Reset()
        {
            events.Reset();
            Summons.DespawnAll();
            CheckWhitemane = true;
            CheckWhitemane2 = true;

            if (instance)
            {
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, NOT_STARTED);

                if (Creature* durand = me->GetCreature(*me, instance->GetData64(DATA_BOSS_COMMANDER_DURAND)))
                {
                    if(!durand->isAlive())
                    {
                        durand->RemoveCorpse();
                        durand->Respawn();
                    }
                }
            }
        }


        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO_WHITEMANE);

            if (instance)
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, IN_PROGRESS);

            events.ScheduleEvent(EVENT_POWER_WORD_SHIELD, 2*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_HOLY_SMITE, 4*IN_MILLISECONDS);
        }


        void EnterEvadeMode()
        {
            if (instance)
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, FAIL);
            ScriptedAI::EnterEvadeMode();

            me->NearTeleportTo(700.40f, 605.83f, 12.00f, 0);
        }

        void KilledUnit(Unit* /*pWho*/)
        {
            Talk(SAY_KILL_WHITEMANE);
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(SAY_DEATH_WHITEMANE);

            if (instance)
                instance->SetBossState(DATA_BOSS_HIGH_INQUISITOR_WHITEMANE, DONE);

            if(Creature* durand = me->GetCreature(*me, instance->GetData64(DATA_BOSS_COMMANDER_DURAND)))
                if(durand->isAlive())
                    me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        void DamageTaken(Unit* /*doneBy*/, uint32 &damage)
        {
            if (damage < me->GetHealth())
                return;

            if (!instance)
                return;

            if (damage >= me->GetHealth() && CheckWhitemane2)
            {
                damage = 0;
                CheckWhitemane2 = false;
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                for (uint8 i = 0; i < 7; ++i)
                    if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                        if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_HOLY_SMITE || me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_MASS_RESURRECTION)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            events.Update(diff);

            if(me->HealthBelowPct(50) && CheckWhitemane)
            {
                events.ScheduleEvent(EVENT_DEEP_SLEEP, 1*IN_MILLISECONDS);
                CheckWhitemane = false;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    if (instance)
                    {
                        case EVENT_POWER_WORD_SHIELD:
                            DoCast(me, SPELL_POWER_WORD_SHIELD);
                            events.ScheduleEvent(EVENT_POWER_WORD_SHIELD, 20*IN_MILLISECONDS);
                            break;

                        case EVENT_HOLY_SMITE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                DoCast(target, SPELL_HOLY_SMITE);
                            }
                            events.ScheduleEvent(EVENT_HOLY_SMITE, 3*IN_MILLISECONDS);
                            break;

                        case EVENT_MASS_RESURRECTION:
                            DoCast(SPELL_MASS_RESURRECTION);
                            events.ScheduleEvent(EVENT_MASS_RESURRECTION, 60*IN_MILLISECONDS);
                            break;

                        case EVENT_DEEP_SLEEP:
                            events.CancelEvent(EVENT_POWER_WORD_SHIELD);
                            events.CancelEvent(EVENT_HOLY_SMITE);

                            if(Map* map = me->GetMap())
                            {
                                Map::PlayerList const & playerList = map->GetPlayers();
                                if(!playerList.isEmpty())
                                {
                                    for(Map::PlayerList::const_iterator iter = playerList.begin() ; iter != playerList.end() ; ++iter)
                                    {
                                        if(Player* player = iter->getSource())
                                        {
                                            me->CastSpell(player, SPELL_DEEP_SLEEP);
                                        }
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_SCARLET_RESURRECTION, 1*IN_MILLISECONDS);
                            break;

                        case EVENT_SCARLET_RESURRECTION:
                            if (Unit* Durand = Unit::GetUnit(*me, instance->GetData64(DATA_BOSS_COMMANDER_DURAND)))
                            {
                                me->CastSpell(Durand, SPELL_SCARLET_RESURRECTION);
                                Talk(SAY_RESSURECTION_WHITEMANE);
                            }
                            me->SetHealth(me->GetMaxHealth());
                            events.ScheduleEvent(EVENT_POWER_WORD_SHIELD, 2*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_HOLY_SMITE, 4*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_MASS_RESURRECTION, 60*IN_MILLISECONDS);
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

void AddSC_boss_high_inquisitor_whitemane()
{
    new boss_commander_durand();
    new boss_high_inquisitor_whitemane();
    new at_commander_durand_intro();
}
