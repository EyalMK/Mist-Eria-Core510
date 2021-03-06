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


#ifndef DEF_SCARLET_MONASTERY_H
#define DEF_SCARLET_MONASTERY_H

uint32 const EncounterCount     = 5;

enum SMEncounter
{
    DATA_BOSS_THALNOS_THE_SOULRENDER        = 0,
    DATA_BOSS_BROTHER_KORLOFF               = 1,
    DATA_BOSS_HIGH_INQUISITOR_WHITEMANE     = 2,
    DATA_BOSS_COMMANDER_DURAND              = 3,
    DATA_NPC_TRAQUEUR_INTRO                 = 4
};

enum CreatureIds
{
    NPC_THALNOS_THE_SOULRENDER          = 59789,
    NPC_BROTHER_KORLOFF                 = 59223,
    NPC_HIGH_INQUISITOR_WHITEMANE		= 3977,
    NPC_COMMANDER_DURAND                = 60040,
    NPC_TRAQUEUR_INTRO                  = 200013
};

enum GameObjects
{
    GO_THALNOS_GATE         = 211844,
    GO_KORLOFF_GATE         = 104591,
    GO_WHITEMANE_GATE       = 210563
};

#endif
