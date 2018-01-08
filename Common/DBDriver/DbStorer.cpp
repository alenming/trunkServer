#include "DbStorer.h"

using namespace std;

CDbStorer::CDbStorer(string dbname, string user, string pwd, string addr, int port):
m_DbName(dbname),
m_DbUser(user),
m_DbPwd(pwd),
m_DbAddress(addr),
m_DbPort(port),
m_ResultPtr(NULL),
m_RowRecord(NULL),
m_CntState(ESTORER_NONE)
{
    m_ConnectPtr = NULL;
	m_ConnectPtr = mysql_init(m_ConnectPtr);

    memset(m_DataStr, 0, sizeof(m_DataStr));

	if(NULL == m_ConnectPtr)
	{
		printf("DBError: CDbStorer mysql_init failed! %s : %d\n", m_DbAddress.c_str(), m_DbPort);
	}
	else
	{
		//自动重连
		mysql_options(m_ConnectPtr,MYSQL_OPT_RECONNECT, "1");//如果是mysql 5.1.6+，则在connect之前执行一次就够了

		//连接
		Connect();
	}
}


CDbStorer::CDbStorer():
m_DbName(""),
m_DbUser(""),
m_DbPwd(""),
m_DbAddress("127.0.0.1"),
m_DbPort(3306),
m_ResultPtr(NULL),
m_RowRecord(NULL),
m_CntState(ESTORER_NONE)
{
    m_ConnectPtr = NULL;
	m_ConnectPtr = mysql_init(m_ConnectPtr);

	if(NULL == m_ConnectPtr)
	{
		printf("DBError: CDbStorer mysql_init failed! %s : %d\n", m_DbAddress.c_str(), m_DbPort);
	}
	else
	{
		mysql_options(m_ConnectPtr,MYSQL_OPT_RECONNECT, "1");
	}
}

CDbStorer::~CDbStorer()
{
	//与数据库服务器断开连接
	Disconnect();
}

//设置数据库名称
void CDbStorer::SetDbName(string db)
{
	m_DbName = db;
}

//设置操作者的帐号
void CDbStorer::SetDbUser(string user)
{
	m_DbUser = user;
}

//设置操作者的密码
void CDbStorer::SetDbPwd(string pwd)
{
	m_DbPwd = pwd;
}

//设置数据库服务器的地址
void CDbStorer::SetDbAddress(string addr)
{
	m_DbAddress = addr;
}

//设置数据库服务器的端口
void CDbStorer::SetDbPort(int port)
{
	m_DbPort = port;
}

//与数据库服务器建立连接
bool CDbStorer::Connect()
{
	if(!CheckDBPtr())
		return false;

	if(mysql_real_connect(m_ConnectPtr, m_DbAddress.c_str(), m_DbUser.c_str(), m_DbPwd.c_str(), m_DbName.c_str(), m_DbPort, NULL, 0))
	{
		mysql_set_character_set(m_ConnectPtr, "UTF8");//推荐使用的设置方法，与mysql的连接断开自动重连后仍能保持设置的编码格式 范围：connection
		m_CntState = ESTORER_CONCATENATE;
		return true;
	}
	else
	{
		printf("DBError: CDbStorer::Connect() Connection failed %d: %s\n",
		     mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}
}

bool CDbStorer::ReConnect(std::string dbname, std::string user, std::string pwd, std::string addr, int port)
{
    //1.更新信息
    m_DbName = dbname;
    m_DbUser = user;
    m_DbPwd = pwd;
    m_DbAddress = addr;
    m_DbPort = port;

    //2.关闭
    if (NULL != m_ConnectPtr)
    {
        mysql_close(m_ConnectPtr);
        m_ConnectPtr = NULL;
		m_CntState = ESTORER_NONE;
    }    

    m_ConnectPtr = mysql_init(NULL);


	printf("CDbStorer::ReConnect() Called !!\n");


    //3.重连
    return Connect();
}

//与数据库服务器断开已建立的连接
bool CDbStorer::Disconnect()
{
	if(!CheckDBPtr())
		return false;
	if (!FreeResult())
		return false;
		
	mysql_close(m_ConnectPtr);
    m_ConnectPtr = NULL;
	m_CntState = ESTORER_DISCONNEXION;
	printf("CDbStorer::Disconnect() Called !!\n");
	return true;
}

//执行SQL语句
bool CDbStorer::Execute(const char* sql,emExecuteType execType/* = EXECTYPE_SELECT*/)
{
	if(!CheckDBPtr())
		return false;
	if (!FreeResult())
		return false;
	if (m_CntState == ESTORER_CONCATENATE && !Ping())
		return false;

	if(0 == mysql_real_query(m_ConnectPtr, sql, strlen(sql)))
	{
		//printf("Execute sql success %s : %d\n", m_DbAddress.c_str(), m_DbPort);
		if (execType == EXECTYPE_SELECT && !StoreResult()) //查询语句，需要获取结果集
		{
			return false;
		}
	}
	else
	{
		printf("SQLError:[%s]\n",sql);
		printf("DBError: CDbStorer::Execute() sql error %d: %s\n",mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}
	return true;
}

//测试已建立连接是否断开
bool CDbStorer::Ping()
{
	if(0 == mysql_ping(m_ConnectPtr))
	{
		return true;
	}
	else
	{
		printf("DBError: CDbStorer::Ping() Ping bad %d: %s\n", mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;	
	}
}

//设置数据库连接选项
bool CDbStorer::SetOption(enum mysql_option opt, const void* arg)
{
	if(!CheckDBPtr())
		return false;
		
	if(0 == mysql_options(m_ConnectPtr, opt, arg))
	{
		return true;
	}
	else
	{
		return false;
	}	
}

//事务开始
bool CDbStorer::TransactionStart()
{
	if(0 != mysql_real_query( m_ConnectPtr, "START TRANSACTION",(unsigned long)strlen("START TRANSACTION") ) )//或者mysql_query("START TRANSACTION");
	{
		printf("DBError: CDbStorer::TransactionBegin() Error %d: %s\n", mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}
	return true;
}
//事务提交
bool CDbStorer::TransactionCommit()
{
	if(0 != mysql_real_query( m_ConnectPtr, "COMMIT",(unsigned long)strlen("COMMIT") ) )
	{
		printf("DBError: CDbStorer::TransactionCommit() Error %d: %s\n", mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}
	return true;
}
//事务回滚
bool CDbStorer::TransactionRollback()
{
	if(0 != mysql_real_query( m_ConnectPtr, "ROLLBACK",(unsigned long)strlen("ROLLBACK") ) )
	{
		printf("DBError: CDbStorer::TransactionRollback() Error %d: %s\n", mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}
	return true;
}
//获取SQL语句影响的行数
unsigned long CDbStorer::GetAffectedCount()
{
	if(!CheckDBPtr())
		return 0;

	return (int)mysql_affected_rows(m_ConnectPtr) ;//得到受影响的行数
}
//获取记录总数
unsigned long CDbStorer::GetRowCount()
{
	return  (int)mysql_num_rows(m_ResultPtr);
}

int CDbStorer::GetFieldLength(unsigned int index)
{
    return mysql_fetch_lengths(m_ResultPtr)[index];
}

//获取插入的自增长ID
int CDbStorer::GetLastInsertID()
{
	return static_cast<int>(mysql_insert_id(m_ConnectPtr));
}

//获取数据库操作结果集
bool CDbStorer::StoreResult()
{
	if(!CheckDBPtr())
		return false;
		
	m_ResultPtr = mysql_store_result(m_ConnectPtr);
	if(NULL == m_ResultPtr)
	{
		//printf("DBError: CDbStorer::StoreResult() get store result error %d: %s\n", mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}

	else
	{
		return true;
	}
}

//释放数据库操作结果集
bool CDbStorer::FreeResult()
{
	if(NULL != m_ResultPtr)
	{
		mysql_free_result(m_ResultPtr);
		m_ResultPtr = NULL;
		m_RowRecord = NULL;
		return true;
	}
	return true;
}

//获取一行查询结果
bool CDbStorer::GetRowResult()
{
	m_RowRecord = mysql_fetch_row(m_ResultPtr);
	if(NULL == m_RowRecord)
	{
		//printf("DBError: CDbStorer::GetRowResult() fetch row error %d: %s\n",
		//	mysql_errno(m_ConnectPtr), mysql_error(m_ConnectPtr));
		return false;
	}
	else
	{
		return true;
	}
}

//获取字段的数值
char* CDbStorer::GetFieldValue(unsigned int index)
{
	if(!CheckDBPtr())
		return NULL;
		
	//外部需检测index的值
	return m_RowRecord[index];
}

//获取字段数目
int	CDbStorer::GetFieldCount()
{
	if(!CheckDBPtr())
		return 0;
	
	return mysql_field_count(m_ConnectPtr);
}

bool CDbStorer::CheckDBPtr()
{
	if(NULL == m_ConnectPtr)
	{
		printf("DBError: CDbStorer::CheckDBPtr() m_ConnectPtr Is NULL\n");
		return false;
	}

	return true;
}

char* CDbStorer::GetDataStr(void* obj, unsigned int objSize)
{
    //转换后最多需要的内存空间
    unsigned int needSize = objSize*2 + 2;
    memset(m_DataStr, 0, sizeof(m_DataStr));

    //空间足够的情况下才进行填充
    if (needSize <= sizeof(m_DataStr))
    {
        mysql_real_escape_string(m_ConnectPtr, m_DataStr, (char*)obj, objSize);
    }
    else
    {
        //内存不足，数据太大
        printf("Warning: GetDataStr ObjSize Is Too Large %u Buffer Size %lu\n",
            needSize, sizeof(m_DataStr));
    }

    return m_DataStr;
}
