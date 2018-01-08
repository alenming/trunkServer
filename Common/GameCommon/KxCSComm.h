/*
* KxCS框架公共头文件
* KxCS框架是基于KxServer和Cocos2d-x的网络同步框架，CS表示Client & Server
* 支持按照KxCS思想编写的Cocos2d-x游戏实现网络同步，验证功能
* 将客户端逻辑代码拷贝至服务器，编写少量初始化代码即可实现验证
*
* 2014-12-16 by 宝爷
*/
#ifndef __KXCSCOMM_H__
#define __KXCSCOMM_H__

#include <map>
#include <list>
#include <deque>
#include <vector>
#include <string>

// 保证精度的float转int
inline int floatToInt(float num)
{
    if (num >= 0.0f)
    {
        return static_cast<int>(num + 0.000001f);
    }
    else
    {
        return static_cast<int>(num - 0.000001f);
    }
}

#define RunningInServer
#ifndef RunningInServer

#include "cocos2d.h"

using cocos2d::Ref;
using cocos2d::Component;
using cocos2d::Node;
using cocos2d::Scene;
using cocos2d::Layer;
using cocos2d::Scheduler;
using cocos2d::Vec2;
using cocos2d::Director;
using cocos2d::Color3B;
using cocos2d::Size;
using cocos2d::Rect;
using cocos2d::Touch;
using cocos2d::Event;
using cocos2d::Point;

#define LOG CCLOG

#else

#include "SimpleCocos.h"
#include "KxLog.h"

// 服务器自定义的Ref类
#define Scene Node
#define Layer Node

#ifndef LOG
#define LOG KXLOGINFO
#endif 

#endif // !RunningInServer

#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#endif


//回调定义
typedef void (Ref::*CallBackV)(void*);
#define CALLBACK_FUNCV(_FUNC) static_cast<CallBackV>(&_FUNC)

// 主显示组件添加的子节点的TAG
#define MAIN_DISPLAY    10086   

//辅助的宏
#define SAFE_RETAIN(ref) do { if(NULL != ref) { ref->retain(); } } while (0);
#define SAFE_RELEASE(ref) do { if(NULL != ref) { ref->release(); ref = NULL; } } while (0);
#define SAFE_DELETE(ref) do { if(NULL != ref) { delete ref; ref = NULL; } } while (0);

//用于调试内存泄露的宏
//#define SAFE_RETAIN(ref) do { if(NULL != ref) { LOG("retain %d On File %s Line %d",ref->_ID, __FILE__, __LINE__); ref->retain(); } } while (0);
//#define SAFE_RELEASE(ref) do { if(NULL != ref) { LOG("release %d On File %s Line %d",ref->_ID, __FILE__, __LINE__); ref->release(); ref = NULL; } } while (0);

#define CHECK_RETURN(success) if(!(success)) { LOG("Execute False On File %s Line %d : %s", __FILE__, __LINE__, #success); return false; };
#define CHECK_BREAK(success) if(!(success)) break;
#define CHECK_CONTINUE(success) if(!(success)) continue;
#define CHECK_RETURN_VOID(success) if(!(success)) return;
#define CHECK_RETURN_NULL(success) if(!(success)) { LOG("Execute False On File %s Line %d : %s", __FILE__, __LINE__, #success); return NULL; };
#define CHECK_RETURN_INT(success) if(!(success)) { LOG("Execute False On File %s Line %d : %s", __FILE__, __LINE__, #success); return -1; };
#define CHECK_RETURN_DEBUG_VOID(success) if(!(success)) { LOG("Execute False On File %s Line %d : %s", __FILE__, __LINE__, #success); return; };

typedef std::vector<int>				VecInt;
typedef std::vector<float>				VecFloat;
typedef std::vector<std::vector<int> >   VecVecInt;
typedef std::vector<std::vector<float> > VecVecFloat;
typedef std::map<std::string, float>	MapStrFloat;

#endif // !__KXCSCOMM_H__
