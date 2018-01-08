#ifndef __LOG_CONSOLE_HANDLER_H__
#define __LOG_CONSOLE_HANDLER_H__

#include "KxLog.h"

class KxLogConsoleHandler : public IKxLogHandler
{
public:
    virtual bool onLog(int level, const std::string& log);
};

#endif
