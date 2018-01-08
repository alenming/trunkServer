#include "RoomManager.h"
#include "Room.h"
#include "KxCore.h"
#include "KxLog.h"

#define MAXROOMID		(0xffffffff >> 1)

using namespace std;

CRoomManager::CRoomManager(void)
    :m_nRoomId(0)
{
}

CRoomManager::~CRoomManager(void)
{
}

CRoomManager *CRoomManager::m_pInstance = NULL;
CRoomManager *CRoomManager::getInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CRoomManager;
    }

    return m_pInstance;
}

void CRoomManager::destroy()
{
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

bool CRoomManager::haveRoomWithUid(int uid)
{
	return NULL != getRoomWithUid(uid);
}

bool CRoomManager::haveRoomWithBattleId(int battleId)
{
	return NULL != getRoomWithBattleId(battleId);
}

CPvpRoom *CRoomManager::enterRoom(int uid, int battleId, int roomType)
{
	std::map<int, IRoom*>::iterator iter = m_PVPRoomMap.find(battleId);
	CPvpRoom *pRoom = NULL;
	m_UsersBattleId[uid] = battleId;
	if (iter == m_PVPRoomMap.end())
	{
		pRoom = new CPvpRoom();
		if (!pRoom->initRoom(battleId, 1, roomType))
		{
			delete pRoom;
			return NULL;
		}
		m_PVPRoomMap[battleId] = pRoom;
		return pRoom;
	}
	else
	{
		return dynamic_cast<CPvpRoom*>(m_PVPRoomMap[battleId]);
	}
	return NULL;
}

IRoom* CRoomManager::getRoomWithBattleId(int battleId)
{
	std::map<int, IRoom*>::iterator iter = m_PVPRoomMap.find(battleId);
	if (iter != m_PVPRoomMap.end())
	{
		return iter->second;
	}
	return NULL;
}

IRoom* CRoomManager::getRoomWithUid(int uid)
{
	std::map<int, int>::iterator iter = m_UsersBattleId.find(uid);
	if (iter != m_UsersBattleId.end())
	{
		return getRoomWithBattleId(iter->second);
	}
	return NULL;
}

void CRoomManager::removeWithBattleId(int battleId)
{
	std::map<int, IRoom*>::iterator iter = m_PVPRoomMap.find(battleId);
	if (iter != m_PVPRoomMap.end())
	{
		IRoom *pRoom = iter->second;
		
		if (pRoom->getRoomType() == PVPROOMTYPE_PVP)
		{
			CPvpRoom *pPvpRoom = dynamic_cast<CPvpRoom*>(pRoom);
			std::map<int, CPlayer*> &players = pPvpRoom->getRoomPlayers();
			for (std::map<int, CPlayer*>::iterator playerIter = players.begin();
				playerIter != players.end(); ++playerIter)
			{
				//玩家战斗列表
				int uid = playerIter->first;
				std::map<int, int>::iterator iter = m_UsersBattleId.find(uid);
				if (iter != m_UsersBattleId.end())
				{
					m_UsersBattleId.erase(iter);
				}
			}
			// 释放玩家, eventmanager等对象
			pPvpRoom->release();
		}
		m_PVPRoomMap.erase(iter);
	}
}
