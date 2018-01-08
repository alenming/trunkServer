#include "RandomNumber.h"
#include <time.h>
#include <stdlib.h>

CRandomNumber::CRandomNumber()
{
	m_nType = RNT_TRUE;
	m_nIndex = 0;
	m_nGroup = 0;
}

int CRandomNumber::random()
{
	if (RNT_TRUE == m_nType)
	{
		srand((int)time(0));
		return rand() % PRN_SIZE;
	}
	else
	{
		int ret = PRN[m_nGroup][m_nIndex++];
		m_nIndex %= PRN_SIZE;
		return ret;
	}
}

int CRandomNumber::random(int min, int max)
{
	if (min >= max)
	{
		return 0;
	}
	return min + random() % (max - min);
}

float CRandomNumber::randomF()
{
	return random() / float(PRN_SIZE);
}

float CRandomNumber::randomF(int min, int max)
{
	if (min >= max)
	{
		return 0.0f;
	}
	return min + randomF() * (max - min);
}
