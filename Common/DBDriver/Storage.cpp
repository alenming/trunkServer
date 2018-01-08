#include "Storage.h"
#include "CacheStorer.h"
#include "DbStorer.h"
#include "RedisStorer.h"

using namespace std;

Storage::Storage()
: m_type(STORE_NONE)
{
}

Storage::~Storage()
{
	for (vector<DBRule>::iterator iter = m_DBRulelist.begin();
		iter != m_DBRulelist.end(); ++iter)
    {
        IStorer *storer = iter->storer;
		delete storer;
    }
    m_DBRulelist.clear();
}

bool Storage::InitStorage(STORER_TYPE type)
{
	m_type = type;
	return m_type != STORE_NONE;
}

IStorer* Storage::GetStorer(int uid)
{
	// [0-359]
	int modvalue = uid % 360;
	for (vector<DBRule>::iterator iter = m_DBRulelist.begin();
		iter != m_DBRulelist.end(); ++iter)
	{
		if (modvalue >= iter->range_minvalue
			&& modvalue <= iter->range_maxvalue)
		{
			return iter->storer;
		}
	}
	return NULL;
}

bool Storage::InsertStorer(DBRule dbRule)
{
	m_DBRulelist.push_back(dbRule);
    return true;
}
