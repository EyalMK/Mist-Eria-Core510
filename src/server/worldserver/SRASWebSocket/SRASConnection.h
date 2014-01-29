#ifndef SRAS_CO_H
#define SRAS_CO_H
#include "ByteBuffer.h"

class SRASConnection
{
public:
    SRASConnection(int socket);

    int ReadyRead(); //Called when new data are available for reading
    int ReadySend();
    void SendPacket(ByteBuffer buffer);

private:

    int m_socket;

    ByteBuffer m_currentReceivedFrame;
    int m_ws_state;
    int m_ws_frameType;

    ByteBuffer m_currentSendFrame;

    std::queue<ByteBuffer> m_queuedMessage;
};

#endif
