#ifndef __REF_H__
#define __REF_H__

class Ref
{
public:
    Ref(void);
    virtual ~Ref(void);

public:

    virtual void retain();

    virtual void release();

    virtual unsigned int getReferenceCount() const;

protected:
    unsigned int m_unReferenceCount;
};

#endif // __REF_H__
