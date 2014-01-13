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


#ifndef DEF_SCARLET_HALLS_H
#define DEF_SCARLET_HALLS_H

uint32 const EncounterCount     = 3;

enum SMEncounter
{
    DATA_BOSS_HOUNDMASTER_BRAUN     = 0,
    DATA_BOSS_ARMSMASTER_HARLAN     = 1,
    DATA_BOSS_FLAMEWEAVER_KOEGLER   = 2
};

enum CreatureIds
{
    NPC_HOUNDMASTER_BRAUN           = 59303,
    NPC_ARMSMASTER_HARLAN           = 58632,
    NPC_FLAMEWEAVER_KOEGLER         = 59150
};

enum GameObjects
{
    GO_BRAUN_GATE           = 210840,
    GO_HARLAN_GATE          = 212160
};

#endif
