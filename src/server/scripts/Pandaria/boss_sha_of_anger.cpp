/*
Notes :
Sha de la col�re : Script 75%	=>	A faire : v�rifier si les sorts fonctionnent.

UPDATE creature_template SET ScriptName = 'boss_sha_of_anger' WHERE entry = 60491;
INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(60491, 0, 0, "Oui ... Oui ! Laissez parler votre rage ! Frappez-moi !", 14, 0, 100, 0, 0, 28999, "Sha of anger - Aggro"),
(60491, 1, 0, "", 14, 0, 100, 0, 0, 29000, "Sha of anger - Death"),
(60491, 2, 0, "Ils sont �teinds !", 14, 0, 100, 0, 0, 29001, "Sha of anger - Slay 1"),
(60491, 2, 1, "Est-ce que vous �tes en col�re ?", 14, 0, 100, 0, 0, 29002, "Sha of anger - Slay 2"),
(60491, 2, 2, "Ressentez votre rage !", 14, 0, 100, 0, 0, 29003, "Sha of anger - Slay 3"),
(60491, 2, 3, "Laissez votre rage vous consumer !", 14, 0, 100, 0, 0, 29004, "Sha of anger - Slay 4"),
(60491, 3, 0, "C�dez a votre col�re !", 14, 0, 100, 0, 0, 29005, "Sha of anger - Spawn 1"),
(60491, 3, 1, "Votre rage vous donne de la force !", 14, 0, 100, 0, 0, 29006, "Sha of anger - Spawn 2"),
(60491, 3, 2, "Votre rage me porte !", 14, 0, 100, 0, 0, 29007, "Sha of anger - Spawn 3"),
(60491, 3, 3, "Vous ne m'enterrerez pas � nouveau !", 14, 0, 100, 0, 0, 29008, "Sha of anger - Spawn 4"),
(60491, 3, 4, "Laissez libre cours � mon courroux !", 14, 0, 100, 0, 0, 29009, "Sha of anger - Spawn 5"),
(60491, 4, 0, "Nourissez-moi de votre COL�RE !", 14,� 0, 100, 0, 0, 29010, "Sha of anger - Spell 1"),
(60491, 5, 0, "MA FUREUR SE D�CHA�NE !", 14, 0, 100, 0, 0, 29011, "Sha of anger - Spell 2");
*/

#include "ScriptPCH.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"

enum Spells
{
    SPELL_SEETHE				= 119487,
    SPELL_ENDLESS_RAGE			= 119586,
    SPELL_BITTER_THOUGHTS		= 119601,
    SPELL_GROWING_ANGER			= 119622,
    SPELL_AGGRESSIVE_BEHAVIOUR	= 119626,
    SPELL_UNLEASHED_WRATH		= 119488,
    SPELL_RAGE_OF_THE_SHA       = 117609,
    SPELL_BERSERK               = 47008
};

enum Events
{
    EVENT_SEETHE				= 1,
    EVENT_ENDLESS_RAGE			= 2,
    EVENT_GROWING_ANGER			= 3,
    EVENT_PHASE_GROWING_ANGER	= 4,
    EVENT_UNLEASHED_WRATH       = 5,
    EVENT_BERSERK               = 6
};

enum Phases
{
    PHASE_GROWING_ANGER		= 1,
    PHASE_UNLEASHED_WRATH	= 2
};

enum Texts
{
    SAY_AGGRO				= 0,
    SAY_DEATH				= 1,
    SAY_SLAY				= 2,
    SAY_ENDLESS_RAGE		= 3,
    SAY_GROWING_ANGER		= 4,
    SAY_UNLEASHED_WRATH		= 5
};

enum Creatures
{
    NPC_SHA                 = 60491,
    NPC_IRE                 = 60579,
    NPC_SHA_OF_ANGER        = 61523
};


class boss_sha_of_anger : public CreatureScript
{
public:
    boss_sha_of_anger() : CreatureScript("boss_sha_of_anger") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_sha_of_angerAI(creature);
    }

    struct boss_sha_of_angerAI : public ScriptedAI
    {
        boss_sha_of_angerAI(Creature *creature) : ScriptedAI(creature), Summons(me)
        {
        }

        EventMap events;
        SummonList Summons;

        void Reset()
        {
            events.Reset();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            Summons.DespawnAll();
        }

        void JustDied(Unit* /*who*/)
        {
            Talk(SAY_DEATH);
            Summons.DespawnAll();
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);
        }

        void KilledUnit(Unit* /*who*/)
        {
            Talk(SAY_SLAY);
        }

        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            events.ScheduleEvent(EVENT_SEETHE, 2*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_ENDLESS_RAGE, 20*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_GROWING_ANGER, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS), 0, PHASE_GROWING_ANGER);
            events.ScheduleEvent(EVENT_UNLEASHED_WRATH, 50*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BERSERK, 900*IN_MILLISECONDS);

            events.SetPhase(PHASE_GROWING_ANGER);
        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim())
                return;

            ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
            ThreatContainer::StorageType::const_iterator i = threatlist.begin();

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SEETHE:
                        for (i = threatlist.begin(); i != threatlist.end(); ++i)
                        {
							if (!me->IsWithinMeleeRange((*i)->getTarget()->ToPlayer()))
								me->CastSpell(me->getVictim(), SPELL_SEETHE);
                        }

                        events.ScheduleEvent(EVENT_SEETHE, 2*IN_MILLISECONDS);
                        break;

                    case EVENT_ENDLESS_RAGE:
                        me->CastSpell(me->getVictim(), SPELL_ENDLESS_RAGE);
                        Talk(SAY_ENDLESS_RAGE);
                        break;

                    case EVENT_GROWING_ANGER:
                        me->CastSpell(me, SPELL_GROWING_ANGER);
                        Talk(SAY_GROWING_ANGER);
                        break;

                    case EVENT_PHASE_GROWING_ANGER:
                        events.SetPhase(PHASE_GROWING_ANGER);
                        events.ScheduleEvent(EVENT_ENDLESS_RAGE, 20*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_GROWING_ANGER, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                        events.ScheduleEvent(EVENT_UNLEASHED_WRATH, 50*IN_MILLISECONDS);
                        break;

                    case EVENT_UNLEASHED_WRATH:
                        DoCast(SPELL_UNLEASHED_WRATH);
                        events.SetPhase(PHASE_UNLEASHED_WRATH);
                        events.ScheduleEvent(EVENT_PHASE_GROWING_ANGER, 25*IN_MILLISECONDS, 0, PHASE_GROWING_ANGER);
                        events.ScheduleEvent(EVENT_ENDLESS_RAGE, 15*IN_MILLISECONDS);
                        break;

                    case EVENT_BERSERK:
                        DoCast(SPELL_BERSERK);
                        break;

                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_sha_of_anger : public CreatureScript
{
public:
    npc_sha_of_anger() : CreatureScript("npc_sha_of_anger") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sha_of_angerAI(creature);
    }

    struct npc_sha_of_angerAI : public ScriptedAI
    {
            npc_sha_of_angerAI(Creature* creature) : ScriptedAI(creature) {}

            void Reset()
            {
                me->CastSpell(me, SPELL_BITTER_THOUGHTS);
            }
    };
};

class spell_growing_anger : public SpellScriptLoader
{
    public:
        spell_growing_anger() : SpellScriptLoader("spell_growing_anger") { }

        class spell_growing_anger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_growing_anger_AuraScript);

            void HandleAfterEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                if(Creature* sha = target->FindNearestCreature(NPC_SHA, 100.0f))
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        sha->CastSpell(target, SPELL_AGGRESSIVE_BEHAVIOUR);

                        Map* map = target->GetMap();

                        Map::PlayerList const& players = map->GetPlayers();

                        for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                        {
                            Player* player = i->getSource();

                            if (player && player->IsInRange(target, 0.0f, 5.0f, false))
                                sha->CastSpell(player, SPELL_AGGRESSIVE_BEHAVIOUR);
                        }
                    }
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_growing_anger_AuraScript::HandleAfterEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_growing_anger_AuraScript();
        }
};

class spell_aggressive_behavior : public SpellScriptLoader
{
    public:
        spell_aggressive_behavior() : SpellScriptLoader("spell_aggressive_behavior") { }

        class spell_aggressive_behavior_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_aggressive_behavior_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetTarget())
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (target->HealthBelowPct(50))
                        {
                            target->RemoveAurasDueToSpell(SPELL_AGGRESSIVE_BEHAVIOUR);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_aggressive_behavior_AuraScript::HandleEffectPeriodic, EFFECT_5, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_aggressive_behavior_AuraScript();
        }
};

void AddSC_boss_sha_of_anger()
{
    new boss_sha_of_anger();
    new npc_sha_of_anger();
    new spell_growing_anger();
    new spell_aggressive_behavior();
};
