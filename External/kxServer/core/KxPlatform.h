#ifndef __KXPLATFORM_H__
#define __KXPLATFORM_H__

#define KX_PLATFORM_UNKNOWN 0  
#define KX_PLATFORM_WIN32 1
#define KX_PLATFORM_LINUX 2
#define KX_PLATFORM_MAC 3

#define KX_TARGET_PLATFORM KX_PLATFORM_UNKNOWN

// mac
#if defined(__APPLE__)
#undef  KX_TARGET_PLATFORM
#define KX_TARGET_PLATFORM KX_PLATFORM_MAC
#endif

// win32
#if defined(_WIN32)
    #undef  KX_TARGET_PLATFORM
    #define KX_TARGET_PLATFORM         KX_PLATFORM_WIN32
#endif

// linux
#if defined(linux)
    #undef  KX_TARGET_PLATFORM
    #define KX_TARGET_PLATFORM         KX_PLATFORM_LINUX
#endif

#endif
