/***************************************
* @file     hardwareinfo.h
* @brief    硬件相关信息
* @details  CPU个数(opencv2.4.6 modules\core\src\parallel.cpp)--getNumberOfCPUs
* @author   phata, wqvbjhc@gmail.com
* @date     2013-9-2
****************************************/
#if defined WIN32 || defined _WIN32 || defined WINCE
#ifndef _WIN32_WINNT           // This is needed for the declaration of TryEnterCriticalSection in winbase.h with Visual Studio 2005 (and older?)
#define _WIN32_WINNT 0x0500  // http://msdn.microsoft.com/en-us/library/ms686857(VS.85).aspx
#endif
#include <windows.h>
#undef small
#undef min
#undef max
#undef abs
#include <tchar.h>
#if defined _MSC_VER
#if _MSC_VER >= 1400
#include <intrin.h>
#endif
#endif
#else
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#endif

#include <stdarg.h>

#if defined __linux__ || defined __APPLE__
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#endif


#ifdef ANDROID
static inline int getNumberOfCPUsImpl()
{
    FILE* cpuPossible = fopen("/sys/devices/system/cpu/possible", "r");
    if(!cpuPossible) {
        return 1;
    }

    char buf[2000]; //big enough for 1000 CPUs in worst possible configuration
    char* pbuf = fgets(buf, sizeof(buf), cpuPossible);
    fclose(cpuPossible);
    if(!pbuf) {
        return 1;
    }

    //parse string of form "0-1,3,5-7,10,13-15"
    int cpusAvailable = 0;

    while(*pbuf) {
        const char* pos = pbuf;
        bool range = false;
        while(*pbuf && *pbuf != ',') {
            if(*pbuf == '-') {
                range = true;
            }
            ++pbuf;
        }
        if(*pbuf) {
            *pbuf++ = 0;
        }
        if(!range) {
            ++cpusAvailable;
        } else {
            int rstart = 0, rend = 0;
            sscanf(pos, "%d-%d", &rstart, &rend);
            cpusAvailable += rend - rstart + 1;
        }

    }
    return cpusAvailable ? cpusAvailable : 1;
}
#endif

inline int getNumberOfCPUs(void)
{
#if defined WIN32 || defined _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );

    return (int)sysinfo.dwNumberOfProcessors;
#elif defined ANDROID
    static int ncpus = getNumberOfCPUsImpl();
    return ncpus;
#elif defined __linux__
    return (int)sysconf( _SC_NPROCESSORS_ONLN );
#elif defined __APPLE__
    int numCPU=0;
    int mib[4];
    size_t len = sizeof(numCPU);

    /* set the mib for hw.ncpu */
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

    /* get the number of CPUs from the system */
    sysctl(mib, 2, &numCPU, &len, NULL, 0);

    if( numCPU < 1 ) {
        mib[1] = HW_NCPU;
        sysctl( mib, 2, &numCPU, &len, NULL, 0 );

        if( numCPU < 1 ) {
            numCPU = 1;
        }
    }

    return (int)numCPU;
#else
    return 1;
#endif
}
