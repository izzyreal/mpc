#pragma once

#define _wrpx11 1 

#if defined(_WIN64) || defined(_WIN32)
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #ifndef WRP_NO_KEY_WRITER
    #include <rtccore.h>
  #endif
  #include <windef.h>
  #include <winbase.h>
  #include <winuser.h>
#elif defined(__APPLE__)
  #include <TargetConditionals.h>
  #if !TARGET_OS_IOS
    #include "Carbon/Carbon.h"
  #endif
#elif defined(__linux)
  #ifndef _wrpx11
    #include <linux/uinput.h>
  #else
    #define XK_MISCELLANY 1
    #define XK_LATIN1
    #include </usr/include/X11/keysymdef.h>
  #endif
#endif
