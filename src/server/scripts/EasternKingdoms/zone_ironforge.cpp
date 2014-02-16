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

/* ScriptData
SDName: Ironforge
SD%Complete: 0
SDComment:
SDCategory: Ironforge
EndScriptData */

/* ContentData
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"

/*************************************/
/****** I Need to Cask a Favor ******/
/*************************************/

enum DarylEnum
{
    QUEST_I_NEED_TO_CASK    = 29356,
    SPELL_BEER_DELIVERY     = 99491
};

class npc_daryl_riknussun: public CreatureScript
{
public:
    npc_daryl_riknussun() : CreatureScript("npc_daryl_riknussun") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daryl_riknussunAI(creature);
    }

    struct npc_daryl_riknussunAI : public ScriptedAI
    {
            npc_daryl_riknussunAI(Creature* creature) : ScriptedAI(creature) {}


            void Reset()
            {
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    if(who->ToPlayer() && who->ToPlayer()->GetQuestStatus(QUEST_I_NEED_TO_CASK) == QUEST_STATUS_INCOMPLETE)
                    {
                        if(who->HasAura(SPELL_BEER_DELIVERY))
                        {
                            if (me->IsWithinDistInMap(who, 5.0f))
                            {
                                who->ToPlayer()->KilledMonsterCredit(53574);
                                who->RemoveAurasDueToSpell(SPELL_BEER_DELIVERY);
                            }
                        }
                    }
                }
            }
    };
};

void AddSC_ironforge()
{
    new npc_daryl_riknussun();
}
