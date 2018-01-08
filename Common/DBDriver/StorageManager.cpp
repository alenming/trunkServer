#include "StorageManager.h"
#include "MarkupSTL.h"
#include "DbStorer.h"
#include "RedisStorer.h"
#include "KxLog.h"
#include "KxCore.h"

using namespace std;

StorageManager::StorageManager()
{    
}

StorageManager::~StorageManager()
{
    //清除Storage对象数据
    MapStorage::iterator iter = m_mapStorage.begin();
    for(; iter != m_mapStorage.end();++iter)
    {
        if(iter->second != NULL)
        {
            delete iter->second;
            iter->second = NULL;
        }
    }

    m_mapStorage.clear();
}

StorageManager* StorageManager::m_pInstance = NULL;
StorageManager * StorageManager::getInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new StorageManager;
    }
    return m_pInstance;
}

void StorageManager::destroy()
{
    if(m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

bool StorageManager::InitWithXML(std::string xmlFile)
{
	CMarkupSTL conf;
	if (conf.Load(xmlFile.c_str()))
	{
		if (!conf.FindElem("module"))
		{
			return false;
		}

		conf.IntoElem();
		while (conf.FindElem("server"))
		{
			// 依次读取配置
			string dbtype = conf.GetAttrib("type");
			int storageId = 0;

			if (dbtype == "mysql")
			{
				//初始化MYSQL
				string ip = conf.GetAttrib("ip");
				string username = conf.GetAttrib("username");
				string password = conf.GetAttrib("password");
				string dbname = conf.GetAttrib("dbname");
				int port = atoi(conf.GetAttrib("port").c_str());
				storageId = atoi(conf.GetAttrib("modeltype").c_str());

				DBRule rule;
				rule.range_minvalue = atoi(conf.GetAttrib("rangemin").c_str());
				rule.range_maxvalue = atoi(conf.GetAttrib("rangemax").c_str());

				CDbStorer *dbStorer = dynamic_cast<CDbStorer *>(GetStorer(ip, port));
				if (NULL == dbStorer)
				{
					dbStorer = new CDbStorer();
					dbStorer->SetDbName(dbname);
					dbStorer->SetDbUser(username);
					dbStorer->SetDbPwd(password);
					dbStorer->SetDbAddress(ip);
					dbStorer->SetDbPort(port);
					if (dbStorer->Connect())
					{
						KXLOGDEBUG("connect to mysql ip=%s, port=%d success!", ip.c_str(), port);
					}
					else
					{
						KXLOGDEBUG("connect to mysql ip=%s, port=%d failed!", ip.c_str(), port);
						continue;
					}
				}
				rule.storer = dbStorer;
				InsertStorer(STORE_MYSQL, storageId, ip, port, rule);
				KXLOGDEBUG("insert mysql storer storageId=%d", storageId);
			}
			else if (dbtype == "tmem")
			{
				// 暂未用
			}
			else if (dbtype == "cache")
			{
				// 暂未用
			}
			else if (dbtype == "redis")
			{
				DBRule rule;
				rule.range_minvalue = atoi(conf.GetAttrib("rangemin").c_str());
				rule.range_maxvalue = atoi(conf.GetAttrib("rangemax").c_str());

				string ip = conf.GetAttrib("ip");
				int port = atoi(conf.GetAttrib("port").c_str());
				string passward = conf.GetAttrib("passward");
				storageId = atoi(conf.GetAttrib("modeltype").c_str());

				CRedisStorer *redisStorer = dynamic_cast<CRedisStorer *>(GetStorer(ip, port));
				if (NULL == redisStorer)
				{
					redisStorer = new CRedisStorer();
					if (SUCCESS == redisStorer->Connect(ip, port,passward))
					{
						KXLOGDEBUG("connect to redis ip=%s, port=%d success!", ip.c_str(), port);
					}
					else
					{
						KXLOGDEBUG("connect to redis ip=%s, port=%d failed!", ip.c_str(), port);
						continue;
					}
				}
				rule.storer = redisStorer;
				InsertStorer(STORE_REDIS, storageId, ip, port, rule);
				KXLOGDEBUG("insert redis storer storageId=%d", storageId);
			}
		}
		return true;
	}
	return false;
}

Storage* StorageManager::GetStorage(int storageId)
{
    MapStorage::iterator iter = m_mapStorage.find(storageId);
    if(iter != m_mapStorage.end())
    {
		return iter->second;
    }
	return NULL;
}

IStorer* StorageManager::GetStorer(int storageId, int uid)
{
    MapStorage::iterator iter = m_mapStorage.find(storageId);
	if (iter == m_mapStorage.end() || NULL == iter->second)
    {
        return NULL;
    }
	return iter->second->GetStorer(uid);
}

IStorer *StorageManager::GetStorer(std::string ip, int port)
{
	char szPort[32] = {};
	snprintf(szPort,sizeof(szPort),"%d", port);
	ip = ip + szPort;

	if (m_mapStorers.find(ip) != m_mapStorers.end())
	{
		return m_mapStorers[ip];
	}
	return NULL;
}

bool StorageManager::InsertStorer(STORER_TYPE type, int storageId, std::string ip, int port, DBRule rule)
{
    Storage *storage = NULL;
	if (NULL == rule.storer)
    { 
        return false;
    }

    MapStorage::iterator iter = m_mapStorage.find(storageId);
    if(iter == m_mapStorage.end())
    {
        switch(type)
        {
        case STORE_MYSQL:
        case STORE_REDIS:
        case STORE_TMEM:
            storage = new Storage;
			storage->InitStorage(type);
            break;
        default:
            break;
        }
        m_mapStorage.insert(pair<int, Storage*>(storageId, storage));
    }
    else
    {
        storage = iter->second;
    }

	char szPort[32] = {};
	snprintf(szPort,sizeof(szPort),"%d", port);
	ip = ip + szPort;
	if (m_mapStorers.find(ip) == m_mapStorers.end())
	{
		m_mapStorers[ip] = rule.storer;
	}

	return storage->InsertStorer(rule);
}
