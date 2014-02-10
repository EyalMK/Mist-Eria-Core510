#include "SRASConnection.h"
#include "Tickets/TicketMgr.h"

void SRASConnection::TicketList()
{
    std::vector<GmTicket*> tickets = sTicketMgr->GetOpenedTicket();

    SRASPacket resp;

    resp.add(TICKET_LIST);
    resp.add(tickets.size());

    for(int i = 0 ; i < tickets.size() ; i++)
    {
        GmTicket *tkt = tickets[i];

        resp.add(tkt->GetId());
        resp.add(tkt->GetMessage());
        resp.add(tkt->GetPlayerName());
        resp.add(tkt->GetLastModifiedTime());
    }

    SendPacket(resp.finalize());
}
