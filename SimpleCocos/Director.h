#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "Ref.h"

class Node;
class Director : public Ref
{
public:
    Director(void);
    ~Director(void);

public:
    // 初始化必要组件
    bool init();
    // 初始化帧数据
    void initFrames(int fps, int maxFrames = 0xffffff, int curFrame = -1);
    // 设置启动游戏
    void runWithScene(Node *pScene);
    // 结束游戏, 回收节点, 回收内存
    void end();
    // 游戏轮询单帧, 引导所有node进入onEnter初始化节点
    void mainLoop();
    // 轮询一个时间间隔, 至于多少帧, 由director的子节点自己确定.
    void loopDelta(float delta);
//     // 轮询到第几帧 (暂时无用)
//     void loopToFrame(int toFrame);
//     // 轮询n帧 (暂时无用)
//     void loopFrames(int frames);
    // 获得游戏的轮询节点
    Node* getRunningScene();

private:
    Node*       m_pScene;               // 将要运行的场景
    Node*       m_pRunningScene;        // 运行中的场景
    int         m_nMaxFrames;           // 总帧数
    int         m_nCurFrame;            // 当前帧数
    int         m_nFPS;                 // 每秒多少帧
    float       m_fDelta;               // 每帧时间
    bool        m_bPaused;              // 是否暂停游戏
};

#endif //__DIRECTOR_H__
