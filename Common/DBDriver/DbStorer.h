#ifndef _DB_STORER_H_
#define _DB_STORER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <mysql.h>

#include "IStorer.h"

//事物回滚 尚未封装
enum emExecuteType
{
	EXECTYPE_NONE,		//
	EXECTYPE_INSERT,	//插入 
	EXECTYPE_DELETE,	//删除 
	EXECTYPE_UPDATE,	//更新
	EXECTYPE_SELECT,	//查询
};

enum emDBStorerState
{
	ESTORER_NONE,				//
	ESTORER_CONCATENATE,		// 连接中
	ESTORER_DISCONNEXION,		// 断开连接
};

class CDbStorer : public IStorer
{
public: 
    //构造函数与析构函数
    CDbStorer();
    CDbStorer(std::string dbname, std::string user, std::string pwd, std::string addr, int port);
    ~CDbStorer();
    
    //设置操作
    //设置数据库名称
    void SetDbName(std::string db);
    //设置操作者的帐号
    void SetDbUser(std::string user);
    //设置操作者的密码
    void SetDbPwd(std::string pwd);
    //设置数据库服务器的地址
    void SetDbAddress(std::string addr);
    //设置数据库服务器的端口
    void SetDbPort(int port);
    
    //处理操作
    //与数据库服务器建立连接
    bool Connect();
    //与数据库服务器重新建立连接
    bool ReConnect(std::string dbname, std::string user, std::string pwd, std::string addr, int port);
    //关闭数据库连接
    bool Disconnect();
    //执行SQL语句
    bool Execute(const char* sql,emExecuteType execType = EXECTYPE_SELECT);
    //测试已建立连接是否断开
    bool Ping();
    //检查数据库是否存在
    bool IsDbExist(std::string db);
    //设置数据库连接选项
    bool SetOption(enum mysql_option opt, const void* arg);
	//事务开始
	bool TransactionStart();
	//事务提交
	bool TransactionCommit();
	//事务回滚
	bool TransactionRollback();
	//事务结束 Mysql 语法里面木有事务结束的操作？
    //bool TransactionEnd();

    //操作结果
	//获取SQL语句影响的行数
	unsigned long GetAffectedCount();
    //获取记录总数
    unsigned long GetRowCount();
    //获取数据库操作结果集
    bool StoreResult();
    //释放数据库操作结果集
    bool FreeResult();
    //获取一行查询结果
    bool GetRowResult();
    //获取字段的数值
    char* GetFieldValue(unsigned int index);
    //获取字段数目
    int GetFieldCount();
    //获取字段长度
    int GetFieldLength(unsigned int index);
	//获取插入的自增长ID
	int GetLastInsertID();

    //要插入数据结构的时候,调用此函数,用返回的数据插到Sql语句中,PS 一次Sql查询只能调一次
    char* GetDataStr(void* obj, unsigned int objSize);

private:
    //检测数据库指针是否初始化
    inline bool CheckDBPtr();

private:
    std::string   m_DbName;             //数据库名称      
    std::string   m_DbUser;             //数据库操作者的帐号
    std::string   m_DbPwd;              //数据库操作者的密码
    std::string   m_DbAddress;          //数据库服务器的地址
    int           m_DbPort;             //数据库服务器的端口 
    MYSQL*        m_ConnectPtr;         //连接集句柄
    MYSQL_RES*    m_ResultPtr;          //结果集句柄 
    MYSQL_ROW     m_RowRecord;          //记录集句柄
	unsigned char m_CntState;			//连接状态
    char          m_DataStr[7872];      //要插入的数据结构 (InteractModel 缓冲最大需要 3932*2 + 2)
 };

#endif //_DB_STORER_H_
