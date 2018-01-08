#include "Player.h"
#include "GameUserManager.h"
#include "Room.h"
#include "CommStructs.h"
#include "PvpProtocol.h"

CPlayer::CPlayer(void)
: m_bIsConnecting(false)
{
}

CPlayer::~CPlayer(void)
{
}

bool CPlayer::initWithData(char* data, int& len)
{
    if (len < sizeof(PvpEnterRoomSS))
    {
        return false;
    }
    PvpEnterRoomSS* enterSS = reinterpret_cast<PvpEnterRoomSS*>(data);
    m_ContinueWin = enterSS->continueWin;
    m_Integral = enterSS->integral;
    m_MMR = enterSS->mmr;
    m_DayWinTimes = enterSS->dayWinTimes;

    PlayerData* playerData = reinterpret_cast<PlayerData*>(
        data + sizeof(PvpEnterRoomSS));
    if (CPlayerModel::initByUserData(playerData, EBATTLE_PVP) < 0)
    {
        return false;
    }

    m_bIsConnecting = true;
    return true;
}

void CPlayer::setIsConnecting(bool connecting)
{
	m_bIsConnecting = connecting;
}

bool CPlayer::isConnecting()
{
	return m_bIsConnecting;
}
