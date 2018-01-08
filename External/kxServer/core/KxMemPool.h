/*
*   MemPool 内存池
*   减少内存碎片，提高分配内存效率，内存复用
*
*   2013-04-14 By 宝爷
*
*/
#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include <set>
#include <map>
#include <list>

namespace KxServer {

    typedef std::map<unsigned int, std::list<void*>*>   MemML;
    typedef std::map<unsigned int, std::set<void*>*>    MemMS;

    class KxMemPool
    {
    public:
        KxMemPool();
        virtual ~KxMemPool();

        // 分配大小为size的内存,实际分配内存大小可能会更大,因为内部对size进行了对齐
        void* memAlocate(unsigned int size);

        // 回收内存,将MemPool分配的内存回收,回收的内存大小为size,成功返回0
        int memRecycle(void* mem, unsigned int size);

        //显示内存池当前状态
        void memDumpInfo();

        // 自动对齐适配要分配的内存块大小
        // 默认最小分配32字节
        // 33-64分配64字节
        // 65-128分配128字节
        // 129-256分配256字节，以此类推
        inline unsigned int memFitSize(unsigned int size);

    private:
        //获取一次性分配的数据数量
        inline unsigned int memFitCounts(unsigned int size);

        //扩展内存池
        int memExtend(unsigned int size, std::list<void*>* plist, std::set<void*>* pset);

        //扩展新内存池
        int memExtendNewSize(unsigned int size);

        //释放指定大小的内存
        int memReleaseWithSize(unsigned int size);

        //需要释放多少块内存
        unsigned int memRelsaseCount(unsigned int size, unsigned int freecount, unsigned int stubcount);

        //根据当前水位等状态自动检测是否释放内存
        int memAutoRelease(unsigned size, std::list<void*>* plist, std::set<void*>* pset);

    private:
        MemML m_Free;                                           //空闲内存块
        MemMS m_Stub;                                           //内存块存根

        unsigned int m_AlocatedSize;                            //已分配大小
        unsigned int m_WaterMark;                               //标记水位
        unsigned int m_MinAlocateSize;                          //分配最小内存大小为 1 << m_MinAlocateSize

        static const unsigned int MAX_WATER_MARK = 1 << 30;     //最大水位
        static const unsigned int MAX_POOL_SIZE = 1 << 31;      //内存池最大容量 

        static const unsigned int MEM_BASE_COUNT = 32;          //每次分配数量
        static const unsigned int MEM_SIZE_MIN = 1 << 20;       //内存标量——小 1M
        static const unsigned int MEM_SIZE_MID = 1 << 26;       //内存标量——中 64M
        static const unsigned int MEM_SIZE_BIG = 1 << 27;       //内存标量——大 128M
    };

    class KxMemManager
    {
    public:
        static KxMemManager* getInstance();
        static void destroy();

        // 分配大小为size的内存,实际分配内存大小可能会更大
        void* memAlocate(unsigned int size);
        void* memAlocate(unsigned int size, unsigned int& outSize);

        // 回收内存,将MemPool分配的内存回收,回收的内存大小为size,成功返回0
        int memRecycle(void* mem, unsigned int size);

        //显示内存池当前状态
        void memDumpInfo();

    private:
        KxMemManager();
        ~KxMemManager();

    private:
        KxMemPool* m_MemPool;
        static KxMemManager* m_Instance;
    };

    inline void* kxMemMgrAlocate(unsigned int size)
    {
        return new char[size];
        //return KxMemManager::getInstance()->memAlocate(size);
    }

    inline void* kxMemMgrAlocate(unsigned int size, unsigned int& outSize)
    {
        outSize = size;
        return new char[size];
        //return KxMemManager::getInstance()->memAlocate(size, outSize);
    }

    inline int kxMemMgrRecycle(void* mem, unsigned int size)
    {
        delete[] (char*)mem;
        return size;
        //return KxMemManager::getInstance()->memRecycle(mem, size);
    }

#define KXSAFE_RECYCLE(mem, size) if(NULL != (mem)) { kxMemMgrRecycle(mem, size); }

}

#endif