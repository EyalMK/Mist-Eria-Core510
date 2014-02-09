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
    serverAddr_in.sin_port = htons(9090);

    if(bind(server, (struct sockaddr*) &serverAddr_in, sizeof(serverAddr_in)) == -1)
    {
        sLog->outError(LOG_FILTER_SRAS, "Can't bind SRAS socket to port 443");
        return;
    }

    if(listen(server, 20) == -1)
    {
        sLog->outError(LOG_FILTER_SRAS, "Can't listen on port 443");
        return;
    }

    fd_set readSet, writeSet;

    sLog->outInfo(LOG_FILTER_SRAS, "SRAS server ready");

    while(!World::IsStopped())
    {
        //On remplie tout les ensemble de socket

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(server, &readSet);

        for(std::list<SRASConnection*>::iterator i = m_openedConnection.begin() ; i != m_openedConnection.end() ; i++)
        {
            FD_SET((*i)->getSocket(), &readSet);
            FD_SET((*i)->getSocket(), &writeSet);
        }

        int ndfs = 8*(sizeof(readSet) + sizeof(writeSet));

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int resultat = select(ndfs, &readSet, NULL, NULL, &timeout);

        /*if(resultat < 0)
            break;*/

        if(resultat > 0)
        {
            //sLog->outInfo(LOG_FILTER_SRAS, "SRAS event");
            if(FD_ISSET(server, &readSet)) //Nouveau client
            {
                sockaddr_in remoteAddr;

                size_t sinSize = sizeof(remoteAddr);

                SOCKET newClient = accept(server, (sockaddr*)&remoteAddr, (socklen_t*)&sinSize);
                //fcntl(newClient, F_SETFL, O_NONBLOCK);

                if(newClient == -1)
                {
                    sLog->outError(LOG_FILTER_SRAS, "Nouvelle connexion invalide");                    ;
                }
                else
                {
                    sLog->outDebug(LOG_FILTER_SRAS, "Nouvelle connexion");
                    SRASConnection *sras = new SRASConnection(newClient);
                    m_openedConnection.push_back(sras);
                }
            }

            for(std::list<SRASConnection*>::iterator i = m_openedConnection.begin() ; i != m_openedConnection.end() ; i++)
            {
                SOCKET cli = (*i)->getSocket();

                int err = 0;

                if(FD_ISSET(cli, &readSet))
                {
                    err = (*i)->ReadyRead();
                    if(err == -1)
                        sLog->outDebug(LOG_FILTER_SRAS, "Erreur on receiving");
                }
                if(err != -1 && FD_ISSET(cli, &writeSet))
                {
                    err = (*i)->ReadySend();
                    if(err == -1)
                        sLog->outDebug(LOG_FILTER_SRAS, "Erreur on sending");
                }

                if(err == -1) //Erreur
                {
                    sLog->outDebug(LOG_FILTER_SRAS, "Erreur on I/O");
                    SRASConnection *sr = (*i);
                    m_openedConnection.erase(i);
                    delete sr;
                    break;
                }
            }
        }
    }

    close(server);

    for(std::list<SRASConnection*>::iterator i = m_openedConnection.begin() ; i != m_openedConnection.end() ; i++)
    {
        SRASConnection *sr = (*i);
        m_openedConnection.erase(i);
        delete sr;
    }

}

#endif
