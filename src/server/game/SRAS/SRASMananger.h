#ifndef SRAS_MGR_H
#define SRAS_MGR_H

#include "ace/Singleton.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "LockedQueue.h"

struct SRASEvent
{
    uint32 type;
    void *dataPtr;
};

struct SRASEventWorldMessage
{
    std::string senderName;
    std::string msg;
    uint32 guidLow;
    bool isGM;
    bool a2;
};

enum SRASEventType
{
    SRAS_EVENT_WORLD_MESSAGE = 1
};

//This class is used to communicate with all SRASConnection
class SRASManager
{
    friend class ACE_Singleton<SRASManager, ACE_Recursive_Thread_Mutex>;
private:
    SRASManager() {}
    ~SRASManager() {}

    ACE_Based::LockedQueue<SRASEvent*, ACE_Thread_Mutex> m_eventQueue;

public:
    void DispatchEvent(SRASEvent *evt);
    bool GetNextEvent(SRASEvent *&evt);
};

#define sSRASMgr ACE_Singleton<SRASManager, ACE_Recursive_Thread_Mutex>::instance()

#endif
