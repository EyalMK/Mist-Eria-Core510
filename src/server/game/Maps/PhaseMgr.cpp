/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

#include "PhaseMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"

//////////////////////////////////////////////////////////////////
// Updating

PhaseMgr::PhaseMgr(Player* _player) : player(_player), phaseData(_player), _UpdateFlags(0)
{
    _PhaseDefinitionStore = sObjectMgr->GetPhaseDefinitionStore();
    _SpellPhaseStore = sObjectMgr->GetSpellPhaseStore();
}

void PhaseMgr::Update()
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Update");
    if (IsUpdateInProgress())
        return;
	
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Checking flags");
    if (_UpdateFlags & PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED) {
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : SendPhaseshitToPlayer");
        phaseData.SendPhaseshiftToPlayer();
	}

    if (_UpdateFlags & PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED) {
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : SendPhaseMaskToPlayer");
        phaseData.SendPhaseMaskToPlayer();
	}
	
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Leaving Update");
    _UpdateFlags = 0;
}

void PhaseMgr::RemoveUpdateFlag(PhaseUpdateFlag updateFlag)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : RemoveUpdateFlag %s", (uint32)updateFlag);
    _UpdateFlags &= ~updateFlag;

    if (updateFlag == PHASE_UPDATE_FLAG_ZONE_UPDATE)
    {
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : PHASE_UPDATE_FLAG_ZONE_UPDATE");
        // Update zone changes
        if (phaseData.HasActiveDefinitions())
        {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : RemoveUpdateFlag : HasActiveDefinitions");
            phaseData.ResetDefinitions();
            _UpdateFlags |= (PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED | PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED);
        }
		
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : RemoveUpdateFlag : Check Zone");
        if (_PhaseDefinitionStore->find(player->GetZoneId()) != _PhaseDefinitionStore->end()) {
			sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : RemoveUpdateFlag : Recalculate");
            Recalculate();
		}
    }
	
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Updating");
    Update();
}

/////////////////////////////////////////////////////////////////
// Notifier

void PhaseMgr::NotifyConditionChanged(PhaseUpdateData const& updateData)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : NotifyConditionChanged");
    if (NeedsPhaseUpdateWithData(updateData))
    {
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : NotifyConditionChanged : NeedsPhaseUpdateWithData = true");
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : NCC : Recalc");
        Recalculate();
		
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : NCC : Update");
        Update();
    }
}

//////////////////////////////////////////////////////////////////
// Phasing Definitions

void PhaseMgr::Recalculate()
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalculate");
    if (phaseData.HasActiveDefinitions())
    {
		sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalc : Clean active defs");
        phaseData.ResetDefinitions();
        _UpdateFlags |= (PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED | PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED);
    }

    sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalc : PhaseDefStore");

    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr != _PhaseDefinitionStore->end())
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
            if (CheckDefinition(&(*phase)))
            {
				sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalc : Good def found");
                phaseData.AddPhaseDefinition(&(*phase));

                if (phase->phasemask) {
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalc : Need phasemask");
                    _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
				}

                if (phase->phaseId || phase->terrainswapmap) {
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalc : Need PhaseShift");
                    _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;
				}

                if (phase->IsLastDefinition()) {
					sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Recalc : Last definition");
                    break;
				}
            }
}

inline bool PhaseMgr::CheckDefinition(PhaseDefinition const* phaseDefinition)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : Check definition");
    return sConditionMgr->IsObjectMeetToConditions(player, sConditionMgr->GetConditionsForPhaseDefinition(phaseDefinition->zoneId, phaseDefinition->entry));
}

bool PhaseMgr::NeedsPhaseUpdateWithData(PhaseUpdateData const updateData) const
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : NeedsPhaseUpdateWIthData");
    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr != _PhaseDefinitionStore->end())
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            ConditionList conditionList = sConditionMgr->GetConditionsForPhaseDefinition(phase->zoneId, phase->entry);
            for (ConditionList::const_iterator condition = conditionList.begin(); condition != conditionList.end(); ++condition)
                if (updateData.IsConditionRelated(*condition))
                    return true;
        }
    }
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : NPUWD : FALSE");
    return false;
}

//////////////////////////////////////////////////////////////////
// Auras

void PhaseMgr::RegisterPhasingAuraEffect(AuraEffect const* auraEffect)
{
    PhaseInfo phaseInfo;

    if (auraEffect->GetMiscValue())
    {
        _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
        phaseInfo.phasemask = auraEffect->GetMiscValue();
    }
    else
    {
        SpellPhaseStore::const_iterator itr = _SpellPhaseStore->find(auraEffect->GetId());
        if (itr != _SpellPhaseStore->end())
        {
            if (itr->second.phasemask)
            {
                _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
                phaseInfo.phasemask = itr->second.phasemask;
            }

            if (itr->second.terrainswapmap)
                phaseInfo.terrainswapmap = itr->second.terrainswapmap;
        }
    }

    phaseInfo.phaseId = auraEffect->GetMiscValueB();

    if (phaseInfo.NeedsClientSideUpdate())
        _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

    phaseData.AddAuraInfo(auraEffect->GetId(), phaseInfo);

    Update();
}

void PhaseMgr::UnRegisterPhasingAuraEffect(AuraEffect const* auraEffect)
{
    _UpdateFlags |= phaseData.RemoveAuraInfo(auraEffect->GetId());

    Update();
}

//////////////////////////////////////////////////////////////////
// Commands

void PhaseMgr::SendDebugReportToPlayer(Player* const debugger)
{
    ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_REPORT_STATUS, player->GetName().c_str(), player->GetZoneId(), player->getLevel(), player->GetTeamId(), _UpdateFlags);

    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr == _PhaseDefinitionStore->end())
        ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_NO_DEFINITIONS, player->GetZoneId());
    else
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            if (CheckDefinition(&(*phase)))
                ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_SUCCESS, phase->entry, phase->IsNegatingPhasemask() ? "negated Phase" : "Phase", phase->phasemask);
            else
                ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_FAILED, phase->phasemask, phase->entry, phase->zoneId);

            if (phase->IsLastDefinition())
            {
                ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_LAST_PHASE, phase->phasemask, phase->entry, phase->zoneId);
                break;
            }
        }
    }

    ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_LIST, phaseData._PhasemaskThroughDefinitions, phaseData._PhasemaskThroughAuras, phaseData._CustomPhasemask);

    ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_PHASEMASK, phaseData.GetPhaseMaskForSpawn(), player->GetPhaseMask());
}

void PhaseMgr::SetCustomPhase(uint32 const phaseMask)
{
    phaseData._CustomPhasemask = phaseMask;

    _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

    Update();
}

//////////////////////////////////////////////////////////////////
// Phase Data

uint32 PhaseData::GetCurrentPhasemask() const
{
    if (player->isGameMaster())
        return uint32(PHASEMASK_ANYWHERE);

    if (_CustomPhasemask)
        return _CustomPhasemask;

    return GetPhaseMaskForSpawn();
}

inline uint32 PhaseData::GetPhaseMaskForSpawn() const
{
    uint32 const phase = (_PhasemaskThroughDefinitions | _PhasemaskThroughAuras);
    return (phase ? phase : PHASEMASK_NORMAL);
}

void PhaseData::SendPhaseMaskToPlayer()
{
    // Server side update
    uint32 const phasemask = GetCurrentPhasemask();
    if (player->GetPhaseMask() == phasemask)
        return;

    player->SetPhaseMask(phasemask, false);

    if (player->IsVisible())
        player->UpdateObjectVisibility();
}

void PhaseData::SendPhaseshiftToPlayer()
{
    // Client side update
    std::set<uint32> phaseIds;
    std::set<uint32> terrainswaps;

    for (PhaseInfoContainer::const_iterator itr = spellPhaseInfo.begin(); itr != spellPhaseInfo.end(); ++itr)
    {
        if (itr->second.terrainswapmap)
            terrainswaps.insert(itr->second.terrainswapmap);

        if (itr->second.phaseId)
            phaseIds.insert(itr->second.phaseId);
    }

    // Phase Definitions
    for (std::list<PhaseDefinition const*>::const_iterator itr = activePhaseDefinitions.begin(); itr != activePhaseDefinitions.end(); ++itr)
    {
        if ((*itr)->phaseId)
            phaseIds.insert((*itr)->phaseId);

        if ((*itr)->terrainswapmap)
            terrainswaps.insert((*itr)->terrainswapmap);
    }

    player->GetSession()->SendSetPhaseShift(phaseIds, terrainswaps);
}

void PhaseData::AddPhaseDefinition(PhaseDefinition const* phaseDefinition)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : AddPhaseDef");
    if (phaseDefinition->IsOverwritingExistingPhases())
    {
        activePhaseDefinitions.clear();
        _PhasemaskThroughDefinitions = phaseDefinition->phasemask;
    }
    else
    {
        if (phaseDefinition->IsNegatingPhasemask())
            _PhasemaskThroughDefinitions &= ~phaseDefinition->phasemask;
        else
            _PhasemaskThroughDefinitions |= phaseDefinition->phasemask;
    }

    activePhaseDefinitions.push_back(phaseDefinition);
}

void PhaseData::AddAuraInfo(uint32 const spellId, PhaseInfo phaseInfo)
{
    if (phaseInfo.phasemask)
        _PhasemaskThroughAuras |= phaseInfo.phasemask;

    spellPhaseInfo[spellId] = phaseInfo;
}

uint32 PhaseData::RemoveAuraInfo(uint32 const spellId)
{
    PhaseInfoContainer::const_iterator rAura = spellPhaseInfo.find(spellId);
    if (rAura != spellPhaseInfo.end())
    {
        uint32 updateflag = 0;

        if (rAura->second.NeedsClientSideUpdate())
            updateflag |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

        if (rAura->second.NeedsServerSideUpdate())
        {
            _PhasemaskThroughAuras = 0;

            updateflag |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

            spellPhaseInfo.erase(rAura);

            for (PhaseInfoContainer::const_iterator itr = spellPhaseInfo.begin(); itr != spellPhaseInfo.end(); ++itr)
                _PhasemaskThroughAuras |= itr->second.phasemask;
        }

        return updateflag;
    }
    else
        return 0;
}

//////////////////////////////////////////////////////////////////
// Phase Update Data

void PhaseUpdateData::AddQuestUpdate(uint32 const questId)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : AddQuestUpdate, quest %u", questId);
    AddConditionType(CONDITION_QUESTREWARDED);
    AddConditionType(CONDITION_QUESTTAKEN);
    AddConditionType(CONDITION_QUEST_COMPLETE);
    AddConditionType(CONDITION_QUEST_NONE);

    _questId = questId;
}

bool PhaseUpdateData::IsConditionRelated(Condition const* condition) const
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : IsConditionRelated");
    switch (condition->ConditionType)
    {
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        case CONDITION_QUEST_COMPLETE:
        case CONDITION_QUEST_NONE:
            return condition->ConditionValue1 == _questId && ((1 << condition->ConditionType) & _conditionTypeFlags);
        default:
            return (1 << condition->ConditionType) & _conditionTypeFlags;
    }
}

bool PhaseMgr::IsConditionTypeSupported(ConditionTypes const conditionType)
{
	sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : IsConditionTypeSupported");
    switch (conditionType)
    {
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        case CONDITION_QUEST_COMPLETE:
        case CONDITION_QUEST_NONE:
        case CONDITION_TEAM:
        case CONDITION_CLASS:
        case CONDITION_RACE:
        case CONDITION_INSTANCE_INFO:
        case CONDITION_LEVEL:
            return true;
        default:
			sLog->outDebug(LOG_FILTER_NETWORKIO, "PHASE MGR : IsConditionTypeSupported : FALSE");
            return false;
    }
}
