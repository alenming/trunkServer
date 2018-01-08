#include "MemoryOperation.h"

CMemoryOperation::CMemoryOperation(unsigned int buffersize) : m_pBuffer(NULL), m_nBufferSize(0), m_nDataLength(0)
{
	m_pBuffer = new char[buffersize];
	if (NULL != m_pBuffer)
	{
		m_nBufferSize = buffersize;
	}
}

CMemoryOperation::~CMemoryOperation()
{
	if (NULL != m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
}

bool CMemoryOperation::WriteData(const void* data, unsigned int length)
{
	if (!CheckBufferSize(length))
	{
		return false;
	}
	else
	{
		memcpy(m_pBuffer + m_nDataLength, data, length);
		m_nDataLength += length;
		return true;
	}
}

bool CMemoryOperation::CheckBufferSize(int length)
{
	if (m_nDataLength + length > m_nBufferSize)
	{
		return false;
	}
	return true;
}
