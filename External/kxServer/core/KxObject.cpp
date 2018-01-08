#include "KxObject.h"

KxObject::KxObject()
:m_ReferenceCount(1)
{
}

KxObject::~KxObject()
{
}

void KxObject::retain()
{
    if (m_ReferenceCount > 0)
    {
        ++m_ReferenceCount;
    }
}

void KxObject::release()
{
    if (m_ReferenceCount > 0)
    {
        --m_ReferenceCount;
        if (m_ReferenceCount == 0)
        {
            delete this;
        }
    }
}

unsigned int KxObject::getReferenceCount() const
{
    return m_ReferenceCount;
}
