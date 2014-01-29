#include "SRASConnection.h"
#include "websocket.h"

#define BUFFER_SIZE 0xFFFF

SRASConnection::SRASConnection(int socket) : m_socket(socket)
{
    m_ws_frameType = WS_INCOMPLETE_FRAME;
    m_ws_state = WS_STATE_OPENING;

    m_currentReceivedFrame.clear();
    m_currentSendFrame.clear();
}

int SRASConnection::ReadyRead()
{
    char buffer[BUFFER_SIZE];
    handshake hs;

    uint8 *frameBuffer;
    uint64 dataSize;

    int received = recv(m_socket, &buffer, BUFFER_SIZE, 0);

    if(received == -1)
        return -1;

    m_currentReceivedFrame.append(&buffer, received);

    if(m_ws_state = WS_STATE_OPENING)
    {
        m_ws_frameType = wsParseHandshake(m_currentReceivedFrame.contents(), m_currentReceivedFrame.size(), &hs);

    }
    else
    {
        m_ws_frameType = wsParseInputFrame(m_currentReceivedFrame.contents(), m_currentReceivedFrame.size(), &frameBuffer, &dataSize);
    }

    if(m_ws_frameType == WS_INCOMPLETE_FRAME)
        return 0;

    if(m_ws_frameType == WS_ERROR_FRAME)
    {
        if(m_ws_state == WS_STATE_OPENING)
        {
            ByteBuffer buff;
            buff << "HTTP/1.1 400 Bad Request\r\n";
            //TODO : Send the packet
        }

        return -1;
    }

    if(m_ws_frameType == WS_EMPTY_FRAME)
    {
        m_currentReceivedFrame.clear();
        return 0;
    }

    if(m_ws_state == WS_STATE_OPENING)
    {
        if(m_ws_frameType = WS_OPENING_FRAME)
        {
            uint8 *outBuffer;
            uint64 outLenght;
            wsGetHandshakeAnswer(&hs, outBuffer, &outLenght);

            ByteBuffer outPacket;
            outPacket.append(outBuffer, outLenght);

            //TODO : Send the packet

            m_ws_state = WS_STATE_NORMAL;
        }
    }

    if(m_ws_frameType == WS_CLOSING_FRAME)
    {
        if(m_ws_state == WS_STATE_CLOSING)
            return -1; //Acquittement du client
        else
        {
            uint8 *outBuffer;
            uint64 outLenght;
            wsMakeFrame(0, 0, outBuffer, &outLenght, WS_CLOSING_FRAME);
            ByteBuffer outPacket;
            outPacket.append(outBuffer, outLenght);

            //TODO : Send the packet

            return -1;
        }
    }

    m_currentReceivedFrame.clear();

    return 0;
}

int SRASConnection::ReadySend()
{
    if(m_currentSendFrame.size() == 0) //Est-on pret a envoyer un nouveau message
    {
        if(m_queuedMessage.empty())
            return 0;

        ByteBuffer newFrameData = m_queuedMessage.front();

        uint8 frameBuffer[BUFFER_SIZE];
        uint64 frameSize;

        wsMakeFrame(newFrameData.contents(), newFrameData.size(), (uint8*)&frameBuffer, &frameSize, WS_BINARY_FRAME); //On créer le message
        m_currentSendFrame.append(&frameBuffer, frameSize);
    }

    int sentByte = send(m_socket, m_currentSendFrame.contents(), m_currentSendFrame.size(), 0); //On l'envoi

    if(sentByte == -1) //S'il y a une erreur...
    {
        if(errno == EAGAIN || errno == EWOULDBLOCK) //...et que l'erreur est est un signal de blocage qui indique que la socket est occupe...
            return 0; //...on retourne sans rien dire et on retente au prochain coup...
        else //...sinon...
            return -1; //...on renvoie une erreur
    }

    if(sentByte == m_currentSendFrame.size()) //Si le message a ete correctement envoye, alors on est pret a en envoyer un autre
    {
        m_currentSendFrame.clear();
    }
    else if(sentByte < m_currentSendFrame.size()) //Si le message a ete partiellement envoyé
    {
        uint8 frameBuffer[BUFFER_SIZE];
        uint32 bufferSize = m_currentSendFrame.size()-sentByte;
        memcpy(&frameBuffer, m_currentSendFrame.contents()+sentByte, bufferSize); //On efface ce qui a ete envoye et on renvoie le reste au prochain coup
        m_currentSendFrame.clear();
        m_currentSendFrame.append(&frameBuffer, bufferSize);
    }
    else //This should never happen (on a envoye plus d'octet que le message n'en contient)
        return -1;

    return 0;
}

void SRASConnection::SendPacket(ByteBuffer buffer)
{
    m_queuedMessage.push(buffer);
}
