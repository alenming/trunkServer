

#ifndef __USER_ACTION_MANAGER_H__
#define __USER_ACTION_MANAGER_H__

#include <map>
#include <list>

#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
#include<string.h>
#endif

#include "UserActionListener.h"

class CUserActionManager
{
public:
    CUserActionManager();
    ~CUserActionManager();

    void AddActionListener(int actID, IUserActionListener* listener);
    bool UserAction(int actID, void *data, int len);

	bool CleanAction();

private:
    std::map<int, std::list<IUserActionListener*> > m_mapAllListener;
};

#endif // !__USER_ACTION_MANAGER_H__