#ifndef __MEMORY_OPERATION_H
#define __MEMORY_OPERATION_H

#include <stdio.h>
#include <memory.h>

class CMemoryOperation
{
public:
	CMemoryOperation(unsigned int buffersize);
	~CMemoryOperation();
	
	template<typename T>
	bool WriteData(T& data)
	{
		if (!CheckBufferSize(sizeof(T)))
		{
			return false;
		}
		else
		{
			*reinterpret_cast<T*>(m_pBuffer + m_nDataLength) = data;
			m_nDataLength += sizeof(T);
			return true;
		}
	}

	bool WriteData(int data)
	{
		if (!CheckBufferSize(sizeof(int)))
		{
			return false;
		}
		else
		{
			*reinterpret_cast<int *>(m_pBuffer + m_nDataLength) = data;
			m_nDataLength += sizeof(int);
			return true;
		}
	}

	bool WriteData(const void* data, unsigned int length);

	char* GetBuffer(){ return m_pBuffer; }

	unsigned int GetDataLenth(){ return m_nDataLength; }

private:
	bool CheckBufferSize(int length);

	char* m_pBuffer;
	unsigned int m_nDataLength;
	unsigned int m_nBufferSize;
};

#endif