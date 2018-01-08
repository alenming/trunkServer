#ifndef __HTTP_HELPER_H__
#define __HTTP_HELPER_H__

#include <map>
#include <string>
#include "curl/curl.h"

class CHttpObject
{
public:
    CHttpObject()
        : m_FormPost(NULL)
    {
    }
    virtual ~CHttpObject(){}
    inline void setUrl(const std::string& url)
    {
        m_Url = url;
    }
    inline std::string getUrl() { return m_Url; }
    // 设置以get方式提交的参数
    inline void setGetParam(const std::string& k, const std::string& v)
    {
        m_Gets[k] = v;
    }
    inline std::map<std::string, std::string>& getGetParams() { return m_Gets; }
    // 设置要提交的表单
    inline void setPostForm(const std::string& k, const std::string& v)
    {
        m_PostForm[k] = v;
    }
    inline std::map<std::string, std::string>& getPostForm() { return m_PostForm; }
    void setFormPost(struct curl_httppost* formpost) { m_FormPost = formpost; }
    struct curl_httppost* getFormPost() { return m_FormPost; }
    inline void appendData(const char* data) { m_Buffer += data; }
    // 请求完成后的回调，code 为 0表示正确，其他表示异常
    virtual void processCallback(int code) = 0;

protected:
    struct curl_httppost* m_FormPost;
    std::string m_Buffer;                           // 接收到的数据
    std::string m_Url;                              // 要请求的URL
    std::map<std::string, std::string> m_PostForm;  // 要提交的表单
    std::map<std::string, std::string> m_Gets;      // 要提交的get参数
};

class CHttpHelper
{
private:
    CHttpHelper();
    virtual ~CHttpHelper();

public:
    static CHttpHelper* getInstance();
    static void destroy();

    bool init();
    // 添加http请求
    bool addHttpRequest(CHttpObject* obj);
    void update();

private:
    CURLM* m_MURLHandle;
    int m_RunningInstance;
    std::map<CURL*, CHttpObject*> m_RequestMap;
    static CHttpHelper* m_Instance;
};

#endif
