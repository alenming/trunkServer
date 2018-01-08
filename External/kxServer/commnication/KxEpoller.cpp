#include "KxEpoller.h"
#include "KxLog.h"

#define KXEPOLL_ERROR(obj, iter, pollMap) {\
	KXLOGDEBUG("error: KxEpoller::poll line %d error objid %d", __LINE__, obj->getCommId()); \
	applyChange(fd, EPOLL_CTL_DEL, obj->getPollType()); \
	obj->setPoller(NULL); \
	pollMap.erase(iter); \
    obj->release(); \
    obj->onError(); \
	obj->release(); \
	continue; \
}

using namespace std;

namespace KxServer {

#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)

	KxEpoller::KxEpoller(int maxEventQueue)
		:m_MaxEventQueue(maxEventQueue)
	{
		m_TimeOut = 1;
		m_EpollFd = epoll_create(maxEventQueue);
		if (m_EpollFd < 0)
		{
			KXLOGERROR("error: KxEpoller::poll epoll_create error %d", errno);
		}
	}

	KxEpoller::~KxEpoller()
	{
		for (map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.begin();
			iter != m_PollMap.end(); ++iter)
		{
			(iter->second)->release();
		}
		m_PollMap.clear();
		close(m_EpollFd);
	}

	int KxEpoller::poll()
	{
		int maxnotify = epoll_wait(m_EpollFd, m_Events, m_MaxEventQueue, m_TimeOut);
		if (maxnotify < 0)
		{
			//linux下可以直接使用errno，但还是应该封装未全局函数
			KXLOGERROR("error: KxEpoller::poll epoll_wait error ret %d errno %d", maxnotify, errno);
            maxnotify = 0;
		}

		for (int i = 0; i < maxnotify; ++i)
		{
			KXCOMMID fd = m_Events[i].data.fd;
			map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
			if (iter == m_PollMap.end())
			{
				KXLOGDEBUG("warn: KxEpoller::poll epoll_wait missing IKxComm with fd %d event %d",
					fd, m_Events[i].events);
				continue;
			}
			IKxComm* obj = iter->second;
            m_CurrentPollObject = obj;
			int event = 0;
			if (m_Events[i].events & (EPOLLHUP | EPOLLERR))
			{
				event = EPOLLIN | EPOLLOUT;
			}
			else
			{
				event = m_Events[i].events;
			}

			int oldPollType = obj->getPollType();
			obj->retain();
			if (event & EPOLLIN)
			{
				//KXLOGDEBUG("warn: KxEpoller::poll onRecv IKxComm with fd %d event %d",
				//	fd, m_Events[i].events);
                int ret = 1;
                while (ret > 0)
                {
                    ret = obj->onRecv();
                }

				if(0 > ret)
				{
					KXEPOLL_ERROR(obj, iter, m_PollMap);
				}
			}

			if (event & EPOLLOUT)
			{
				KXLOGDEBUG("warn: KxEpoller::poll onSend IKxComm with fd %d event %d",
					fd, m_Events[i].events);
				if(0 > obj->onSend())
				{
					KXEPOLL_ERROR(obj, iter, m_PollMap);
				}
			}

			// ET模式下重新添加
			if(oldPollType != obj->getPollType())
			{
				KXLOGDEBUG("warn: KxEpoller::poll auto modify %d type %d to %d", fd, oldPollType, obj->getPollType());
				modifyCommObject(obj, obj->getPollType());
			}
            m_CurrentPollObject = NULL;
			obj->release();
		}
		
		return maxnotify;
	}

	int KxEpoller::addCommObject(IKxComm* obj, int events)
	{
		if (NULL == obj)
		{
			KXLOGERROR("error: KxEpoller::addCommObject error NULL");
			return -1;
		}
		KXCOMMID fd = obj->getCommId();
		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
		if (iter != m_PollMap.end())
		{
			if(iter->second != obj)
			{
				// 如果存在关闭了socket但没有removeCommObject的异常，我们可以将旧的Object释放
				KXLOGERROR("error: KxEpoller::addCommObject has a invalid object %x new obj %x of sock fd %d",
					iter->second, obj, fd);
				iter->second->setPoller(NULL);
				iter->second->release();
				m_PollMap.erase(iter);
			}
			else
			{
				// 重复addCommObject，第二次add自动调整为Modify操作
				// 如果applyChange失败，讲道理应该把它移除的
				KXLOGDEBUG("warn: KxEpoller::addCommObject add sock %d duplicate", fd);
				int ret = applyChange(fd, EPOLL_CTL_MOD, events);
				if (ret == -1)
				{
					iter->second->setPoller(NULL);
					iter->second->release();
					m_PollMap.erase(iter);
				}
				return ret;
			}
		}

		int ret = applyChange(fd, EPOLL_CTL_ADD, events);
		if (ret == 0)
		{
			// 只有添加成功才能插入到PollMap中
			m_PollMap[fd] = obj;
			obj->retain();
			obj->setPollType(events);
			obj->setPoller(this);
			KXLOGDEBUG("warn: KxEpoller::addCommObject object %x fd %d events %d success", obj, fd, events);
		}

		return ret;
	}

	int KxEpoller::modifyCommObject(IKxComm* obj, int events)
	{
		if (NULL == obj)
		{
			KXLOGERROR("error: KxEpoller::modifyCommObject error NULL");
			return -1;
		}
		KXCOMMID fd = obj->getCommId();
		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
		if (iter == m_PollMap.end())
		{
			KXLOGERROR("error: KxEpoller::modifyCommObject object %x fd %d faile, isn't in pollMap",
				obj, fd);
			return -1;
		}
		else if (iter->second != obj)
		{
			KXLOGERROR("error: KxEpoller::modifyCommObject object %x fd %d faile, invalid object %x",
				obj, fd, iter->second);
			return -1;
		}

		int ret = applyChange(fd, EPOLL_CTL_MOD, events);
		if (ret == -1)
		{
			iter->second->setPoller(NULL);
			iter->second->release();
			m_PollMap.erase(iter);
		}

		return ret;
	}

	int KxEpoller::removeCommObject(IKxComm* obj)
	{
		if (NULL == obj)
		{
			KXLOGERROR("error: KxEpoller::removeCommObject error NULL");
			return -1;
		}
		KXCOMMID fd = obj->getCommId();
		// 先从epoll中移除
		int ret = applyChange(fd, EPOLL_CTL_DEL, obj->getPollType());
		obj->setPoller(NULL);

		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
		if (iter != m_PollMap.end())
		{
			if (iter->second != obj)
			{
				// 当一个object在被关闭之后，没有从poller中移除，该fd被重用之后，没有添加到poller中，却调用了移除
				// 这种情况下，该object也是不会被触发，应该被移除，这里只添加一行简单日志记录
				KXLOGERROR("error: KxEpoller::removeCommObject object %x fd %d faile, invalid object %x",
					obj, fd, iter->second);
			}
			iter->second->release();
			m_PollMap.erase(iter);
		}
		else
		{
			KXLOGERROR("error: KxEpoller::removeCommObject object %x fd %d faile, isn't in pollMap",
				obj, fd);
			return -1;
		}
		return ret;
	}

	int KxEpoller::applyChange(KXCOMMID fd, int opt, int events)
	{
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events  = events | EPOLLET;
		int ret = epoll_ctl(m_EpollFd, opt, fd, &ev);
		if (ret == -1)
		{
			if (opt == EPOLL_CTL_ADD && errno == EEXIST)
			{
				opt = EPOLL_CTL_MOD;
			}
			else if (opt == EPOLL_CTL_MOD && errno == ENOENT)
			{
				opt = EPOLL_CTL_ADD;
			}
			else
			{
				KXLOGERROR("error: KxEpoller::applyChange fd %d opt %d events %d faile, errno %d",
					fd, opt, events, errno);
				return ret;
			}
			ret = epoll_ctl(m_EpollFd, opt, fd, &ev);
			if (ret == -1)
			{
				KXLOGERROR("error: KxEpoller::applyChange try again fd %d opt %d events %d faile, errno %d",
					fd, opt, events, errno);
				return ret;
			}
		}
		KXLOGDEBUG("warn: KxEpoller::applyChange fd %d opt %d evetns %d, success", fd, opt, events);
		return ret;
	}

	IKxComm* KxEpoller::getComm(KXCOMMID cid)
	{
		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(cid);
		if (iter == m_PollMap.end())
		{
			return NULL;
		}

		return iter->second;
	}

#endif

}
