#ifndef SRAS_SERVER_H
#define SRAS_SERVER_H

#include "Common.h"
#include "SRASConnection.h"
#include <ace/Reactor.h>

class SRASServer : public ACE_Based::Runnable
{
public:
    SRASServer();
    ~SRASServer();

    void run();

    private:
    std::map<int, SRASConnection*> m_openedConnection;
};

#endif
