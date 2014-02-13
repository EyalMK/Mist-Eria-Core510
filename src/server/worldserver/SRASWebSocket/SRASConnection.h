#ifndef SRAS_CO_H
#define SRAS_CO_H
#include "ByteBuffer.h"

#define CHECK_SECURITY(sec) if (m_security < sec) {close(m_scoket); return;}

enum SRASOpcodeClient //Client => Server
{
    AUTH_CHALLENGE = 1,
    TICKET_LIST = 2,
    SEARCH_QUERY = 3
};

enum SRASOpcodeServer //Server => Client
{
    AUTH_RESPONSE = 1,
    TICKET_RESP = 2,
    SEARCH_RESPONSE = 3
};

class SRASPacket
{
    //TODO : Add protection to prevent having the same packet in read and write mode
public:

    SRASPacket() {}

    SRASPacket(const SRASPacket &other)
    {
        m_tokens = other.m_tokens;
        ite = other.ite;
        m_dataToSend.flush();
        m_dataToSend << other.m_dataToSend;
    }

    SRASPacket(std::string datas)
    {
        ite = -1;
        std::string currentToken;

        const unsigned char *raw = (const unsigned char*)datas.c_str();

        for(int i = 0 ; i < datas.size() ; i++)
        {
            if(raw[i] == '|')
            {
                m_tokens.push_back(currentToken);
                currentToken.clear();
            }
            else
                currentToken += raw[i];
        }

        m_tokens.push_back(currentToken);

        /*sLog->outDebug(LOG_FILTER_SRAS, "Dump des tokens");

        for(int i = 0 ; i < m_tokens.size() ; i++)
        {
            sLog->outDebug(LOG_FILTER_SRAS, "%s", m_tokens[i].c_str());
        }*/
    }

    int toInt()
    {
        return atoi(m_tokens[ite].c_str());
    }

    std::string toString()
    {
        return m_tokens.at(ite);
    }

    std::string next()
    {
        ite++;
        if(ite < m_tokens.size())
            return m_tokens[ite];
        else
            throw std::string("SRAS packet owerflow");
    }

    template <class T>
    void add(T data)
    {
        if(m_dataToSend.str().empty())
            m_dataToSend << data;
        else
            m_dataToSend << "|" << data;
    }

    std::string finalize()
    {
        return m_dataToSend.str();
    }

private:
    std::vector<std::string> m_tokens;
    int ite;

    std::stringstream m_dataToSend;
};

class SRASConnection
{
public:
    SRASConnection(int socket);
    ~SRASConnection();

    int ReadyRead(); //Called when new data are available for reading
    int ReadySend();
    void SendPacket(std::string buffer);

    int HandlePacket(int opcode, SRASPacket pkt);

    int getSocket() {return m_socket;}

    //Handlers

    void AuthChallenge(SRASPacket pkt);    

    void TicketList();

    void SearchQuery(SRASPacket pkt);

private:

    int m_socket;

    ByteBuffer m_currentReceivedFrame;
    int m_ws_state;
    int m_ws_frameType;

    ByteBuffer m_currentSendFrame;

    std::queue<std::string> m_queuedMessage;

    std::string m_user;
    uint32 m_security;
};

#endif
