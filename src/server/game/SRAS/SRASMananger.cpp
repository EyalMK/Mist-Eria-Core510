#include "SRASMananger.h"

void SRASManager::DispatchEvent(SRASEvent *evt)
{
    m_eventQueue.add(evt);
}

bool SRASManager::GetNextEvent(SRASEvent *&evt)
{
    return m_eventQueue.next(evt);
}
