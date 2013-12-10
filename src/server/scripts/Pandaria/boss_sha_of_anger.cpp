#include "ScriptPCH.h"

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

            events.ScheduleEvent(EVENT_SEETHE, 2*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_ENDLESS_RAGE, 20*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_GROWING_ANGER, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS), 0, PHASE_GROWING_ANGER);
            events.ScheduleEvent(EVENT_UNLEASHED_WRATH, 50*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BERSERK, 300*IN_MILLISECONDS);

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
                            if (Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid()))
                                if (unit && (unit->GetTypeId() == TYPEID_PLAYER) && !me->IsWithinMeleeRange(me->getVictim()))
                                    me->CastSpell(me->getVictim(), SPELL_SEETHE);
                        }

                        events.ScheduleEvent(EVENT_SEETHE, 2*IN_MILLISECONDS);
                        break;

                    case EVENT_ENDLESS_RAGE:
                        me->CastSpell(me->getVictim(), SPELL_ENDLESS_RAGE);
                        Talk(SAY_ENDLESS_RAGE);
                       /* events.ScheduleEvent(EVENT_ENDLESS_RAGE, 25*IN_MILLISECONDS);*/
                        break;

                    case EVENT_GROWING_ANGER:
                        me->CastSpell(me, SPELL_GROWING_ANGER);
                        Talk(SAY_GROWING_ANGER);
                       /* events.ScheduleEvent(EVENT_GROWING_ANGER, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS));*/
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

void AddSC_boss_sha_of_anger()
{
    new boss_sha_of_anger();
    new npc_sha_of_anger();
};
