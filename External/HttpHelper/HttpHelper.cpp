#include "HttpHelper.h"
#include "KXServer.h"

CHttpHelper* CHttpHelper::m_Instance = NULL;
using namespace std;
using namespace KxServer;

CHttpHelper::CHttpHelper()
: m_MURLHandle(NULL)
, m_RunningInstance(0)
{
}

CHttpHelper::~CHttpHelper()
{
    curl_multi_cleanup(m_MURLHandle);
}

CHttpHelper* CHttpHelper::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new CHttpHelper();
    }
    return m_Instance;
}

void CHttpHelper::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

bool CHttpHelper::init()
{
    if (m_MURLHandle == NULL)
    {
        m_MURLHandle = curl_multi_init();
    }
    return true;
}

size_t writeFun(void* ptr, size_t size, size_t nmemb, void *userdata)
{
    ((CHttpObject*)(userdata))->appendData((char*)ptr);
    return nmemb;
}

bool CHttpHelper::addHttpRequest(CHttpObject* obj)
{
    if (NULL == obj)
    {
        return false;
    }

    CURL *handle = curl_easy_init();
    // 设置URL
    std::map<std::string, std::string>& gets = obj->getGetParams();
    if (gets.size() > 0)
    {
        std::string url = obj->getUrl();
        char buffer[128] = { 0 };
        char separator = '?';
        for (std::map<std::string, std::string>::iterator iter = gets.begin();
            iter != gets.end(); ++iter)
        {
            snprintf(buffer, sizeof(buffer), "%c%s=%s", separator, iter->first.c_str(), iter->second.c_str());
            url += buffer;
            separator = '&';
        }
        curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    }
    else
    {
        curl_easy_setopt(handle, CURLOPT_URL, obj->getUrl().c_str());
    }
    // 设置表单
    std::map<std::string, std::string>& form = obj->getPostForm();
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    for (std::map<std::string, std::string>::iterator iter = form.begin();
        iter != form.end(); ++iter)
    {
        curl_formadd(&formpost,
            &lastptr,
            CURLFORM_COPYNAME, iter->first.c_str(),
            CURLFORM_COPYCONTENTS, iter->second.c_str(),
            CURLFORM_END);
    }
    if (NULL != formpost)
    {
        curl_easy_setopt(handle, CURLOPT_HTTPPOST, formpost);
        // 保存起来，待清理
        obj->setFormPost(formpost);
    }
    // 设置接收回调
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeFun);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, obj);
    // 添加到m_MURLHandle中
    curl_multi_add_handle(m_MURLHandle, handle);
	curl_multi_perform(m_MURLHandle, &m_RunningInstance);
    m_RequestMap[handle] = obj;
    return true;
}

void CHttpHelper::update()
{
    if (m_RunningInstance > 0)
    {
        struct timeval timeout;
        int rc = -1;
        int msgs_in_queue;
        fd_set fdread;
        fd_set fdwrite;
        fd_set fdexcep;
        int maxfd = -1;
        //long curl_timeo = -1;
        FD_ZERO(&fdread);
        FD_ZERO(&fdwrite);
        FD_ZERO(&fdexcep);
        timeout.tv_sec = 0;
        timeout.tv_usec = 10;

        // 从CURLM中取出fdset，并返回maxfd
        curl_multi_fdset(m_MURLHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
        if (maxfd != -1)
        {
            // select监听是否有读写事件触发，此时的select是阻塞函数，会阻塞timeout所指定的时间
            // 如果不希望select阻塞，可以将timeout的两个成员变量都设置为0，传入NULL表示一直阻塞
            // 当有连接可读或可写时，select会立即返回可读写连接的数量，如果超时则返回0，返回-1表示异常
            rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
        }
        else
        {
            // 网络还未初始化好，下次再来
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_MURLHandle, &m_RunningInstance));
            return;
        }

        // 根据select的结果来处理
		switch (rc)
		{
			// select 发生了错误
		case -1:
			m_RunningInstance = 0;
			KXLOGERROR("select() returns error, this is badness\n");
			break;
			// 有数据可读或可写，调用curl_multi_perform驱动数据读写
			// curl_multi_perform会将剩余正在执行的请求输出到still_running中
			// CURL句柄设置的回调
		case 0:
			return;
		default:
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_MURLHandle, &m_RunningInstance));
			break;
		}

        CURLMsg * msg = NULL;
        do
        {
            msg = curl_multi_info_read(m_MURLHandle, &msgs_in_queue);
            if (msg)
            {
                // 打印请求结果，并清理curl对象
                KXLOGDEBUG("%d http request finish result %d ", msg->easy_handle, msg->data.result);
                std::map<CURL*, CHttpObject*>::iterator iter = m_RequestMap.find(msg->easy_handle);

                if (iter != m_RequestMap.end())
                {
                    CHttpObject* obj = iter->second;
                    // 执行回调
                    obj->processCallback(static_cast<int>(msg->data.result));
                    // 清理表单对象
                    if (obj->getFormPost() != NULL)
                    {
                        curl_formfree(obj->getFormPost());
                    }
                    m_RequestMap.erase(iter);
                    delete obj;
                }
                curl_multi_remove_handle(m_MURLHandle, msg->easy_handle);
                curl_easy_cleanup(msg->easy_handle);
            }
		} while (msg != NULL);
    }
}
