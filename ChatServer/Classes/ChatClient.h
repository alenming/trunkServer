/*
*
*   客户端连接对象
*
*/
#ifndef __CHAT_CLIENTER_H__
#define __CHAT_CLIENTER_H__

#include "KXServer.h"
#include<map>

class CChatClient :
    public KxServer::KxTCPClienter
{
public:
	CChatClient();
	virtual ~CChatClient(void);

    virtual void retain() 
    {
        KxObject::retain();
    }
    virtual void release()
    {
        KxObject::release();
    }

    bool setConServer(int nKey, int nValue);
    bool sendDataToServer(int nGroupID, int nKey, char *pszContext, int nLen);
    bool sendDataToGroupServer(int nGroupID, char *pszContext, int nLen);
    bool sendDataToAllServer(char *pszContext, int nLen);
    int getRouteValue(int nKey);

    virtual int onRecv();
    virtual void setTimer();
    virtual void onTimer();
    virtual void clean();

    inline void setPermission(unsigned int permission) { m_Permission = permission; }
    inline int getPermission() { return m_Permission; }
    inline void setUserId(unsigned int userId) { m_UserId = userId; }
    inline unsigned int getUserId() { return m_UserId; }
    inline void setGuestId(unsigned int guestId) { m_GuestId = guestId; }
    inline unsigned int getGuestId() { return m_GuestId; }

private:
    int m_Permission;	                    // 权限
    unsigned int m_GuestId;	                // 未验证的访客ID
    unsigned int m_UserId;                  // 已验证的用户ID    
    std::map<int, int> m_MapConKeyValue;    // 获取对应的key相应的值
    KxServer::KxTimerCallback<CChatClient>* m_TimerCallBack;
};

#endif //__CHAT_CLIENTER_H__
