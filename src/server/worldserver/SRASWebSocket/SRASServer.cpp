#ifdef linux

#include "SRASServer.h"
#include "websocket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "World.h"
#include <fcntl.h>

#define BUFFER_SIZE 0xFFFF

typedef int SOCKET;

SRASServer::SRASServer()
{
}

SRASServer::~SRASServer()
{

}

void SRASServer::run()
{
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

    if(server == -1)
    {
        sLog->outError(LOG_FILTER_SRAS, "Can't create server socket");
        return;
    }

    struct sockaddr_in serverAddr_in;
    serverAddr_in.sin_family = AF_INET;
    serverAddr_in.sin_addr.s_addr = INADDR_ANY;
    serverAddr_in.sin_port = htons(443);

    if(bind(server, (struct sockaddr*) &serverAddr_in, sizeof(serverAddr_in)) == -1)
    {
        sLog->outError(LOG_FILTER_SRAS, "Can't bind SRAS socket to port 443");
        return;
    }

    if(listen(server, 1))
    {
        sLog->outError(LOG_FILTER_SRAS, "Can't listen on port 443");
        return;
    }

    fd_set readSet, writeSet;

    while(!World::IsStopped())
    {
        //On remplie tout les ensemble de socket

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(server, &readSet);

        for(std::map<int, SRASConnection*>::iterator i = m_openedConnection.begin() ; i != m_openedConnection.end() ; ++i)
        {
            FD_SET(i->first, &readSet);
            FD_SET(i->first, &writeSet);
        }

        int ndfs = m_openedConnection.size() + 1 /*server socket*/ + 1 /*Comme la doc le dit*/;

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int resultat = select(ndfs, &readSet, &writeSet, NULL, &timeout);

        if(resultat < 0)
            break;

        if(resultat)
        {
            if(FD_ISSET(server, &readSet)) //Nouveau client
            {
                sockaddr_in remoteAddr;

                size_t sinSize = sizeof(remoteAddr);

                SOCKET newClient = accept(server, (sockaddr*)&remoteAddr, (socklen_t*)&sinSize);
                fcntl(newClient, F_SETFL, O_NONBLOCK);

                if(newClient == -1)
                {
                    sLog->outError(LOG_FILTER_SRAS, "Nouvelle connexion invalide");
                    break;
                }
                else
                {
                    sLog->outDebug(LOG_FILTER_SRAS, "Nouvelle connexion");
                    SRASConnection *sras = new SRASConnection(newClient);
                    m_openedConnection[newClient] = sras;
                }
            }

            for(std::map<int, SRASConnection*>::iterator i = m_openedConnection.begin() ; i != m_openedConnection.end() ; ++i)
            {
                SOCKET cli = i->first;

                int err;

                if(FD_ISSET(cli, &readSet))
                {
                    err = i->second->ReadyRead();
                }
                if(err != -1 && FD_ISSET(cli, &writeSet))
                {
                    err = i->second->ReadySend();
                }

                if(err == -1) //Erreur
                {
                    close(cli);
                    delete i->second;
                    m_openedConnection.erase(cli);
                }
            }
        }
    }
}

#endif
