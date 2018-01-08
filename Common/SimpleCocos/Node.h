#ifndef __NODE_H__
#define __NODE_H__

#include "Ref.h"
#include "Vec2.h"

#include <vector>

class Node : public Ref
{
public:
    Node(void);
    virtual ~Node(void);

public:
    // execute about
    virtual bool init();

    virtual void onEnter();

    virtual void onEnterTransitionDidFinish();
    
    virtual void onExit();

    virtual void onExitTransitionDidStart();

    virtual void update(float dt);

    //children about
    virtual void addChild(Node *child);

    virtual void addChild(Node *child, int zOrder);

    virtual void addChild(Node *child, int zOrder, int tag);

    virtual void removeAllChildrenWithCleanup(bool bCleanup);

    virtual void removeChild(Node* child, bool bCleanup);

    virtual void removeFromParentAndCleanup(bool cleanup);

    // self about
    virtual bool isRunning();

    virtual void cleanup();

    virtual void setParent(Node *pParent);
    Node* getParent() { return m_pParent; }

    virtual void setPosition(float x, float y);
    virtual void setPosition(const Vec2 &position);
    virtual const Vec2& getPosition() const;

    virtual void setPositionX(float x);
    virtual float getPositionX(void) const;
    virtual void setPositionY(float y);
    virtual float getPositionY(void) const;

protected:
    Node*                  m_pParent;
    Vec2                   m_Position;
    bool                   m_bRunning;
    int                    m_nTag;
    std::vector<Node *>    m_pChildren;
};

#endif 
