#ifndef SRAS_SERVER_H
#define SRAS_SERVER_H

#include "Common.h"
#include "SRASConnection.h"
#include <ace/Reactor.h>
#include <vector>

class SRASServer : public ACE_Based::Runnable
{
public:
    SRASServer();
    ~SRASServer();

    void run();

    private:
    std::list<SRASConnection*> m_openedConnection;
};

#endif
