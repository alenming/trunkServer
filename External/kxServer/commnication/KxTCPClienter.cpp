#include "KxTCPClienter.h"
#include "KxLog.h"

namespace KxServer {

KxTCPClienter::KxTCPClienter()
{
	//KXLOGERROR("TCP new clienter ref = %d", m_ReferenceCount);
}

KxTCPClienter::~KxTCPClienter()
{
}

bool KxTCPClienter::init(KXCOMMID fd)
{
    if (NULL == m_Socket)
    {
        m_Socket = new KxSock();
    }
    changePollType(KXPOLLTYPE_IN);
	KXLOGDEBUG("warn: socket %d KxTCPClienter::init m_PollType %d", getCommId(), m_PollType);
    if (m_Socket->init(fd))
    {
        m_Socket->setSockNonblock();
        m_Socket->setSockKeepAlive();
        return true;
    }
    return false;
}

void KxTCPClienter::retain()
{
	KxTCPUnit::retain();
	//KXLOGERROR("TCP retain clienter ref = %d", m_ReferenceCount);
}

void KxTCPClienter::release()
{
	KxTCPUnit::release();
	//KXLOGERROR("TCP release clienter ref = %d", m_ReferenceCount);
}

}
