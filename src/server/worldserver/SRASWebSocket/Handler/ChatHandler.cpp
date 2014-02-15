#include "SRASConnection.h"
#include "ChannelMgr.h"
#include "Channel.h"
#include "SRASMananger.h"
#include "ObjectDefines.h"

void SRASConnection::WorldMsgPosted(SRASPacket pkt)
{
    pkt.next();
    uint32 a2 = pkt.toInt();
    std::string msg = pkt.next();

    ChannelMgr *mgr;
    if(a2)
        mgr = ChannelMgr::forTeam(ALLIANCE);
    else
        mgr = ChannelMgr::forTeam(HORDE);

    Channel *world = mgr->GetChannel("world", 0, false);
    if(!world)
        return;

    world->SRASSay(MAKE_NEW_GUID(m_currentGuid, 0, HIGHGUID_PLAYER), msg);

    CharacterNameData const* name = sWorld->GetCharacterNameData(m_currentGuid);
    if(!name)
        return;

    SRASEvent *evt = new SRASEvent;
    evt->type = SRAS_EVENT_WORLD_MESSAGE;

    SRASEventWorldMessage *data = new SRASEventWorldMessage;
    data->a2 = a2;
    data->guidLow = m_currentGuid;
    data->isGM = 1;
    data->senderName = name->m_name;
    data->msg = msg;

    evt->dataPtr = (void*)data;

    sSRASMgr->DispatchEvent(evt);
}
