#include "LogConsoleHandler.h"

bool KxLogConsoleHandler::onLog(int level, const std::string& log)
{
    printf("%s\n", log.c_str());
    return true;
}
