#include "Node.h"

using namespace std;

Node::Node(void)
    :m_pParent(NULL)
    ,m_bRunning(false)
    ,m_nTag(-1)
{
}

Node::~Node(void)
{
}

bool Node::init()
{
    return true;
}

void Node::onEnter()
{
    m_bRunning = true;

    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        Node *pChild = *iter;
        if (pChild->isRunning())
        {
            pChild->onEnter();
        }
    }
}

void Node::onEnterTransitionDidFinish()
{
    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        Node *pChild = *iter;
        pChild->onEnterTransitionDidFinish();
    }
}

void Node::onExit()
{
    m_bRunning = false;

    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        Node *pChild = *iter;
        pChild->onExit();
    }
}

void Node::onExitTransitionDidStart()
{
    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        Node *pChild = *iter;
        pChild->onExitTransitionDidStart();
    }
}

void Node::update(float dt)
{

}

void Node::addChild(Node *pChild)
{
    addChild(pChild, 0, -1);
}

void Node::addChild(Node *pChild, int zOrder)
{
    addChild(pChild, zOrder, -1);
}

void Node::addChild(Node *pChild, int zOrder, int tag)
{
    if (NULL == pChild)
    {
        return;
    }

    pChild->m_nTag = tag;
    pChild->setParent(this);
    pChild->retain();
    m_pChildren.push_back(pChild);
    
    if (m_bRunning)
    {
        pChild->onEnter();
        pChild->onEnterTransitionDidFinish();
    }
}

void Node::removeAllChildrenWithCleanup(bool bCleanup)
{
    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        Node *pChild = *iter;
        if (NULL != pChild)
        {
            if (m_bRunning)
            {
                pChild->onExitTransitionDidStart();
                pChild->onExit();
            }

            if (bCleanup)
            {
                pChild->cleanup();
            }

            pChild->setParent(NULL);
            pChild->release();
        }
    }

    m_pChildren.clear();
}

void Node::removeChild(Node* child, bool bCleanup)
{
    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        if ((*iter) == child)
        {
            if (m_bRunning)
            {
                child->onExitTransitionDidStart();
                child->onExit();
            }

            if (bCleanup)
            {
                child->cleanup();
            }

            child->setParent(NULL);
            child->release();
            m_pChildren.erase(iter);
            return;
        }
    }
}

void Node::removeFromParentAndCleanup(bool cleanup)
{
    if (m_pParent != NULL)
    {
        m_pParent->removeChild(this, cleanup);
    } 
}

void Node::cleanup()
{
    for (vector<Node*>::iterator iter = m_pChildren.begin(); 
        iter != m_pChildren.end(); ++iter)
    {
        Node *pChild = *iter;
        if (NULL != pChild)
        {
            pChild->cleanup();
        }
    }
}

bool Node::isRunning()
{
    return m_bRunning;
}

void Node::setParent(Node *pParent)
{
    m_pParent = pParent;
}

void Node::setPosition(float x, float y)
{
    if (m_Position.x == x && m_Position.y == y)
        return;

    m_Position.x = x;
    m_Position.y = y;
}

void Node::setPosition(const Vec2 &position)
{
    setPosition(position.x, position.y);
}

const Vec2& Node::getPosition() const
{
    return m_Position;
}

void Node::setPositionX(float x)
{
    m_Position.x = x;
}

float Node::getPositionX(void) const
{
    return m_Position.x;
}

void Node::setPositionY(float y)
{
    m_Position.y = y;
}

float Node::getPositionY(void) const
{
    return m_Position.y;
}
