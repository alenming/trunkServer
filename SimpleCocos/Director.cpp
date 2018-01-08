#include "Director.h"
#include "Node.h"

Director::Director(void)
: m_pScene(NULL)
, m_pRunningScene(NULL)
, m_nMaxFrames(0)
, m_nCurFrame(-1)
, m_fDelta(0.0f)
, m_bPaused(false)
{
}

Director::~Director(void)
{
}

bool Director::init()
{
    return true;
}

void Director::initFrames(int fps, int maxFrames, int curFrame)
{
    m_nFPS = fps;
    m_fDelta = 1.0f / m_nFPS;

    m_nMaxFrames = maxFrames;
    m_nCurFrame = curFrame;
}

void Director::runWithScene(Node *pScene)
{
    if (NULL != pScene)
    {
        m_pScene = pScene;
    }
}

void Director::end()
{
    if (NULL != m_pRunningScene)
    {
        m_pRunningScene->onExitTransitionDidStart();
        m_pRunningScene->onExit();
        m_pRunningScene->cleanup();
        m_pRunningScene->release();
    }

    m_pRunningScene = NULL;
}

void Director::mainLoop()
{
    if (m_pScene != NULL)
    {
        m_pRunningScene = m_pScene;
        m_pScene = NULL;

        m_pRunningScene->onEnter();
        m_pRunningScene->onEnterTransitionDidFinish();
    }

    if (NULL != m_pRunningScene)
    {
        m_pRunningScene->update(m_fDelta);
    }
}

void Director::loopDelta(float delta)
{
    m_fDelta = delta;
    mainLoop();
    m_fDelta = 0;
}

// void Director::loopToFrame(int toFrame)
// {
//     while (toFrame > m_nCurFrame)
//     {
//         mainLoop();
//         ++m_nCurFrame;
//     }
// }
// 
// void Director::loopFrames(int frames)
// {
//     int loopFrames = 0;
//     while(++loopFrames <= frames)
//     {
//         mainLoop();
//         ++m_nCurFrame;
//     }
// }

Node* Director::getRunningScene()
{
    return m_pRunningScene;
}
