#include "stormstout_brewery.h"

enum OokOokSpells
{
    SPELL_GROUND_POUND  = 106807,
    SPELL_GOING_BANANAS = 106651
};

enum Actions
{
    ACTION_JUMP = 0
};

enum Events
{
    EVENT_GROUND_POUND  = 1,
    EVENT_CHECK         = 2
};

enum Misc
{

};

enum Talk
{
    SAY_JUMP                = 1,
    SAY_ENTER_COMBAT        = 2,
    SAY_GOING_BANANAS_ONE   = 3,
    SAY_GOING_BANANAS_TWO   = 4,
    SAY_GOING_BANANAS_THREE = 5,
    SAY_KILLED_PLAYER       = 6,
    SAY_JUST_DIED           = 7,
    SAY_SUMMON_ALL_BARRELS  = 8
};

const Position jumpPosition = {-756.727173f, 1354.609985f, 147.095245f, 2.085622f};

class boss_ook_ook : public CreatureScript
{
public :
    boss_ook_ook() : CreatureScript("boss_ook_ook")
    {

    }

    class boss_ook_ookAI : public ScriptedAI
    {
    public :
        boss_ook_ookAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            m_bHasActivated = false ;
            m_bReady = false ;
        }

        void Reset()
        {
            if(instance)
                instance->SetData(INSTANCE_DATA_OOK_OOK_STATUS, NOT_STARTED);
            events.Reset();
            if(m_bHasActivated)
                events.ScheduleEvent(EVENT_CHECK, 500);
            m_uiNextBananaPercent = 90 ;
        }

        void DoAction(const int32 action)
        {
            if(action == 0)
            {
                Talk(SAY_JUMP);
                me->GetMotionMaster()->MoveJump(jumpPosition, 10.0f, 10.0f);
				DoSlayHozens();
                m_bHasActivated = false;
                m_bReady = true ;
            }
        }

        void EnterCombat(Unit *aggro)
        {
            if(GameObject* go = me->SummonGameObject(200002, -766.802002f, 1391.209961f, 146.738998f, 0.236796f, 0, 0, 0, 0, 0))
				go->SetGoState(GO_STATE_READY);
            if(instance)
                instance->SetData(INSTANCE_DATA_OOK_OOK_STATUS, IN_PROGRESS);

            events.ScheduleEvent(EVENT_GROUND_POUND, IsHeroic() ? 6000 : 8000);
            Talk(SAY_ENTER_COMBAT);
            m_bHasActivated = true ;
			DoZoneInCombat();
        }

        void JustDied(Unit* killer)
        {
            if(instance)
                instance->SetData(INSTANCE_DATA_OOK_OOK_STATUS, DONE);
            if(GameObject* go = me->FindNearestGameObject(200002, 50000.0f))
                go->RemoveFromWorld();
            if(Creature* stalker = me->FindNearestCreature(200501, 50000.0f))
                stalker->AI()->DoAction(1);
			
			DoRemoveBarrels();
            Talk(SAY_JUST_DIED);
        }

        void KilledUnit(Unit *killed)
        {
            Talk(SAY_KILLED_PLAYER);
            if(!UpdateVictim())
            {
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Stormstout Brewery: Ook Ook killed a player and UpdateVictim() == NULL ; EnterEvadeMode() called");
                m_bReady = false ;
            }
        }

        void EnterEvadeMode()
        {
            if(instance)
                instance->SetData(INSTANCE_DATA_OOK_OOK_STATUS, FAIL);
			
			if(GameObject* go = me->FindNearestGameObject(200002, 50000.0f))
                go->RemoveFromWorld();

            if(!m_bReady)
                ScriptedAI::EnterEvadeMode();
        }

        void DamageTaken(Unit *doneBy, uint32 &amount)
        {
            if((me->GetHealth() - amount) < me->CountPctFromMaxHealth(m_uiNextBananaPercent))
            {
                DoCast(me, SPELL_GOING_BANANAS, true);
                Talk(SAY_KILLED_PLAYER - m_uiNextBananaPercent / 30);
				DoRemoveBarrels();
                if(instance)
                    instance->ProcessEvent(NULL, INSTANCE_EVENT_SUMMON_ALL);
                Talk(SAY_SUMMON_ALL_BARRELS);
                m_uiNextBananaPercent -= 30 ;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(!m_bReady && !UpdateVictim() && m_bHasActivated)
            {
                events.Update(diff);

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                    case EVENT_CHECK :
                        if(DoCheckForPlayers())
                        {
                            me->GetMotionMaster()->MoveJump(jumpPosition, 1, 1);
                            events.Reset();
                            m_bReady = true ;
                            break;
                        }
                        else
                        {
                            events.ScheduleEvent(EVENT_CHECK, 500);
                            return ;
                        }
                        break ;

                    default :
                        break ;
                    }
                }
            }

            if(m_bReady && !UpdateVictim())
                return ;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return ;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_GROUND_POUND :
                    DoCastAOE(SPELL_GROUND_POUND);
                    events.ScheduleEvent(EVENT_GROUND_POUND, IsHeroic() ? urand(5000, 6000) : urand(6000, 8000));
                    break ;

                default :
                    break ;
                }
            }

            DoMeleeAttackIfReady();
        }

        bool DoCheckForPlayers()
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
                            if(p->isDead())
                                return false ;
                            else
                                continue ;
                        }
                    }
                }
                else
                    return false ;
            }
            else
                return false ;

            return true ;
        }

        void DoRemoveBarrels()
        {
            CreatureList barrels ;
            GetCreatureListWithEntryInGrid(barrels, me, NPC_ROLLING_BARREL, 50000.0f);

            if(!barrels.empty())
            {
                for(CreatureListConstIter iter = barrels.begin() ; iter != barrels.end() ; ++iter)
                {
                    if(Creature* c = *iter)
                        c->DisappearAndDie();
                }
            }
        }
		
		void DoSlayHozens()
		{
			CreatureList hozens ;
			GetCreatureListWithEntryInGrid(hozens, me, 56927, 5000.0f);
			
			for(CreatureListConstIter iter = hozens.begin() ; iter != hozens.end() ; ++iter)
				(*iter)->AI()->DoAction(1);
		}

    private :
        EventMap events ;
        InstanceScript * instance ;
        uint32 m_uiNextBananaPercent ;
        bool m_bHasActivated ;
        bool m_bReady ;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new boss_ook_ookAI(creature);
    }
};

void AddSC_boss_ook_ook()
{
    new boss_ook_ook();
}
