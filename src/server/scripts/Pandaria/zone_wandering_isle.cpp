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
#include "ScriptedGossip.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "GridNotifiers.h"
#include "ScriptedEscortAI.h"

/*******************************/
/********* AreaTrigger *********/
/*******************************/

enum Area7736
{
    NPC_HUOJIN_MONK             = 60176,
    QUEST_THE_SPIRIT_GUARDIAN   = 29420,
    SAY_HUOJIN                  = 0
};

class at_huojin_monk_talk : public AreaTriggerScript
{
public :
    at_huojin_monk_talk() : AreaTriggerScript("at_huojin_monk_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_SPIRIT_GUARDIAN) == QUEST_STATUS_COMPLETE)
        {
            Creature* huojin = player->FindNearestCreature(NPC_HUOJIN_MONK, 500.0f);
            if(huojin)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                huojin->AI()->Talk(SAY_HUOJIN, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7737
{
    NPC_JAOMIN_RO                   = 54611,
    QUEST_THE_DISCIPLE_CHALLENGE    = 29409,
    SAY_JAOMIN                      = 0
};

class at_jaomin_ro_talk : public AreaTriggerScript
{
public :
    at_jaomin_ro_talk() : AreaTriggerScript("at_jaomin_ro_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_DISCIPLE_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* jaomin = player->FindNearestCreature(NPC_JAOMIN_RO, 500.0f);
            if(jaomin)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                jaomin->AI()->Talk(SAY_JAOMIN, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7746
{
    NPC_TRAINEE_NIM = 60183,
    SAY_NIM         = 0
};

class at_trainee_nim_talk : public AreaTriggerScript
{
public :
    at_trainee_nim_talk() : AreaTriggerScript("at_trainee_nim_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_DISCIPLE_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* nim = player->FindNearestCreature(NPC_TRAINEE_NIM, 500.0f);
            if(nim)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                nim->AI()->Talk(SAY_NIM, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};

enum Area7747
{
    NPC_TRAINEE_GUANG           = 60244,
    SAY_GUANG                   = 0,
    QUEST_AYSA_OF_THE_TUSHUI    = 29410
};

class at_trainee_guang_talk : public AreaTriggerScript
{
public :
    at_trainee_guang_talk() : AreaTriggerScript("at_trainee_guang_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_AYSA_OF_THE_TUSHUI) == QUEST_STATUS_COMPLETE)
        {
            Creature* guang = player->FindNearestCreature(NPC_TRAINEE_GUANG, 500.0f);
            if(guang)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                guang->AI()->Talk(SAY_GUANG, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum AreaLorvo
{
    NPC_LORVO   = 54943,
    SAY_LORVO   = 0
};

enum AreaAysa
{
    NPC_AYSA                        = 54567,
    QUEST_THE_MISSING_DRIVER_AREA   = 29419,
    SAY_AYSA                        = 0
};

class at_area_7748_talk : public AreaTriggerScript
{
public :
    at_area_7748_talk() : AreaTriggerScript("at_area_7748_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_AYSA_OF_THE_TUSHUI) == QUEST_STATUS_COMPLETE)
        {
            Creature* lorvo = player->FindNearestCreature(NPC_LORVO, 500.0f);
            if(lorvo)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                lorvo->AI()->Talk(SAY_LORVO, player->GetGUID());
                return true;
            }
        }
        if(player->GetQuestStatus(QUEST_THE_MISSING_DRIVER_AREA) == QUEST_STATUS_COMPLETE)
        {
            Creature* aysa = player->FindNearestCreature(NPC_AYSA, 500.0f);
            if(aysa)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                aysa->AI()->Talk(SAY_AYSA, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7749
{
    NPC_JI_FIREPAW          = 54568,
    SAY_JI                  = 0,
    QUEST_JI_OF_THE_HUOJIN  = 29522
};

class at_ji_firepaw_talk : public AreaTriggerScript
{
public :
    at_ji_firepaw_talk() : AreaTriggerScript("at_ji_firepaw_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_JI_OF_THE_HUOJIN) == QUEST_STATUS_COMPLETE)
        {
            Creature* ji = player->FindNearestCreature(NPC_JI_FIREPAW, 500.0f);
            if(ji)
            {
                forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
                ji->AI()->Talk(SAY_JI, player->GetGUID());
                return true;
            }
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum AreaChia
{
    NPC_CHIA    = 60248,
    SAY_CHIA    = 0
};

enum AreaBrewerLin
{
    NPC_BREWER_LIN          = 60253,
    QUEST_THE_PASSION_AREA  = 29423,
    SAY_LIN                 = 0
};

class at_area_7750_talk : public AreaTriggerScript
{
public :
    at_area_7750_talk() : AreaTriggerScript("at_area_7750_talk") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_THE_PASSION_AREA) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* lin = player->FindNearestCreature(NPC_BREWER_LIN, 500.0f);
            Creature* chia = player->FindNearestCreature(NPC_CHIA, 500.0f);
            if(lin)
                lin->AI()->Talk(SAY_LIN, player->GetGUID());
            if(chia)
                chia->AI()->Talk(SAY_CHIA, player->GetGUID());

            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 60000));
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


enum Area7784
{
    QUEST_SINGING_POOLS     = 29521,
    NPC_DENG                = 60249,
    NPC_CAI                 = 60250
};

class at_pop_child_panda : public AreaTriggerScript
{
public :
    at_pop_child_panda() : AreaTriggerScript("at_pop_child_panda") {}

    bool OnTrigger(Player *player, const AreaTriggerEntry *at)
    {
        std::map<uint64, uint32>::iterator iter = forbiddenPlayers.find(player->GetGUID());
        if(iter != forbiddenPlayers.end())
            return false;
        if(player->GetQuestStatus(QUEST_SINGING_POOLS) == QUEST_STATUS_COMPLETE)
        {
            forbiddenPlayers.insert(std::pair<uint64, uint32>(player->GetGUID(), 120000));
            player->SummonCreature(NPC_DENG, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
            player->SummonCreature(NPC_CAI, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
            return true;
        }
        return false;
    }

    void Update(const uint32 uiDiff)
    {
        for (std::map<uint64, uint32>::iterator iter = forbiddenPlayers.begin() ; iter != forbiddenPlayers.end() ; ++iter)
        {
            if(iter->second <= uiDiff)
                forbiddenPlayers.erase(iter);
            else
                iter->second -= uiDiff ;
        }
    }

private :
    std::map<uint64, uint32> forbiddenPlayers ;
};


/************************************/
/******* FIN AREATRIGGER ************/
/************************************/



// npc_first_quest_pandaren
class npc_first_quest_pandaren : public CreatureScript
{
public:
    npc_first_quest_pandaren() : CreatureScript("npc_first_quest_pandaren") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_first_quest_pandarenAI (pCreature);
    }

    struct npc_first_quest_pandarenAI : public ScriptedAI
    {
        npc_first_quest_pandarenAI(Creature *c) : ScriptedAI(c)
        {
        }

        uint32 timer;

        void Reset()
        {
            timer = 0;
        }


        void UpdateAI(const uint32 diff)
        {
            if (timer < diff)
            {
                Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        Player *p = i->getSource();
                        if (p)
                        {
                            if (me->GetExactDist(p) < 40)
								if(p->getRace() == RACE_PANDAREN)
								{
									uint32 questId = 0;

									switch(p->getClass())
									{
									case CLASS_MONK:
										questId = 30039;
										break;
									case CLASS_MAGE:
										questId = 30040;
										break;
									case CLASS_HUNTER:
										questId = 30041;
										break;
									case CLASS_PRIEST:
										questId = 30042;
										break;
									case CLASS_ROGUE:
										questId = 30043;
										break;
									case CLASS_SHAMAN:
										questId = 30044;
										break;
									case CLASS_WARRIOR:
										questId = 30045;
										break;
									default: // Not supposed to happen but in case of
										questId = 30044;
										break;
									}

									Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
									if (quest && !p->hasQuest(questId) && p->CanAddQuest(quest, true) && (p->GetQuestStatus(questId) != QUEST_STATUS_COMPLETE) && (p->GetQuestStatus(questId) != QUEST_STATUS_REWARDED))
									{
										p->AddQuest(quest, NULL);
										p->CompleteQuest(questId);
									}
								}
                        }
                    }
                }

                timer = 2000;
            }
            else
                timer -=diff;
        }
    };
};

/*******************************/
/**The Lesson of Stifled Pride**/
/*******************************/

enum Spells
{
    SPELL_JAB               = 109079,
    SPELL_BLACKOUT_KICK     = 109080
};

enum Texts
{
    SAY_LOOSE   = 0
};

enum Creatures
{
    NPC_ZHI    = 61411
};

class npc_trainee : public CreatureScript
{
public:
    npc_trainee() : CreatureScript("npc_trainee") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_traineeAI(creature);
    }

    struct npc_traineeAI : public ScriptedAI
    {
        npc_traineeAI(Creature* creature) : ScriptedAI(creature)
        {
            creature->SetReactState(REACT_PASSIVE);
        }

        uint32 AttackTimer;
        uint32 DespawnTimer;
        uint32 EmoteTimer;
        bool VerifPV;
        bool Despawn;
        bool Health;
        bool EmoteSpeak;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            AttackTimer = 5000;
            VerifPV = true;
            Despawn = false;
            EmoteSpeak = false;
            Health = true;
            me->setFaction(7);
        }

        void DamageTaken(Unit* caster, uint32 &damage)
        {
            if(damage >= 1)
            {
                if(caster->GetTypeId() == TYPEID_PLAYER && !me->HasReactState(REACT_AGGRESSIVE))
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    ScriptedAI::AttackStart(caster);
                }
            }

            if (damage >= me->GetHealth())
            {
                damage = 0;
                me->SetHealth(0);
            }

        }

        void JustDied(Unit* /*killer*/)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(uint32 diff)
        {
            if(Despawn)
            {
                if(DespawnTimer <= diff)
                {
                    me->DisappearAndDie();
                    Despawn = false;
                }
                else DespawnTimer -= diff;
            }

            if(EmoteSpeak)
            {
                if(EmoteTimer <= diff)
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    Talk(SAY_LOOSE);
                    EmoteSpeak = false;
                }
                else EmoteTimer -= diff;
            }

            if(!UpdateVictim())
                return;

            if(VerifPV)
            {
                if(AttackTimer <= diff)
                {
                    if(me->GetEntry() == 54586 || me->GetEntry() == 65470)
                        me->CastSpell(me->getVictim(), SPELL_JAB);

                    if(me->GetEntry() == 54587 || me->GetEntry() == 65471)
                        me->CastSpell(me->getVictim(), SPELL_BLACKOUT_KICK);

                    AttackTimer = 5000;
                }
                else AttackTimer -= diff;
            }

            if (me->GetHealthPct() <= 20 && Health)
            {
                VerifPV = false;

                DespawnTimer = 5000;
                Despawn = true;

                EmoteTimer = 2000;
                EmoteSpeak = true;

                if(Unit* player = me->getVictim())
                    if(player->GetTypeId() == TYPEID_PLAYER)
                        player->ToPlayer()->KilledMonsterCredit(54586, 0);

                me->setFaction(35);
                me->StopMoving();
                me->RemoveAllAuras();
                me->GetMotionMaster()->Clear();
                me->CombatStop(true);
                me->DeleteThreatList();

                Health = false;
            }

            DoMeleeAttackIfReady();
        }
    };
};

/*#####
## at_The Missing Driver
#####*/

enum eTheMissingDriver
{
    QUEST_THE_MISSING_DRIVER    = 29419,
    NPC_AMBERLEAF_SCAMP         = 54130,
    NPC_MIN_DIMWIND             = 54855,
    NPC_MIN_DIMWIND_POP         = 56503,
    SAY_1                       = 0

};

class areatrigger_at_the_missing_driver : public AreaTriggerScript
{
    public:

        areatrigger_at_the_missing_driver(): AreaTriggerScript("areatrigger_at_the_missing_driver")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->isAlive() && !player->isInCombat())
            {
                if (player->GetQuestStatus(QUEST_THE_MISSING_DRIVER) == QUEST_STATUS_INCOMPLETE)
                {
                    if(Creature* min = player->FindNearestCreature(NPC_MIN_DIMWIND, 20.0f))
                    {
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, player, NPC_AMBERLEAF_SCAMP, 20.0f);

                        for(std::list<Creature*>::const_iterator iter = creatures.begin() ; iter != creatures.end() ; ++iter)
                        {
                            (*iter)->AI()->Talk(SAY_1);
                            (*iter)->DespawnOrUnsummon(3000);
                        }

                        min->DisappearAndDie();
                        player->SummonCreature(NPC_MIN_DIMWIND_POP, min->GetPositionX(), min->GetPositionY(), min->GetPositionZ(), 2.08f, TEMPSUMMON_TIMED_DESPAWN, 120000);
                        player->KilledMonsterCredit(54855);

                        return true;
                    }
                }

                return false;
            }
            return false;
        }
};

/*######
## npc_min_dimwind_pop
######*/

enum eMinDimwind
{
    SAY_DIMWIND_1    = 0,
    SAY_DIMWIND_2    = 1,
    SAY_DIMWIND_3    = 2
};

class npc_min_dimwind_pop : public CreatureScript
{
public:
    npc_min_dimwind_pop(): CreatureScript("npc_min_dimwind_pop") { }

    struct npc_min_dimwind_popAI : public npc_escortAI
    {
        npc_min_dimwind_popAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_DIMWIND_1);
                    break;
                case 2:
                    Talk(SAY_DIMWIND_2);
                    break;
                case 3:
                    Talk(SAY_DIMWIND_3);
                    break;
                case 10:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_min_dimwind_popAI(creature);
    }
};

/*######
## npc_aysa_cloudsinger_pop
######*/

enum eAysaCloudsinger
{
    SAY_AYSA_1    = 0
};

class npc_aysa_cloudsinger_pop : public CreatureScript
{
public:
    npc_aysa_cloudsinger_pop(): CreatureScript("npc_aysa_cloudsinger_pop") { }

    struct npc_aysa_cloudsinger_popAI : public npc_escortAI
    {
        npc_aysa_cloudsinger_popAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
        }

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_AYSA_1);
                    break;
                case 2:
                    me->GetMotionMaster()->MoveJump(1197.14f, 3492.05f, 91.33f, 20, 20);
                    break;
                case 4:
                    me->GetMotionMaster()->MoveJump(1193.65f, 3478.94f, 108.50f, 20, 20);
                    break;
                case 6:
                    me->GetMotionMaster()->MoveJump(1188.2f, 3456.19f, 102.90f, 20, 20);
                    break;
                case 10:
                    me->DespawnOrUnsummon();
                    break;

            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);

            if (UpdateVictim())
                return;

            Start(false, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aysa_cloudsinger_popAI(creature);
    }
};



/*************************************/
/********The Way of the Tushui********/
/*************************************/

const Position SummonPositions[] =
{
    {1180.69f, 3450.00f, 103.00f, 3.50f},
    {1181.10f, 3447.40f, 102.70f, 3.50f},
    {1182.05f, 3444.38f, 102.70f, 3.50f},
    {1183.14f, 3440.85f, 102.70f, 3.50f}
};

enum Quests
{
    QUEST_THE_WAY_OF_THE_TUSHUI     = 29414
};

enum Events
{
    EVENT_ADD_POWER     = 1,
    EVENT_SUMMON_NPCS   = 2,
    EVENT_INTRO         = 3,
    EVENT_LOOK_PLAYERS  = 4
};

enum SpellsAysa
{
    SPELL_MEDITATION_BAR = 116421
};

enum Npcs
{
    NPC_MASTER_LI_FEI   = 54856,
    MOB_SCAMP           = 59637
};

enum SaysLiFei
{
    LI_FEI_SPEECH_1     = 0,
    LI_FEI_SPEECH_2     = 1,
    LI_FEI_SPEECH_3     = 2,
    LI_FEI_SPEECH_4     = 3,
    LI_FEI_SPEECH_5     = 4,
    LI_FEI_SPEECH_6     = 5,
    LI_FEI_SPEECH_7     = 6
};

enum SaysAysa
{
    AYSA_SAY_INTRO  = 0,
    AYSA_SAY_OUTRO  = 1
};

enum Area
{
    AREA_MEDITATION = 5848
};

class npc_aysa_cloudsinger_meditation : public CreatureScript
{
public :
    npc_aysa_cloudsinger_meditation() : CreatureScript("npc_aysa_cloudsinger_meditation") {    }

    struct npc_aysa_cloudsinger_meditation_AI : public ScriptedAI
    {
    public :
        npc_aysa_cloudsinger_meditation_AI(Creature* c) : ScriptedAI(c), Summons(me)
        {
        }

        bool isStarted ;
        uint32 StartTimer;
        EventMap events ;
        Creature* LiFei;
        SummonList Summons;

        void Reset()
        {
            isStarted = false ;
            Summons.DespawnAll();
            StartTimer = 1000;
        }

        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);
        }

        void UpdateAI(uint32 diff)
        {
            if(StartTimer <= diff)
            {
                if(!isStarted)
                {
                    if(isAPlayerWithQuestInDist())
                    {
                        StartEvent();
                        isStarted = true ;
                    }
                    else
                        return ;
                }

                StartTimer = 1000;
            }
            else StartTimer -= diff;

            events.Update(diff);

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                case EVENT_INTRO :
                    DoIntro();
                    break ;

                case EVENT_ADD_POWER :
                    AddPowerToPlayersOnMap();
                    events.ScheduleEvent(EVENT_ADD_POWER, 2000);
                    break ;

                case EVENT_SUMMON_NPCS :
                    SummonNpcs();
                    events.ScheduleEvent(EVENT_SUMMON_NPCS, 10000);
                    break ;

                case EVENT_LOOK_PLAYERS :
                    if(isAPlayerWithQuestInDist())
                        events.ScheduleEvent(EVENT_LOOK_PLAYERS, 1000);
                    else
                        EndEvent();
                    break ;

                default :
                    break ;
                }
            }
        }

        void StartEvent()
        {
            events.ScheduleEvent(EVENT_INTRO, 3000);
            events.ScheduleEvent(EVENT_ADD_POWER, 4000);
            events.ScheduleEvent(EVENT_SUMMON_NPCS, 2000);
            events.ScheduleEvent(EVENT_LOOK_PLAYERS, 3000);
        }

        void EndEvent()
        {
            events.CancelEvent(EVENT_ADD_POWER);
            events.CancelEvent(EVENT_INTRO);
            events.CancelEvent(EVENT_SUMMON_NPCS);
            events.CancelEvent(EVENT_LOOK_PLAYERS);
            isStarted = false;
            if(LiFei)
                LiFei->DespawnOrUnsummon();
            Reset();
        }

        bool isAPlayerWithQuestInDist()
        {
            Map* map = me->GetMap();
            if(map)
            {
                Map::PlayerList const& players = map->GetPlayers();

                if(players.isEmpty())
                    return false ;

                for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                {
                    Player *player = iter->getSource();
                    if(player)
                    {
                        if (player->isAlive() && player->GetQuestStatus(QUEST_THE_WAY_OF_THE_TUSHUI) == QUEST_STATUS_INCOMPLETE)
                        {
                            if(player->GetAreaId() == AREA_MEDITATION && player->IsInDist2d(me, 35))
                            {
                                if(!player->HasAura(SPELL_MEDITATION_BAR))
                                {
                                    player->CastSpell(player, SPELL_MEDITATION_BAR, true);
                                    player->SetMaxPower(POWER_ALTERNATE_POWER, 90);
                                    player->SetPower(POWER_ALTERNATE_POWER, 0);
                                }
                                return true;
                            }
                        }
                        if (!player->IsInDist2d(me, 35) && player->HasAura(SPELL_MEDITATION_BAR))
                        {
                            player->RemoveAura(SPELL_MEDITATION_BAR);
                        }
                    }
                }
            }
            return false ;
        }

        void AddPowerToPlayersOnMap()
        {
            if(!isStarted)
                return ;

            Map* map = me->GetMap();
            if(map)
            {
                Map::PlayerList const& players = map->GetPlayers();

                if(players.isEmpty())
                    return ;

                for(Map::PlayerList::const_iterator iter = players.begin() ; iter != players.end() ; ++iter)
                {
                    Player* player = iter->getSource();
                    if(player)
                    {
                        if (player->isAlive() && player->GetQuestStatus(QUEST_THE_WAY_OF_THE_TUSHUI) == QUEST_STATUS_INCOMPLETE)
                        {
                            if(player->GetAreaId() == AREA_MEDITATION && player->IsInDist2d(me, 35))
                            {
                                if(player->HasAura(SPELL_MEDITATION_BAR))
                                {
                                    AddPower(player);
                                }
                            }
                        }
                    }
                }
            }
        }

        void AddPower(Player* player)
        {
            if(!isStarted)
                return ;

            if(player)
            {
                player->ModifyPower(POWER_ALTERNATE_POWER, +2);

                switch(player->GetPower(POWER_ALTERNATE_POWER))
                {
                    case 20 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_1, player->GetGUID());
                        break;
                    case 40 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_2, player->GetGUID());
                        break ;
                    case 50 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_3, player->GetGUID());
                        break ;
                    case 60 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_4, player->GetGUID());
                        break;
                    case 70 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_5, player->GetGUID());
                        break ;
                    case 80 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_6, player->GetGUID());
                        break;
                    case 90 :
                        LiFei->AI()->Talk(LI_FEI_SPEECH_7, player->GetGUID());
                        player->KilledMonsterCredit(NPC_MASTER_LI_FEI);
                        player->RemoveAura(SPELL_MEDITATION_BAR);
                        Talk(AYSA_SAY_OUTRO, player->GetGUID());
                        break ;

                default :
                    break ;
                }
            }
        }

        void SummonNpcs()
        {
            if(!isStarted)
                return ;

            uint8 number = (1 + (rand() % 3));

            for(uint8 i = 0 ; i < number ; ++i)
            {
                Creature* scamp = me->SummonCreature(MOB_SCAMP, SummonPositions[i], TEMPSUMMON_TIMED_DESPAWN, 180000);
                    scamp->GetMotionMaster()->MovePoint(1, 1144.12f, 3435.47f, 105.3f);
            }
        }

        void DoIntro()
        {
            if(!isStarted)
                return ;

            LiFei = me->SummonCreature(NPC_MASTER_LI_FEI, 1130.19f, 3435.37f, 106.00f, 0.19f);
            Talk(AYSA_SAY_INTRO);
        }
    };

    CreatureAI* GetAI(Creature *c) const
    {
        return new npc_aysa_cloudsinger_meditation_AI(c);
    }
};


/*************************************/
/********Fanning the Flames********/
/*************************************/

enum SpellsAir
{
    SPELL_LIGHTNING_BOLT    = 73212,
    SPELL_LIGHTNING_CLOUD   = 108693
};

class npc_living_air: public CreatureScript
{
public:
    npc_living_air() : CreatureScript("npc_living_air") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_living_airAI(creature);
    }

    struct npc_living_airAI : public ScriptedAI
    {
            npc_living_airAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Bolt1_timer;
            uint32 Bolt2_timer;
            uint32 Bolt3_timer;
            uint32 Cloud_timer;

            void Reset()
            {
                Bolt1_timer = 1000;
                Bolt2_timer = 4200;
                Bolt3_timer = 7400;
                Cloud_timer = 10600;
            }

            void UpdateAI(uint32 diff)
            {
                if(!UpdateVictim())
                    return;

                if (Bolt1_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_BOLT, false);
                    Bolt1_timer = 14000;
                }
                else Bolt1_timer -= diff;

                if (Bolt2_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_BOLT, false);
                    Bolt2_timer = 14000;
                }
                else Bolt2_timer -= diff;

                if (Bolt3_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_BOLT, false);
                    Bolt3_timer = 14000;
                }
                else Bolt3_timer -= diff;

                if (Cloud_timer <= diff)
                {
                    me->CastSpell(me->getVictim(), SPELL_LIGHTNING_CLOUD, false);
                    Cloud_timer = 14000;
                }
                else Cloud_timer -= diff;

                DoMeleeAttackIfReady();
            }
    };
};


/*************************************/
/****Only the Worthy Shall Pass****/
/*************************************/

class spell_blessing_flamme_panda: public SpellScriptLoader
{
    public:
        spell_blessing_flamme_panda() : SpellScriptLoader("spell_blessing_flamme_panda") { }

        class spell_blessing_flamme_panda_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blessing_flamme_panda_AuraScript);

            bool DoCheckTarget(Unit* target)
            {
                if(target)
                {
                    if(target->ToCreature() && target->ToCreature()->GetEntry() == 54900)
                        return true;

                    if(target->ToPlayer() && target->ToPlayer()->GetQuestStatus(29421) == QUEST_STATUS_INCOMPLETE)
                        return true;
                }
                return false;
            }

            void Register()
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_blessing_flamme_panda_AuraScript::DoCheckTarget);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_blessing_flamme_panda_AuraScript();
        }
};


/*************************************/
/****The Passion of Shen-zin Su****/
/*************************************/

enum HuoEnum
{
    NPC_SHANG_XI        = 54786,
    SAY_SHANG_XI        = 0,
    QUEST_THE_PASSION   = 29423
};

class npc_huo_escort: public CreatureScript
{
public:
    npc_huo_escort() : CreatureScript("npc_huo_escort") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_huo_escortAI(creature);
    }

    struct npc_huo_escortAI : public ScriptedAI
    {
            npc_huo_escortAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 TestTimer;

            void Reset()
            {
                TestTimer = 1000;
                if (Unit* owner = me->GetOwner())
                    me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (who->GetEntry() == NPC_SHANG_XI)
                {
                    if (me->IsWithinDistInMap(who, 10.0f))
                    {
                        if (Creature* talker = who->ToCreature())
                            talker->AI()->Talk(SAY_SHANG_XI);

                        if (Unit* owner = me->GetOwner())
                            if(Player* player = owner->ToPlayer())
                                player->KilledMonsterCredit(61128);

                        me->DespawnOrUnsummon();
                    }
                }
            }

            void UpdateAI(uint32 diff)
            {
                if (TestTimer <= diff)
                {
                    if (Unit* owner = me->GetOwner())
                        if(owner->ToPlayer() && owner->ToPlayer()->GetQuestStatus(QUEST_THE_PASSION) == QUEST_STATUS_NONE)
                            me->DespawnOrUnsummon();

                    TestTimer = 1000;
                }
                else TestTimer -= diff;
            }
    };
};


/*************************************/
/*********The Singing Pools**********/
/*************************************/

enum ChildDeng
{
    SAY_DENG_0      = 0,
    SAY_DENG_1      = 1,
    SAY_DENG_2      = 2
};

class npc_deng_child: public CreatureScript
{
public:
    npc_deng_child() : CreatureScript("npc_deng_child") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deng_childAI(creature);
    }

    struct npc_deng_childAI : public ScriptedAI
    {
            npc_deng_childAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Say0_timer;
            uint32 Say1_timer;
            uint32 Say2_timer;
            uint32 Despawn_timer;

            void Reset()
            {
                Say0_timer = 9000;
                Say1_timer = 25000;
                Say2_timer = 41000;
                Despawn_timer = 45000;

                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            void UpdateAI(uint32 diff)
            {
                if (Say0_timer <= diff)
                {
                    Talk(SAY_DENG_0);
                    Say0_timer = 60000;
                }
                else Say0_timer -= diff;

                if (Say1_timer <= diff)
                {
                    Talk(SAY_DENG_1);
                    Say1_timer = 60000;
                }
                else Say1_timer -= diff;

                if (Say2_timer <= diff)
                {
                    Talk(SAY_DENG_2);
                    Say2_timer = 60000;
                }
                else Say2_timer -= diff;

                if (Despawn_timer <= diff)
                {
                    me->DespawnOrUnsummon();
                    Despawn_timer = 60000;
                }
                else Despawn_timer -= diff;
            }
    };
};

enum ChildCai
{
    SAY_CAI_0      = 0,
    SAY_CAI_1      = 1,
    SAY_CAI_2      = 2,
    SAY_CAI_3      = 3
};

class npc_cai_child: public CreatureScript
{
public:
    npc_cai_child() : CreatureScript("npc_cai_child") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cai_childAI(creature);
    }

    struct npc_cai_childAI : public ScriptedAI
    {
            npc_cai_childAI(Creature* creature) : ScriptedAI(creature) {}

            uint32 Say0_timer;
            uint32 Say1_timer;
            uint32 Say2_timer;
            uint32 Say3_timer;
            uint32 Despawn_timer;

            void Reset()
            {
                Say0_timer = 1000;
                Say1_timer = 17000;
                Say2_timer = 33000;
                Say3_timer = 41000;
                Despawn_timer = 45000;

                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }

            void UpdateAI(uint32 diff)
            {
                if (Say0_timer <= diff)
                {
                    Talk(SAY_CAI_0);
                    Say0_timer = 60000;
                }
                else Say0_timer -= diff;

                if (Say1_timer <= diff)
                {
                    Talk(SAY_CAI_1);
                    Say1_timer = 60000;
                }
                else Say1_timer -= diff;

                if (Say2_timer <= diff)
                {
                    Talk(SAY_CAI_2);
                    Say2_timer = 60000;
                }
                else Say2_timer -= diff;

                if (Say3_timer <= diff)
                {
                    Talk(SAY_CAI_3);
                    Say3_timer = 60000;
                }
                else Say3_timer -= diff;

                if (Despawn_timer <= diff)
                {
                    me->DespawnOrUnsummon();
                    Despawn_timer = 60000;
                }
                else Despawn_timer -= diff;
            }
    };
};












/********************************/
/**The Lesson of the Iron Bough**/
/********************************/

class EquippedItemCheckPredicate
{
public :
    EquippedItemCheckPredicate(uint32 questId, uint32 itemId, uint32 itemId2) : item1(itemId), item2(itemId2), quest(questId)
    {
        if(itemId2) twice = true ;
    }

    bool operator()(Player* p)
    {
        if(!p) return false ;

        if(!twice)
        {
            if(p->hasQuest(quest) && p->GetQuestStatus(quest) == QUEST_STATUS_INCOMPLETE && p->GetItemByEntry(item1))
                return(p->GetItemByEntry(item1)->IsEquipped());
        }
        else
        {
            if(p->hasQuest(quest) && p->GetQuestStatus(quest) == QUEST_STATUS_INCOMPLETE && p->GetItemByEntry(item1) && p->GetItemByEntry(item2))
                return(p->GetItemByEntry(item1)->IsEquipped() && p->GetItemByEntry(item2)->IsEquipped());
        }

		return false ;
    }

    bool isTwiced()
    {
        return twice;
    }

private :
    uint32 item1, item2, quest ;
    bool twice ;
};

struct QuestAndItems
{
    uint8 playerClass;
    uint32 quest;
    uint32 questItems[2];
};

QuestAndItems items[] =
{
    {1, 30037, 76391, 72313},
    {2, 30038, 73210, 0},
    {3, 30034, 73211, 0},
    {4, 30036, 73208, 73212},
    {5, 30035, 73207, 76393},
    {8, 30033, 76390, 76392},
    {10, 30027, 73209, 0}
};

class stalker_item_equiped : public CreatureScript
{
public :
    stalker_item_equiped() : CreatureScript("stalker_item_equipped")
    {

    }

    struct stalker_item_equipedAI : public ScriptedAI
    {
    public :
        stalker_item_equipedAI(Creature* c) : ScriptedAI(c)
        {

        }

        void Reset()
        {
            checkTimer = 1000 ;
        }

        void UpdateAI(uint32 diff)
        {
            if(checkTimer <= diff)
            {
                Check();
                checkTimer = 1000 ;
            }
            else checkTimer -= diff ;
        }

    private :
        uint32 checkTimer;

        void Check()
        {
            Map* map = me->GetMap();
            if(!map) return ;

            Map::PlayerList const& pl = map->GetPlayers();

            if(pl.isEmpty()) return ;


            for(Map::PlayerList::const_iterator iter = pl.begin() ; iter != pl.end() ; ++iter)
            {
                if(Player* p = iter->getSource())
                {
                    uint8 pClass = p->getClass();
                    uint8 i = 0 ;
                    for(; i < 7 ; ++i)
                    {
                        if(pClass = items[i].playerClass)
                        {
                            EquippedItemCheckPredicate predicate(items[i].quest, items[i].questItems[0], items[i].questItems[1]);
                            if(predicate(p))
                            {
                                p->KilledMonsterCredit(REWARD_1);
                                if(predicate.isTwiced())
                                        p->KilledMonsterCredit(REWARD_2);
                            }
                            break ;
                        }
                    }
                }
            }
        }

        enum NPCS
        {
            REWARD_1 = 0,
            REWARD_2 = 0
        };
    };

    CreatureAI* GetAI(Creature *c) const
    {
        return new stalker_item_equipedAI(c);
    }
};

/**********************************/
/*****The Disciple's Challenge*****/
/**********************************/

class mob_jaomin_ro : public CreatureScript
{
public :
    mob_jaomin_ro() : CreatureScript("mob_jaomin_ro")
    {

    }

    struct mob_jaomin_roAI : public ScriptedAI
    {
    public :
        mob_jaomin_roAI(Creature* c) : ScriptedAI(c)
        {

        }

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit *pWho)
        {
            if(pWho->GetTypeId() != TYPEID_PLAYER)
            {
                me->Kill(pWho, false);
                EnterEvadeMode();
            }

            ScheduleEvents();
        }

        void DamageTaken(Unit *attacker, uint32 &amount)
        {
            if(me->GetHealth() < amount)
            {
                amount = 0 ;
                me->SetHealth(me->GetMaxHealth()/10);
                me->CombatStop(true);
                EnterEvadeMode();
                if(attacker->ToPlayer())
                    attacker->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
                me->Say(irand(SAY_DEFEAT_1, SAY_DEFEAT_7), LANG_COMMON, attacker->GetGUID());
            }
        }

        void UpdateAI(uint32 diff)
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
                case EVENT_HAWK :
                    if(me->getVictim())
                        DoCast(me->getVictim(), SPELL_HAWK, false);
                    events.ScheduleEvent(EVENT_ELEPHANT, 3000);;
                    break ;

                case EVENT_ELEPHANT :
                    if(me->getVictim())
                        DoCast(me->getVictim(), SPELL_ELEPHANT, false);
                    events.ScheduleEvent(EVENT_HAWK, 3000);
                    break;

                case EVENT_ROUNDHOUSE :
                    DoCastAOE(SPELL_ROUNDHOUSE, false);
                    events.ScheduleEvent(EVENT_ROUNDHOUSE, 5000);
                    break;

                default :
                    break ;
                }
            }
            DoMeleeAttackIfReady();
        }

    private :
        EventMap events ;

        inline void ScheduleEvents()
        {
            events.ScheduleEvent(RAND(EVENT_HAWK, EVENT_ELEPHANT), 3000);
            events.ScheduleEvent(EVENT_ROUNDHOUSE, 5000);
        }

        enum Events
        {
            EVENT_HAWK = 1,
            EVENT_ELEPHANT,
            EVENT_ROUNDHOUSE
        };

        enum Spells
        {
            SPELL_HAWK = 108955,
            SPELL_ELEPHANT = 108938,
            SPELL_ROUNDHOUSE = 119301
        };

        enum Says
        {
            SAY_DEFEAT_1 = -5461106,
            SAY_DEFEAT_2,
            SAY_DEFEAT_3,
            SAY_DEFEAT_4,
            SAY_DEFEAT_5,
            SAY_DEFEAT_6,
            SAY_DEFEAT_7
        };
    };

    CreatureAI* GetAI(Creature *c) const
    {
        return new mob_jaomin_roAI(c);
    }

private :
};

/****************************/
/*****The Missing Driver*****/
/****************************/

class MissingDriverCheckPredicate
{
public :
    MissingDriverCheckPredicate(uint32 questId) : quest(questId)
    {

    }

    bool operator()(Player* p)
    {
        if(!p) return false ;

        return(p->hasQuest(quest) && p->GetQuestStatus(quest) == QUEST_STATUS_INCOMPLETE);
    }

private :
    uint32 quest;
};

class mob_amberleaf_scamp29419 : public CreatureScript
{
public :
    mob_amberleaf_scamp29419() : CreatureScript("mob_amberleaf_scamp29419")
    {

    }

    struct mob_amberleaf_scamp29419_AI : public ScriptedAI
    {
    public :
        mob_amberleaf_scamp29419_AI(Creature* c) : ScriptedAI(c)
        {

        }

        void EnterCombat(Unit* who)
        {
            me->GetPosition(&homePosition);
            MissingDriverCheckPredicate predicate(29419);
            if(who->ToPlayer() && predicate(who->ToPlayer()))
            {
                me->GetMotionMaster()->MoveFleeing(who, 0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                me->Yell(RAND(YELL_FLEE_1, YELL_FLEE_2), LANG_COMMON, who->GetGUID());
                canAttack = false ;
                who->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
            }
            else
                me->AI()->AttackStart(who);
        }

        void Reset()
        {
            canAttack = true ;
            takeThisTimer = 3500 ;
            returnTimer = 7500 ;
        }

        void UpdateAI(uint32 diff)
        {
            if(!UpdateVictim() && canAttack) return ;

            if(!canAttack)
            {
                if(returnTimer <= diff)
                {
                    me->GetMotionMaster()->MovePoint(0, homePosition);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                    canAttack = true ;
                    returnTimer = 7500 ;
                }
                else returnTimer -= diff ;
            }
            else
            {
                if(takeThisTimer <= diff)
                {
                    if(me->getVictim())
                        DoCast(me->getVictim(), 109081, false);
                    takeThisTimer = 3000;
                }
                else takeThisTimer -= diff ;
            }
        }

    private :
        uint32 takeThisTimer;
        uint32 returnTimer;
        Position homePosition;
        bool canAttack;

        enum Yells
        {
            YELL_FLEE_1 = -5413001,
            YELL_FLEE_2
        };
    };

    CreatureAI* GetAI(Creature *c) const
    {
        return new mob_amberleaf_scamp29419_AI(c);
    }
};

/****************************/
/*****Fanning the Flames*****/
/****************************/

class spell_summon_living_air : public SpellScriptLoader
{
public :
    spell_summon_living_air() : SpellScriptLoader("spell_quest29523_summon_living_air")
    {

    }

    class spell_summon_living_airSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_living_airSpellScript)

        bool Validate(const SpellInfo *spellInfo)
        {
            if(!sSpellMgr->GetSpellInfo(106999) || !sSpellMgr->GetSpellInfo(102207))
                return false ;

            return true ;
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if(Unit* caster = GetCaster())
            {
                caster->CastSpell((Unit*)NULL, 102207, true);
            }
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_summon_living_airSpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_summon_living_airSpellScript();
    }
};

/********************************/
/*****The Challenger's Fires*****/
/********************************/

class TorchEquippedPredicate
{
public :
    TorchEquippedPredicate(uint32 questId, uint32 itemId) : quest(questId), item(itemId)
    {

    }

    bool operator()(Player* p)
    {
        if(!p || !p->GetItemByEntry(item) || !p->hasQuest(quest) || p->GetQuestStatus(quest) != QUEST_STATUS_INCOMPLETE)
            return false ;

        return(p->GetItemByEntry(item)->IsInBag());
    }

private:
    uint32 quest, item;
};

class go_brazier_of_flickering_flames : public GameObjectScript
{
public :
    go_brazier_of_flickering_flames() : GameObjectScript("go_brazier_of_flickering_flames")
    {

    }

    bool OnGossipHello(Player *p, GameObject *go)
    {
        if(!p->hasQuest(29664) || p->GetQuestStatus(29664) != QUEST_STATUS_INCOMPLETE)
            return false ;
        else if(p->hasQuest(29664) && p->GetQuestStatus(29664) == QUEST_STATUS_INCOMPLETE)
        {
            go->CastSpell(p, 105151);
            return true;
        }

		return false ;
    }
};


class npc_shang_xi_the_lesson_of_the_burning_scroll : public CreatureScript
{
public:
    npc_shang_xi_the_lesson_of_the_burning_scroll() : CreatureScript("npc_shang_xi_the_lesson_of_the_burning_scroll") { }
    
    bool OnGossipHello(Player *p, Creature *c)
    {
        if(p->hasQuest(QUEST_THE_LESSON_OF_THE_BURNING_SCROLL))
		{
			if(!p->HasItemCount(FLAMME, 1))
			{
				p->CastSpell(p, SPELL_ANIM_TAKE_FLAME, true);
				p->CastSpell(p, SPELL_CREATE_THE_FLAMME, true);
				p->KilledMonsterCredit(KILL_CREDIT_FLAMME, 0);
				c->MonsterSay(SHANG_XI_TALK, 0, p->GetGUID());
			}

		} else if(p->GetQuestStatus(QUEST_THE_LESSON_OF_THE_BURNING_SCROLL) == QUEST_STATUS_COMPLETE) {
			if (c->isQuestGiver())
				p->PrepareQuestMenu(c->GetGUID());
				p->PlayerTalkClass->SendGossipMenu(1,  c->GetGUID());
		}

		return true;
    }
private: 
	enum enums
	{
		QUEST_THE_LESSON_OF_THE_BURNING_SCROLL = 29408,
		SPELL_ANIM_TAKE_FLAME = 114746,
		FLAMME = 80212,
		SPELL_CREATE_THE_FLAMME = 114611,
		SHANG_XI_TALK = 0,
		KILL_CREDIT_FLAMME = 59591
	};
};

class gob_edict_of_temperance_the_lesson_of_the_burning_scroll : public GameObjectScript
{
public:
    gob_edict_of_temperance_the_lesson_of_the_burning_scroll() : GameObjectScript("gob_edict_of_temperance_the_lesson_of_the_burning_scroll") { }

    bool OnGossipHello(Player* p, GameObject* gob)
    {
        if(p->HasItemCount(FLAMME, 1))
        {
            p->KilledMonsterCredit(KILL_CREDIT_BURN, 0);
            p->CastSpell(p, SPELL_NEW_PHASE, true);
            p->RemoveAura(p->GetAura(59073)); // Enlève la phase 2

            p->DestroyItemCount(FLAMME, 1, true);
            if(GameObject* go = ObjectAccessor::GetGameObject(*p, 400014))
                if(Creature* npc = go->FindNearestCreature(TRACKER, 10, true))
                    npc->CastSpell(npc, SPELL_BURN, true);
        }

        return true;
    }
private:
    enum enums
    {
        TRACKER = 65490,
        FLAMME = 80212,
        SPELL_BURN = 88579,
        SPELL_NEW_PHASE = 59074, // Tester si le changement de phase marche bien
        KILL_CREDIT_BURN = 59570
    };
};


enum AreaTest
{
    SPELL_MALE  = 102938
};

class at_test_etang : public AreaTriggerScript
{
    public:

        at_test_etang() : AreaTriggerScript("at_test_etang")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if(player->HasAura(SPELL_MALE))
            {
                player->RemoveAurasDueToSpell(SPELL_MALE);
                return true;
            }

            if(!player->HasAura(SPELL_MALE))
            {
                player->CastSpell(player, SPELL_MALE, true);
                return true;
            }

            return true;
        }
};

class npc_balance_pole: public CreatureScript
{
public:
    npc_balance_pole() : CreatureScript("npc_balance_pole") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_balance_poleAI(creature);
    }

    struct npc_balance_poleAI : public ScriptedAI
    {
            npc_balance_poleAI(Creature* creature) : ScriptedAI(creature) {}


            void Reset()
            {
                me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
                me->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);
            }

            void UpdateAI(uint32 diff)
            {

            }
    };
};



void AddSC_wandering_isle()
{
    new at_huojin_monk_talk();
    new at_jaomin_ro_talk();
    new at_trainee_nim_talk();
    new at_trainee_guang_talk();
    new at_area_7748_talk();
    new at_ji_firepaw_talk();
    new at_area_7750_talk();
    new at_pop_child_panda();
    new npc_first_quest_pandaren();	
    new npc_trainee();
    new areatrigger_at_the_missing_driver();
    new npc_min_dimwind_pop();
    new npc_aysa_cloudsinger_pop();
    new npc_aysa_cloudsinger_meditation();
    new npc_living_air();
    new spell_blessing_flamme_panda();
    new npc_huo_escort();
    new npc_deng_child();
    new npc_cai_child();
    new at_test_etang();
    new npc_balance_pole();

    new stalker_item_equiped();
    new mob_jaomin_ro();
    new mob_amberleaf_scamp29419();
    new spell_summon_living_air();
    new go_brazier_of_flickering_flames();	
	new npc_shang_xi_the_lesson_of_the_burning_scroll();
	new gob_edict_of_temperance_the_lesson_of_the_burning_scroll();
}
