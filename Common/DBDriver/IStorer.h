#ifndef __ISTORER_H__
#define __ISTORER_H__

#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include <malloc.h>
#endif

class IStorer
{
public:
	IStorer();
	virtual ~IStorer();
};

#endif //__ISTORER_H__
