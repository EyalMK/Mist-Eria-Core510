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

#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Vehicle.h"
#include "Player.h"
#include "Log.h"
#include "ObjectAccessor.h"

void WorldSession::HandleDismissControlledVehicle(WorldPacket &recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Recvd CMSG_DISMISS_CONTROLLED_VEHICLE");

	/*
    uint64 vehicleGUID = _player->GetCharmGUID();

    if (!vehicleGUID)                                       // something wrong here...
    {
        recvData.rfinish();                                // prevent warnings spam
        return;
    }

    MovementInfo mi;
    _player->ReadMovementInfo(recvData, &mi);

    _player->m_movementInfo = mi;
	*/

    _player->ExitVehicle();
}

void WorldSession::HandleChangeSeatsOnControlledVehicle(WorldPacket& recvData)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Recvd CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE");

    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
    {
        recvData.rfinish();                                // prevent warnings spam
        return;
    }

    VehicleSeatEntry const* seat = GetPlayer()->GetVehicle()->GetSeatForPassenger(GetPlayer());
    if (!seat->CanSwitchFromSeat())
    {
        recvData.rfinish();                                // prevent warnings spam
        sLog->outError(LOG_FILTER_NETWORKIO, "HandleChangeSeatsOnControlledVehicle, Opcode: %u, Player %u tried to switch seats but current seatflags %u don't permit that.",
            recvData.GetOpcode(), GetPlayer()->GetGUIDLow(), seat->m_flags);
        return;
    }

    switch (recvData.GetOpcode())
    {
        case CMSG_REQUEST_VEHICLE_PREV_SEAT: // to find value for this opcode
            GetPlayer()->ChangeSeat(-1, false);
            break;
        case CMSG_REQUEST_VEHICLE_NEXT_SEAT: // to find value for this opcode
            GetPlayer()->ChangeSeat(-1, true);
            break;
        case CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE:
        {
            float x, y, z; // need to test the order of floats 
            recvData >> z;
            int8 seatId;
			recvData >> y;
            recvData >> seatId;
            recvData >> x;
            
            uint64 accessory;        // accessory vehicle guid
            recvData >> accessory;

            if (!accessory)
                GetPlayer()->ChangeSeat(-1, seatId > 0); // prev/next
            else if (Unit* vehUnit = Unit::GetUnit(*GetPlayer(), accessory))
            {
                if (Vehicle* vehicle = vehUnit->GetVehicleKit())
                    if (vehicle->HasEmptySeat(seatId))
                        vehUnit->HandleSpellClick(GetPlayer(), seatId);
            }
            break;
        }
        case CMSG_REQUEST_VEHICLE_SWITCH_SEAT: // to find value for this opcode
        {
            uint64 guid;        // current vehicle guid
            recvData.readPackGUID(guid);

            int8 seatId;
            recvData >> seatId;

            if (vehicle_base->GetGUID() == guid)
                GetPlayer()->ChangeSeat(seatId);
            else if (Unit* vehUnit = Unit::GetUnit(*GetPlayer(), guid))
                if (Vehicle* vehicle = vehUnit->GetVehicleKit())
                    if (vehicle->HasEmptySeat(seatId))
                        vehUnit->HandleSpellClick(GetPlayer(), seatId);
            break;
        }
        default:
            break;
    }
}

void WorldSession::HandleEnterPlayerVehicle(WorldPacket& data)
{
    // Read guid
    ObjectGuid guid;

	guid[5] = data.ReadBit();
 	guid[0] = data.ReadBit();
 	guid[3] = data.ReadBit();
 	guid[1] = data.ReadBit();
 	guid[7] = data.ReadBit();
 	guid[2] = data.ReadBit();
 	guid[6] = data.ReadBit();
 	guid[4] = data.ReadBit();

	data.ReadByteSeq(guid[7]);
 	data.ReadByteSeq(guid[6]);
 	data.ReadByteSeq(guid[0]);
 	data.ReadByteSeq(guid[3]);
 	data.ReadByteSeq(guid[5]);
 	data.ReadByteSeq(guid[4]);
 	data.ReadByteSeq(guid[1]);
 	data.ReadByteSeq(guid[2]);

    if (Player* player = ObjectAccessor::FindPlayer(guid))
    {
        if (!player->GetVehicleKit())
            return;
        if (!player->IsInRaidWith(_player))
            return;
        if (!player->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            return;

        _player->EnterVehicle(player);
    }
}

void WorldSession::HandleEjectPassenger(WorldPacket& data)
{
    Vehicle* vehicle = _player->GetVehicleKit();
    if (!vehicle)
    {
        data.rfinish();                                     // prevent warnings spam
        sLog->outError(LOG_FILTER_NETWORKIO, "HandleEjectPassenger: Player %u is not in a vehicle!", GetPlayer()->GetGUIDLow());
        return;
    }

    uint64 guid;
    data >> guid;

    if (IS_PLAYER_GUID(guid))
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %u tried to eject player %u from vehicle, but the latter was not found in world!", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        if (!player->IsOnVehicle(vehicle->GetBase()))
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %u tried to eject player %u, but they are not in the same vehicle", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(player);
        ASSERT(seat);
        if (seat->IsEjectable())
            player->ExitVehicle();
        else
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %u attempted to eject player %u from non-ejectable seat.", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
    }

    else if (IS_CREATURE_GUID(guid))
    {
        Unit* unit = ObjectAccessor::GetUnit(*_player, guid);
        if (!unit) // creatures can be ejected too from player mounts
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %u tried to eject creature guid %u from vehicle, but the latter was not found in world!", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        if (!unit->IsOnVehicle(vehicle->GetBase()))
        {
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %u tried to eject unit %u, but they are not in the same vehicle", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(unit);
        ASSERT(seat);
        if (seat->IsEjectable())
        {
            ASSERT(GetPlayer() == vehicle->GetBase());
            unit->ExitVehicle();
        }
        else
            sLog->outError(LOG_FILTER_NETWORKIO, "Player %u attempted to eject creature GUID %u from non-ejectable seat.", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
    }
    else
        sLog->outError(LOG_FILTER_NETWORKIO, "HandleEjectPassenger: Player %u tried to eject invalid GUID "UI64FMTD, GetPlayer()->GetGUIDLow(), guid);
}

void WorldSession::HandleRequestVehicleExit(WorldPacket& /*recvData*/)
{
    sLog->outDebug(LOG_FILTER_NETWORKIO, "WORLD: Recvd CMSG_REQUEST_VEHICLE_EXIT");

    if (Vehicle* vehicle = GetPlayer()->GetVehicle())
    {
        if (VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(GetPlayer()))
        {
            if (seat->CanEnterOrExit())
                GetPlayer()->ExitVehicle();
            else
                sLog->outError(LOG_FILTER_NETWORKIO, "Player %u tried to exit vehicle, but seatflags %u (ID: %u) don't permit that.",
                GetPlayer()->GetGUIDLow(), seat->m_ID, seat->m_flags);
        }
    }
}
