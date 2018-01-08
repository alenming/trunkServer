
#include "KXServer.h"
#include "CheckServer.h"
#include "MD5.h"
#include "json/json.h"

//#include "HttpHelper.h"
//
//class CMyObj : public CHttpObject
//{
//    virtual void processCallback(int code)
//    {
//        printf("%s", m_Buffer.c_str());
//    }
//};

int main(int argc, char **argv)
{

	//const char *text = "hello world";
	//CMD5 iMD5;
	//iMD5.GenerateMD5((unsigned char*)text, strlen(text));
	//string Temp = iMD5.ToString();

	//const char* str = "{\"uploadid\": \"UP000000\",\"code\": {\"id\":100,\"value\":\"300\"},\"msg\": \"\",\"files\": \"\"}";

	//Json::Reader reader;
	//Json::Value root;
	//if (reader.parse(str, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
	//{
	//	std::string upload_id = root["uploadid"].asString();  // 访问节点，upload_id = "UP000000"  
	//	int code = root["code"].asInt();    // 访问节点，code = 100 
	//}

    //    CMyObj* myobj = new CMyObj();
    //    myobj->setUrl("http://120.76.166.234/api.php");
    //    myobj->setPostForm("a", "11");
    //    myobj->setPostForm("b", "21");
    //    myobj->setPostForm("c", "31");
    //    myobj->setGetParam("d", "123123");
    //    myobj->setGetParam("e", "123123");
    //    myobj->setGetParam("f", "123123");
    //    CHttpHelper::getInstance()->init();
    //    CHttpHelper::getInstance()->addHttpRequest(myobj);

	//if (reader.parse(str, root))
	//{
	//	std::string code; 

	//	int file_size = root["code"].size();  // 得到"files"的数组个数  
	//	int nValue = root["code"]["id"].asInt();
	//	std::string szValue = root["code"]["value"].asString();
	//	
	//}

	CCheckServer* server = CCheckServer::getInstance();
	if (server->initServerInfo(argc, argv))
	{
		server->startServer();
	}
	CCheckServer::destroy();
    return 0;
}
