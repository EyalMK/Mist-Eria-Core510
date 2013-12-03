/*
Notes :
Sha de la colère : Script 75%	=>	A faire : vérifier si les sorts fonctionnent.

UPDATE creature_template SET ScriptName = 'boss_sha_of_anger' WHERE entry = 60491;
INSERT INTO creature_text (entry, groupid, id, text, type, language, probability, emote, duration, sound, comment) VALUES
(60491, 0, 0, "Oui ... Oui ! Laissez parler votre rage ! Frappez-moi !", 14, 0, 100, 0, 0, 28999, "Sha of anger - Aggro"),
(60491, 1, 0, "", 14, 0, 100, 0, 0, 29000, "Sha of anger - Death"),
(60491, 2, 0, "Ils sont éteinds !", 14, 0, 100, 0, 0, 29001, "Sha of anger - Slay 1"),
(60491, 2, 1, "Est-ce que vous êtes en colère ?", 14, 0, 100, 0, 0, 29002, "Sha of anger - Slay 2"),
(60491, 2, 2, "Ressentez votre rage !", 14, 0, 100, 0, 0, 29003, "Sha of anger - Slay 3"),
(60491, 2, 3, "Laissez votre rage vous consumer !", 14, 0, 100, 0, 0, 29004, "Sha of anger - Slay 4"),
(60491, 3, 0, "Cédez a votre colère !", 14, 0, 100, 0, 0, 29005, "Sha of anger - Spawn 1"),
(60491, 3, 1, "Votre rage vous donne de la force !", 14, 0, 100, 0, 0, 29006, "Sha of anger - Spawn 2"),
(60491, 3, 2, "Votre rage me porte !", 14, 0, 100, 0, 0, 29007, "Sha of anger - Spawn 3"),
(60491, 3, 3, "Vous ne m'enterrerez pas à nouveau !", 14, 0, 100, 0, 0, 29008, "Sha of anger - Spawn 4"),
(60491, 3, 4, "Laissez libre cours à mon courroux !", 14, 0, 100, 0, 0, 29009, "Sha of anger - Spawn 5"),
(60491, 4, 0, "Nourissez-moi de votre COLÈRE !", 14, 0, 100, 0, 0, 29010, "Sha of anger - Spell 1"),
(60491, 5, 0, "MA FUREUR SE DÉCHAÎNE !", 14, 0, 100, 0, 0, 29011, "Sha of anger - Spell 2");
*/

#include "ScriptPCH.h"

enum Spells
{
    SPELL_SEETHE				= 119487,
    SPELL_ENDLESS_RAGE			= 119586,
    SPELL_BITTER_THOUGHTS		= 119601,
    SPELL_GROWING_ANGER			= 119622,
    SPELL_AGGRESSIVE_BEHAVIOUR	= 119626,
    SPELL_UNLEASHED_WRATH		= 119488,
    SPELL_RAGE_OF_THE_SHA       = 117609
};

enum Events
{
    EVENT_SEETHE				= 1,
    EVENT_ENDLESS_RAGE			= 2,
    EVENT_GROWING_ANGER			= 3,
    EVENT_PHASE_GROWING_ANGER	= 4,
    EVENT_PHASE_UNLEASHED_WRATH = 5
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

#define NPC_IRE             60579
#define NPC_SHA_OF_ANGER    61523

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
        boss_sha_of_angerAI(Creature *creature) : ScriptedAI(creature)
        {
        }

        EventMap events;

        void Reset()
        {
            events.Reset();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        }

        void JustDied(Unit* /*who*/)
        {
            Talk(SAY_DEATH);
        }

        void KilledUnit(Unit* /*who*/)
        {
            Talk(SAY_SLAY);
        }

        void EnterCombat(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
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

            if (events.IsInPhase(PHASE_GROWING_ANGER))
            {
                events.ScheduleEvent(EVENT_SEETHE, 2*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENDLESS_RAGE, 25*IN_MILLISECONDS, 0, PHASE_GROWING_ANGER);
                events.ScheduleEvent(EVENT_GROWING_ANGER, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS), 0, PHASE_GROWING_ANGER);
                events.ScheduleEvent(EVENT_PHASE_UNLEASHED_WRATH, 51*IN_MILLISECONDS);
            }

            if (events.IsInPhase(PHASE_UNLEASHED_WRATH))
            {
                me->CastSpell(me, SPELL_UNLEASHED_WRATH);
                events.ScheduleEvent(EVENT_PHASE_GROWING_ANGER, 26*IN_MILLISECONDS);
            }

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SEETHE:
                        for (i = threatlist.begin(); i != threatlist.end(); ++i)
                        {
                            if (Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid()))
                                if (unit && (unit->GetTypeId() == TYPEID_PLAYER) && !me->IsWithinMeleeRange(me->getVictim()))
                                    me->CastSpell(me->getVictim(), SPELL_SEETHE);
                        }
                        events.ScheduleEvent(EVENT_SEETHE, 2*IN_MILLISECONDS);
                        break;

                    case EVENT_ENDLESS_RAGE:
                        me->CastSpell(me->getVictim(), SPELL_ENDLESS_RAGE);
                        Talk(SAY_ENDLESS_RAGE);
                        events.ScheduleEvent(EVENT_ENDLESS_RAGE, 25*IN_MILLISECONDS);
                        break;

                    case EVENT_GROWING_ANGER:
                        me->CastSpell(me, SPELL_GROWING_ANGER);
                        Talk(SAY_GROWING_ANGER);
                        events.ScheduleEvent(EVENT_GROWING_ANGER, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                        break;

                    case EVENT_PHASE_GROWING_ANGER:
                        events.SetPhase(PHASE_GROWING_ANGER);
                        break;

                    case EVENT_PHASE_UNLEASHED_WRATH:
						events.SetPhase(PHASE_UNLEASHED_WRATH);
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

        uint32 uiBitterThoughtsTimer;

        void Reset()
        {
            uiBitterThoughtsTimer = 1*IN_MILLISECONDS;
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (uiBitterThoughtsTimer <= diff)
                {
                    me->CastSpell(me, SPELL_BITTER_THOUGHTS);
                } else uiBitterThoughtsTimer -= diff;
            }
        }
    };
};

class npc_sha_of_anger_test : public CreatureScript
{
public:
    npc_sha_of_anger_test() : CreatureScript("npc_sha_of_anger_test") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sha_of_anger_testAI(creature);
    }

    struct npc_sha_of_anger_testAI : public ScriptedAI
    {
        npc_sha_of_anger_testAI(Creature* creature) : ScriptedAI(creature) {}

        EventMap events;

        void Reset()
        {
			events.Reset();
        }

		void EnterCombat(Unit* /*who*/)
        {
			events.ScheduleEvent(1, 1*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
				return;

			events.Update(diff);

			while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
					case 1:
						me->CastSpell(me->getVictim()->ToPlayer(), 119626);

						events.ScheduleEvent(1, 40*IN_MILLISECONDS);
						break;

                    default:
                        break;
                }
            }

			DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_sha_of_anger()
{
    new boss_sha_of_anger();
    new npc_sha_of_anger();
	new npc_sha_of_anger_test();
};