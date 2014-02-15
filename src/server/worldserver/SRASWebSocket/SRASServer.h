#ifndef SRAS_SERVER_H
#define SRAS_SERVER_H

#include "Common.h"
#include "SRASConnection.h"
#include <ace/Reactor.h>
#include <vector>
#include "SRASMananger.h"

class SRASServer : public ACE_Based::Runnable
{
public:
    SRASServer();
    ~SRASServer();

    void run();

    void ProcessEvent(SRASEvent *);
    void SendToAll(std::string pkt);

    private:
    std::list<SRASConnection*> m_openedConnection;
};

#endif
