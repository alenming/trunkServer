#include "KxTCPUnit.h"
#include "KxMemPool.h"
#include "KxLog.h"

#define BUFF_SIZE 4096
#define MAX_TCP_PKG_LEN 1<<16

namespace KxServer {

char* KxTCPUnit::s_RecvBuffer = NULL;

KxTCPUnit::KxTCPUnit()
: m_Socket(NULL)
, m_SendBuffer(NULL)
, m_RecvBuffer(NULL)
, m_SendBufferLen(0)
, m_RecvBufferLen(0)
, m_SendBufferOffset(0)
, m_RecvBufferOffset(0)
{
    // 分配全局的接收缓冲区
    if (NULL == s_RecvBuffer)
    {
        s_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(BUFF_SIZE));
    }
}

KxTCPUnit::~KxTCPUnit()
{
    KXSAFE_RELEASE(m_Socket);
    KXSAFE_RELEASE(m_Poller);
    KXSAFE_RECYCLE(m_SendBuffer, m_SendBufferLen);
    KXSAFE_RECYCLE(m_RecvBuffer, m_RecvBufferLen);

	// 先清空列表中缓存的数据
    kxBufferNode* node = m_BufferList.head();
    while (NULL != node)
    {
        kxMemMgrRecycle(node->buffer, node->len);
        node = node->next;
    }
	// 清空列表
	m_BufferList.clear();
}

bool KxTCPUnit::init()
{
    m_PollType = KXPOLLTYPE_IN;
    m_Socket = new KxSock();
    return m_Socket->init(KXSOCK_TCP);
}

// 发送数据
int KxTCPUnit::sendData(const char* buffer, unsigned int len)
{
    int ret = 0;
    // 1. 当待发送列表为空时说明前面没有数据待发送，可以发送
    // 2. 否则当要发送的buffer等于m_SendBuffer + m_SendBufferOffset时说明是onSend的自动发送，可以发送
    if (m_BufferList.isEmpty()
        || (m_SendBuffer != NULL && buffer == (m_SendBuffer + m_SendBufferOffset)))
    {
        ret = m_Socket->send(buffer, len);
        if (ret < 0)
        {
            int errorNo = m_Socket->getSockError();
            if (SOCKERR_RW_RETRIABLE(errorNo))
            {
                ret = 0;
            }
            else
            {
                KXLOGERROR("error: KxTCPUnit::sendData %d send datalen %d faile, errno %d", getCommId(), len, errorNo);
                // 如果是onSend调用的，则不执行onError
                if (m_Poller != NULL && m_Poller->getCurrentPollObject() != this)
                {
                    retain();
                    m_Poller->removeCommObject(this);
                    onError();
                    release();
                }
                return ret;
            }
        }
    }

    // 如果ret < len，说明有数据未发送完——要发送的长度小于实际发送的长度
    // 前面已经过滤了发送异常，这里将要发送的数据缓存，并依次发送
    if (ret < static_cast<int>(len))
    {
        // 如果发送的不是m_SendBuffer中的数据，追加到m_BufferList尾部
        if (buffer != (m_SendBuffer + m_SendBufferOffset))
        {
            len -= ret;
            // 未能一次性发送完的日志
            KXLOGDEBUG("warn: socket %d send %d data ret %d, cache unsend data!!!",
                getCommId(), len + ret, ret);
            char* buf = static_cast<char*>(kxMemMgrAlocate(len));
            memcpy(buf, buffer + ret, len);
            m_BufferList.pushBack(buf, len);
			KXLOGDEBUG("warn: socket %d changePollType m_PollType %d, cache unsend data!!!", getCommId(), m_PollType);
            addPollType(KXPOLLTYPE_OUT);
        }
        // 如果是m_SendBuffer中的数据，说明是在onSend回调中发送的
        // onSend处理了再次发送失败或没发送完的逻辑
    }

    return ret;
}

// 接收数据
int KxTCPUnit::recvData(char* buffer, unsigned int len)
{
    int ret = m_Socket->recv(buffer, len);
    if (ret <= 0)
    {
        if (m_Socket->isSockBlockError())
        {
            ret = 0;
        }
        else
        {
            if (m_Poller != NULL && m_Poller->getCurrentPollObject() != this)
            {
                retain();
                m_Poller->removeCommObject(this);
                onError();
                release();
            }
            ret = -1;
        }
    }
    return ret;
}

// 接收到数据时触发的回调，由IKxCommPoller调用
int KxTCPUnit::onRecv()
{
    memset(s_RecvBuffer, 0, BUFF_SIZE);
    int requestLen = 0;
    int state = 0;
    int ret = m_Socket->recv(s_RecvBuffer, BUFF_SIZE);
    if (ret == BUFF_SIZE)
    {
        state = 1;
    }
	
    // 如果是非阻塞
    if (ret < 0 && m_Socket->isSockBlockError())
    {
        return 0;
    }
    else if (ret <= 0)
    {
		KXLOGERROR("warn: ret <= 0 %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
        return -1;
    }
    else if (NULL != m_ProcessModule && ret > 0)
    {
		//KxLogger::getInstance()->HexDumpImp(s_RecvBuffer, ret);
        char* processBuf = s_RecvBuffer;
        char* stickBuf = NULL;

        // 如果有半包，先拼接到半包的后面，注意newsize
        if (NULL != m_RecvBuffer)
        {
            unsigned int newsize = ret;
            // 如果接到的包有多余的内容，为stickBuf赋值，并调整newsize，使其只拷贝该包的剩余内容到半包中
            if ((m_RecvBufferLen - m_RecvBufferOffset) < static_cast<unsigned int>(ret))
            {
                newsize = m_RecvBufferLen - m_RecvBufferOffset;
                stickBuf = processBuf + newsize;
            }
            memcpy(m_RecvBuffer + m_RecvBufferOffset, processBuf, newsize);
            //KXLOGDEBUG("contact half package to m_RecvBuffer, buffer offset %d buffer size %d ", m_RecvBufferOffset, m_RecvBufferLen);
            ret += m_RecvBufferOffset;
            m_RecvBufferOffset += newsize;
            processBuf = m_RecvBuffer;
        }

        // 完整包长度判断
        requestLen = m_ProcessModule->processLength(processBuf, ret);
        if (requestLen <= 0 || requestLen > MAX_TCP_PKG_LEN)
        {
            KXLOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
            return -1;
        }

        // 如果不足以处理，且没有半包，则为半包分配空间
        if (ret < requestLen && NULL == m_RecvBuffer)
        {
            m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(requestLen));
            m_RecvBufferLen = requestLen;
            m_RecvBufferOffset = ret;
            memcpy(m_RecvBuffer, processBuf, ret);
            return state;
        }
        // 这是一种复杂的情况...，当半包变大
        // 首先需要保证原先的半包正常拼接，然后再处理
        else if (NULL != stickBuf)
        {
            // 分配一块新的内存来缓存更大的半包
            char* oldRecvBuffer = m_RecvBuffer;
            unsigned int oldRecvBufferLen = m_RecvBufferLen;
            m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(requestLen));

            // 先将原先半包内容复制到新内存中
            memcpy(m_RecvBuffer, oldRecvBuffer, oldRecvBufferLen);
            kxMemMgrRecycle(oldRecvBuffer, oldRecvBufferLen);

            // 将后面额外的半包内容追加
            // 如果半包还是不能处理，变成了一个更大的半包，则进一步拼接后返回
            if (ret < requestLen)
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, ret - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = ret;
                KXLOGERROR("warn: something terrible, you are so lucky");
            }
            // 如果半包可以处理了，但后面还有更多内容
            else
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, requestLen - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = requestLen;
                // 处理m_RecvBuffer，并将stickBuf进行偏移
                processBuf = m_RecvBuffer;
                stickBuf += (requestLen - oldRecvBufferLen);
                KXLOGERROR("warn: another terrible, you are so lucky too");
            }
        }

        // 粘包处理
        while (ret >= requestLen)
        {
            m_ProcessModule->processLogic(processBuf, requestLen, this);
            //KXLOGDEBUG("process stick package, ret %d requestLen %d processBuf %x\n", ret, requestLen, processBuf);
            processBuf += requestLen;

            if (NULL != m_RecvBuffer)
            {
                processBuf = stickBuf;
                kxMemMgrRecycle(m_RecvBuffer, m_RecvBufferLen);
                m_RecvBuffer = NULL;
                m_RecvBufferOffset = m_RecvBufferLen = 0;
            }

            ret -= requestLen;
            if (ret > 0 && NULL != processBuf)
            {
                requestLen = m_ProcessModule->processLength(processBuf, ret);
                if (requestLen <= 0 || requestLen > MAX_TCP_PKG_LEN)
                {
                    KXLOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
					//KxLogger::getInstance()->HexDumpImp(processBuf, requestLen);
                    return -1;
                }
                //半包缓存
                else if (ret < requestLen)
                {
                    m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(requestLen));
                    m_RecvBufferLen = requestLen;
                    m_RecvBufferOffset = ret;
                    memcpy(m_RecvBuffer, processBuf, ret);
                    return state;
                }
            }
        }
    }

    return state;
}

// 数据可被发送时触发的回调，由IKxCommPoller调用
int KxTCPUnit::onSend()
{
    // 将缓存列表中的数据逐个发送，直到发送完或触发EAGAIN等非阻塞错误
again:
    // 将m_BufferList的第一个节点放到m_SendBuffer中
    if (NULL == m_SendBuffer)
    {
        // 取出下一个待发送的缓存数据
        kxBufferNode* node = m_BufferList.next();
        if (NULL != node)
        {
            m_SendBuffer = node->buffer;
            m_SendBufferLen = node->len;
            m_SendBufferOffset = 0;
            delete node;
			KXLOGDEBUG("warn: KxTCPUnit::onSend() NULL != node fd %d", getCommId());
        }
        else
        {
            // 无数据需要发送
            changePollType(KXPOLLTYPE_IN);
			KXLOGDEBUG("warn: socket %d KxTCPUnit::onSend() NULL = node m_PollType %d", getCommId(), m_PollType);
            return 0;
        }
    }

    // 应该发送的数据
    int sendLen = m_SendBufferLen - m_SendBufferOffset;
    int len = sendData(m_SendBuffer + m_SendBufferOffset, sendLen);
	KXLOGDEBUG("warn: KxTCPUnit::onSend() fd %d sendlen %d success", getCommId(), len);
    if (len >= sendLen)
    {
        //回收已经发送出去的内存
        kxMemMgrRecycle(m_SendBuffer, m_SendBufferLen);
        kxBufferNode* node = m_BufferList.head();
        m_SendBuffer = NULL;
        m_SendBufferLen = m_SendBufferOffset = 0;

        if (NULL == node)
        {
            changePollType(KXPOLLTYPE_IN);
        }
        else
        {
            goto again;
        }
    }
    else if (len < sendLen && len >= 0)
    {
        m_SendBufferOffset += len;
        addPollType(KXPOLLTYPE_OUT);
		KXLOGDEBUG("warn: socket %d KxTCPUnit::onSend() m_PollType %d", getCommId(), m_PollType);    
    }

    return len;
}

}