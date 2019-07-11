#define wxUSE_SECRETSTORE 0
#define wxUSE_STD_CONTAINERS_COMPATIBLY 0
#define wxUSE_UNSAFE_WXSTRING_CONV 0
#define wxUSE_ADDREMOVECTRL 0
#define wxUSE_ACTIVITYINDICATOR 0
#define wxUSE_PRIVATE_FONTS 0
#define wxUSE_DBGHELP 0
#define wxUSE_TASKBARBUTTON 1
#define wxUSE_WINSOCK2 1
#define wxUSE_ZLIB_H_IN_PATH 1

#ifdef HX_WINDOWS
#include "setup_hx_windows.h"
#elif defined(HXCPP_RPI)
#include "setup_hx_rpi.h"
#elif defined(HX_MACOS)
#include "setup_hx_mac.h"
#elif defined(HX_LINUX)
   #ifdef HXCPP_M64
   #include "setup_hx_linux64.h"
   #else
   #include "setup_hx_linux.h"
   #endif
#else
#error "Unknown platform"
#endif
