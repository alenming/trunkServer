#include "Ref.h"

Ref::Ref(void)
    :m_unReferenceCount(1)
{
}

Ref::~Ref(void)
{
}

void Ref::retain()
{
    if (m_unReferenceCount > 0)
    {
        ++m_unReferenceCount;
    }
}

void Ref::release()
{
    if (m_unReferenceCount > 0)
    {
        --m_unReferenceCount;
        if (m_unReferenceCount == 0)
        {
            delete this;
        }
    }
}

unsigned int Ref::getReferenceCount() const
{
    return m_unReferenceCount;
}
