#include "SRASConnection.h"
#include "World.h"
#include "Chat/Chat.h"
#include "Language.h"

void SRASConnection::ServerAnnounce(SRASPacket pkt)
{
    std::string msg = pkt.next();

    std::stringstream ss;
    ss << "[SERVER] " << msg;
    sWorld->SendServerMessage(SERVER_MSG_STRING, ss.str().c_str());
}
