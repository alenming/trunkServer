#include "KxLog.h"
#include "KxCore.h"

#define MAX_BUF 512

using namespace std;

KxLogger* KxLogger::m_Instance = NULL;

KxLogger::KxLogger()
:m_LogLevel(0),
m_LevelOption(true),
m_TimeOption(true),
m_DateOption(true),
m_FileOption(false),
m_FuncOption(false),
m_LineOption(false)
{

}

KxLogger::~KxLogger()
{
    clearHandler();
}

KxLogger* KxLogger::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new KxLogger();
    }

    return m_Instance;
}

void KxLogger::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

bool KxLogger::addHandler(int handleId, IKxLogHandler* handler)
{
    if (NULL == handler)
    {
        return false;
    }

    map<int, IKxLogHandler*>::iterator iter = m_Handlers.find(handleId);
    if (iter != m_Handlers.end())
    {
        return false;
    }

    handler->setHandleId(handleId);
    m_Handlers[handleId] = handler;
    return true;
}

bool KxLogger::addHandler(IKxLogHandler* handler)
{
    if (NULL == handler)
    {
        return false;
    }

    map<int, IKxLogHandler*>::iterator iter = m_Handlers.find(handler->getHandleId());
    if (iter != m_Handlers.end())
    {
        return false;
    }

    m_Handlers[handler->getHandleId()] = handler;
    return true;
}

bool KxLogger::removeHandler(int handleId)
{
    map<int, IKxLogHandler*>::iterator iter = m_Handlers.find(handleId);
    if (iter != m_Handlers.end())
    {
        iter->second->onExit();
        delete iter->second;
        m_Handlers.erase(iter);
        return true;
    }

    return false;
}

IKxLogHandler* KxLogger::getHandler(int id)
{
    map<int, IKxLogHandler*>::iterator iter = m_Handlers.find(id);
    if (iter != m_Handlers.end())
    {
        return iter->second;
    }

    return NULL;
}

void KxLogger::clearHandler()
{
    for (map<int, IKxLogHandler*>::iterator iter = m_Handlers.begin();
        iter != m_Handlers.end(); ++iter)
    {
        iter->second->onExit();
        delete iter->second;
    }

    m_Handlers.clear();
}

void KxLogger::writelog(int tag, int level, const char* file, const char* func, int line, const char* log, ...)
{

#ifdef _DEBUG
    if (level >= m_LogLevel)
    {
        char szBuf[MAX_BUF];
        va_list ap;
        va_start(ap, log);
        vsnprintf_s(szBuf, MAX_BUF, log, ap);
        va_end(ap);

        updateFormat(level, szBuf, file, func, line);
        map<int, IKxLogHandler*>::iterator iter = m_Handlers.begin();
        for (; iter != m_Handlers.end(); ++iter)
        {
            // 与为ture的才执行
            if (iter->second->getTag() & tag)
            {
                if (1 == tag)
                {
                    iter->second->onLog(level, m_StringFormat);
                }
                else
                {
                    // tag 如果不是默认值1的话, 自己处理前缀格式
                    iter->second->onLog(level, szBuf);
                }
            }            
        }
    }
#endif
}

void KxLogger::clearLog()
{
    map<int, IKxLogHandler*>::iterator iter = m_Handlers.begin();
    for (; iter != m_Handlers.end(); ++iter)
    {
        iter->second->onClean();
    }
}

void KxLogger::updateFormat(int level, const char* log, const char* file, const char* func, int line)
{
    char szBuf[MAX_BUF];
    m_StringFormat = "";

    if (m_LevelOption)
    {
        switch (level)
        {
        case KXLOG_DEBUG:
            m_StringFormat += "[Debug]";
            break;
        case KXLOG_INFO:
            m_StringFormat += "[Info]";
            break;
        case KXLOG_ERROR:
            m_StringFormat += "[Error]";
            break;

        default:
            break;
        }
    }

    if (m_DateOption || m_TimeOption)
    {
        time_t t = time(NULL);
		tm tmnow;
		stlocaltime(&t, &tmnow);

        if (m_DateOption)
        {
            snprintf(szBuf, sizeof(szBuf), "[%04d-%02d-%02d]",
                tmnow.tm_year + 1900, tmnow.tm_mon, tmnow.tm_mday);
            m_StringFormat += szBuf;
        }

        if (m_TimeOption)
        {
            snprintf(szBuf, sizeof(szBuf), "[%02d:%02d:%02d]",
                tmnow.tm_hour, tmnow.tm_min, tmnow.tm_sec);
            m_StringFormat += szBuf;
        }
    }

    if (m_FileOption && NULL != file)
    {
        m_StringFormat += "[file]" + string(file);
    }

    if (m_FuncOption && NULL != func)
    {
        m_StringFormat += "[func]" + string(func);
    }

    if (m_LineOption)
    {
        snprintf(szBuf, sizeof(szBuf), "[line]%d", line);
        m_StringFormat += szBuf;
    }

    m_StringFormat += " ";
    m_StringFormat += log;
}
