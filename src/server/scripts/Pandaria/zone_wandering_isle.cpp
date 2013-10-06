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

class npc_shang_xi_the_lesson_of_the_burning_scroll : public CreatureScript
{
public:
    npc_shang_xi_the_lesson_of_the_burning_scroll() : CreatureScript("npc_shang_xi_the_lesson_of_the_burning_scroll") { }

    bool OnGossipHello(Player *p, Creature *c)
    {
        if(p->hasQuest(QUEST_THE_LESSON_OF_THE_BURNING_SCROLL))
        {
            p->CastSpell(p, SPELL_CREATE_THE_FLAMME, true);
            p->KilledMonsterCredit(KILL_CREDIT_FLAMME, 0);
            c->Say(SHANG_XI_TALK, 0, p->GetGUID());

            return true;
        }
    }
private:
    enum enums
    {
        QUEST_THE_LESSON_OF_THE_BURNING_SCROLL = 29408,
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

            p->DestroyItemCount(FLAMME, 1, true);
            if(GameObject* go = ObjectAccessor::GetGameObject(*p, 406808))
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


void AddSC_wandering_isle()
{
    new npc_first_quest_pandaren();
    new npc_shang_xi_the_lesson_of_the_burning_scroll();
    new gob_edict_of_temperance_the_lesson_of_the_burning_scroll();

}
