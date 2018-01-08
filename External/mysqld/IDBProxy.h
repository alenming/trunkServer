#ifndef __IDBPROXY_H__
#define __IDBPROXY_H__



#ifdef DBPROXY_EXPORTS
#define DBPROXY_API __declspec(dllexport)
#else
#define DBPROXY_API __declspec(dllimport)
#endif


#ifdef _DEBUG
#define DBPROXY_DLL_NAME	TEXT("DBProxy_d.dll")			//组件 DLL 名字
#else
#define DBPROXY_DLL_NAME	TEXT("DBProxy.dll")			//组件 DLL 名字
#endif



// 数据库代理请求返回回调接口
struct IDBProxyRetSink
{
	// 数据库代理请求返回回调函数
	// nReturn: 参考 IDBEngine.h 里面数据库请求返回值定义
	// pOutData: 输出数据缓冲区指针
	// nOutLen: 输出数据缓冲区长度
	virtual void OnRet(long nIndex, int nUserID, int nRequestID, char *pInData, int nInLen,
		int nReturn, char *pOutData, int nOutLen) = 0;

};

// 数据库代理用户处理回调
struct IDBProxyUserSink
{
	// 添加用户处理回调函数
	virtual void OnAddUser(DWORD dwServerID, long lUserID, long lActorID) = 0;
	// 删除用户处理回调函数
	virtual void OnDelUser(DWORD dwServerID, long lUserID) = 0;

};

// 数据库代理接口
struct IDBProxy
{
	// 销毁数据库代理
	virtual void Release(void) = 0;

	// 请求数据库
	// nIndex: 请求流水号, 唯一对应每个请求
	// nRequestID: 请求ID
	// nUserID: 发出本请求的用户ID, 用于取模进行负载均衡
	// pInData: 输入数据缓冲区
	// nInDataLen: 输入数据缓冲区长度
	// pRet: 请求返回回调接口
	virtual BOOL Request(long nIndex, int nRequestID, int nUserID,
		char *pInData, int nInDataLen, IDBProxyRetSink *pRet, DWORD dwServerID) = 0;

	//关闭服务器，用于处理缓存数据的保存
	virtual void CloseServer(DWORD dwServerID) = 0;
};

// 创建数据库代理(输出函数)
extern "C" __declspec(dllexport) BOOL CreateDBProxy(DWORD dwSaveTimes,IDBProxy **ppDBProxy, char *szKey,
													ILog *pTrace, ILogicThread *pLogicThread, ITimerAxis *pTimeAxis,
													IDBProxyUserSink *pUserSink);
typedef BOOL (__cdecl *ProcCreateDBProxy)(DWORD dwSaveTimes,IDBProxy **ppDBProxy, char *szKey,
										  ILog *pTrace, ILogicThread *pLogicThread, ITimerAxis *pTimeAxis,
										  IDBProxyUserSink *pUserSink);

// 数据库代理创建辅助类
class CDBProxyCreateHelper
{
public:
	// 构造函数
	CDBProxyCreateHelper()
	{
		m_hDll = NULL;
		m_pDBProxy = NULL;
	}

	// 析构函数
	~CDBProxyCreateHelper()
	{
		Close();
	}

	// 创建通用数据库引擎
	BOOL Create(DWORD dwSaveTimes,char *szKey, 
				ILog *pTrace,
				ILogicThread *pLogicThread,
				ITimerAxis *pTimeAxis, 
				IDBProxyUserSink *pUserSink)
	{
		if (m_hDll != NULL)
		{
			return FALSE;
		}

		try
		{
			m_hDll = ::LoadLibrary(DBPROXY_DLL_NAME);
			if (NULL == m_hDll)
			{
				throw "Cann't load DBProxy.dll!";
			}

			ProcCreateDBProxy pProc = NULL;
			pProc = (ProcCreateDBProxy)::GetProcAddress(m_hDll, "CreateDBProxy");
			if (NULL == pProc)
			{
				throw "Cann't get process CreateDBProxy!";
			}

			if (FALSE == pProc(dwSaveTimes,&m_pDBProxy, szKey, pTrace, pLogicThread,
				pTimeAxis, pUserSink))
			{
				throw "Create DBProxy error!";
			}

			return TRUE;
		}
		catch (LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
		}
		catch (...)
		{
			OutputDebugString("DBProxy unknow error!");
			OutputDebugString("\r\n");
		}

		return FALSE;
	}

	// 释放数据库代理
	void Close(void)
	{
		if (m_pDBProxy != NULL)
		{
			m_pDBProxy->Release();
			m_pDBProxy = NULL;
		}

		if (m_hDll != NULL)
		{
			::FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}

public:
	IDBProxy	*m_pDBProxy;	// 数据库代理指针

private:
	HINSTANCE	m_hDll;			// 动态库句柄

};

#endif // __IDBPROXY_H__
