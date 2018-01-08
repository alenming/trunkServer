/*
* 数据缓冲区对象
* 用于序列化和反序列化
*
* 2015-5-21 By 宝爷
*/
#ifndef __BUFFERDATA_H__
#define __BUFFERDATA_H__

#include <stdio.h>
#include <memory.h>
#include <assert.h>

//序列化插入一个值,反序列化在相应的位置读取该值,并判断是否匹配,如果匹配则没有错位, 不匹配输出信息
//序列化一个值,用来测试匹配
#define SERIALIZE_MATCH(_buffData_)					\
{													\
	(_buffData_).writeData(12345678);				\
}
//测试反序列化数值是否匹配
#define UN_SERIALIZE_MATCH(_buffData_)											\
{																				\
	int _readValue_ = 0;														\
	(_buffData_).readData(_readValue_);											\
	LOG("readValue= %d, File %s, Line %d", _readValue_, __FILE__, __LINE__);	\
	assert("反序列化不匹配" && (12345678 ==_readValue_));							\
}

#define SERIALIZE_MATCH_VALUE(_buffData_, _value_)	\
{													\
	(_buffData_).writeData(_value_);				\
}
//测试反序列化数值是否匹配
#define UN_SERIALIZE_MATCH_VALUE(_buffData_, _value_)							\
{																				\
	int _readValue_ = 0;														\
	(_buffData_).readData(_readValue_);											\
	LOG("readValue= %d, File %s, Line %d", _readValue_, __FILE__, __LINE__);	\
	assert("反序列化不匹配" && (_value_ ==_readValue_));							\
}

class CBufferData
{
public:
    CBufferData();
    virtual ~CBufferData();

    bool init(int bufferSize);
    bool init(char* buffer, unsigned int dataLength);

    void clean();

    template<typename T>
    bool writeData(T data)
    {
        if (!checkBufferSize(sizeof(data)))
        {
            return false;
        }
        else
        {
            *reinterpret_cast<T*>(m_Buffer + m_DataLength) = data;
            m_DataLength += sizeof(data);
            return true;
        }
    }
    bool writeData(const void* data, unsigned int length);
    // 更新一块空的buffer，主要用于占位使用
    bool writeEmptyBuffer(unsigned int length);

    template<typename T>
    bool readData(T& data)
    {
        if (!checkDataLength(sizeof(data)))
        {
            return false;
        }

        data = *reinterpret_cast<T*>(m_Buffer + m_Offset);
        m_Offset += sizeof(data);
        return true;
    }
    bool readData(void* data, unsigned int dataSize);

    inline void resetOffset() { m_Offset = 0; }
    inline void updateOffset(unsigned int offset) { m_Offset = offset; }
    inline unsigned int getOffset() { return m_Offset; }
    inline unsigned int getDataLength() { return m_DataLength; }
    inline unsigned int getBufferSize() { return m_BufferSize; }

	char *getBuffer(){ return m_Buffer; };

private:
    bool checkDataLength(unsigned int dataLength);
    bool checkBufferSize(unsigned int newLength);

private:
    bool m_IsInit;              // 防止重复初始化
    bool m_IsReadMode;          // 读写模式
    unsigned int m_Offset;      // 当前读取偏移
    unsigned int m_DataLength;  // 当前实际数据长度
    unsigned int m_BufferSize;  // 缓冲区大小
    char* m_Buffer;
};

// 可序列化与可反序列化接口
class ISerializable
{
    virtual bool serialize(CBufferData& data) = 0;
    virtual bool unserialize(CBufferData& data) = 0;
};

#endif
