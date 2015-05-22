/*****************************************
* @file     process_mem_cpu.h
* @brief
* @details  获取指定进程cpu与内存占有率的函数,win版本需要链接psapi.lib
            cb链接选项:[[ if (PLATFORM == PLATFORM_MSW) { print( _T("-lpsapi"));} ]]
* @author   phata,wqvbjhc@gmail.com
* @date     2013-4-27
* @mod      2012:第一次实现，应用于xte_iva的测试时，用于检测cpu与mem性能
            2013-4-27:整合，把它们整合成一个.h文件，并存放于common目录中
******************************************/
#ifndef COMMON_PROCESS_MEM_CPU_H
#define COMMON_PROCESS_MEM_CPU_H
//函数声明
#include <stdint.h>
/// 获取当前进程的cpu使用率(0-100,为百分比)，返回: 0失败，非零成功
int get_curprocess_cpu_usage(double *cpuusage);
/// 获取当前进程内存和虚拟内存使用量(单位为KB)，返回: 0失败，非零成功
int get_curprocess_memory_usage(uint64_t* mem, uint64_t* vmem);

//函数定义
#if defined (WIN32) || defined(_WIN32)
/* @brief 进程统计信息函数的实现
 * @author 张亚霏
 * @date 2009/05/03
 * @version 0.1
 *
 * 部分代码来自MSDN的例子
 * 部分代码来自google chromium项目
 *
 * 需要连接到psapi.lib
 * 需要定义
 #define _WIN32_WINNT 0x0500
 */
#include <windows.h>
#include <winbase.h>
#include <psapi.h>
#include <assert.h>
#ifdef _MSC_VER
#pragma comment(lib,"psapi.lib")
#endif
/// 时间转换
static uint64_t file_time_2_utc(const FILETIME* ftime)
{
    LARGE_INTEGER li;

    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}


/// 获得CPU的核数
static int get_processor_number()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}

/// 获取当前进程的cpu使用率(0-100,为百分比)，返回: 0失败，非零成功
int get_curprocess_cpu_usage(double *cpuusage)
{
    //cpu数量
    static int processor_count_ = -1;
    //上一次的时间
    static int64_t last_time_ = 0;
    static int64_t last_system_time_ = 0;
    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    int64_t system_time;
    int64_t time;
    int64_t system_time_delta;
    int64_t time_delta;
    if(!cpuusage) {
        return 0;
    }


    if(processor_count_ == -1) {
        processor_count_ = get_processor_number();
    }

    GetSystemTimeAsFileTime(&now);

    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
                         &kernel_time, &user_time)) {
        // We don't assert here because in some cases (such as in the Task Manager)
        // we may call this function on a process that has just exited but we have
        // not yet received the notification.
        return 0;
    }
    system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) /processor_count_;
    time = file_time_2_utc(&now);

    if ((last_system_time_ == 0) || (last_time_ == 0)) {
        // First call, just set the last values.
        last_system_time_ = system_time;
        last_time_ = time;
        return 0;
    }

    system_time_delta = system_time - last_system_time_;
    time_delta = time - last_time_;

    if(time_delta == 0) return 0;//时间间隔太短

    if (time_delta == 0) {
        return 0;
    }

    // We add time_delta / 2 so the result is rounded.
    *cpuusage = (double)((system_time_delta * 100. + time_delta / 2) / (time_delta));
    last_system_time_ = system_time;
    last_time_ = time;
    return 1;
}

/// 获取当前进程内存和虚拟内存使用量(单位为KB)，返回: 0失败，非零成功
int get_curprocess_memory_usage(uint64_t* mem, uint64_t* vmem)
{
    PROCESS_MEMORY_COUNTERS pmc;
    if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        if(mem) {
            *mem = pmc.WorkingSetSize/1024;
        }
        if(vmem) {
            *vmem = pmc.PagefileUsage/1024;
        }
        return 1;
    }
    return 0;
}

#elif defined __linux__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/vtimes.h>
#define LINE_LENGTH 256

/// 获取当前进程的cpu使用率(0-100,为百分比)，返回: 0失败，非零成功
int get_curprocess_cpu_usage(double *cpuusage)
{
    static clock_t lastCPU, lastSysCPU, lastUserCPU;
    static int numProcessors;
    static int isInit=0;
    if(0==isInit) {
        struct tms timeSample;
        char line[LINE_LENGTH];

        lastCPU = times(&timeSample);
        if(-1==lastCPU) return 0;
        lastSysCPU = timeSample.tms_stime;
        lastUserCPU = timeSample.tms_utime;

        FILE* file = fopen("/proc/cpuinfo", "r");
        if(!file) return 0;
        numProcessors = 0;
        while(fgets(line, LINE_LENGTH, file) != NULL) {
            if (strncmp(line, "processor", 9) == 0) {
                numProcessors++;
            }
        }
        fclose(file);
        isInit=1;
        *cpuusage=-1.;
        return 0;
    }

    if(!cpuusage) {
        return 0;
    }

    struct tms timeSample;
    clock_t now;

    now = times(&timeSample);
    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
            timeSample.tms_utime < lastUserCPU) {
        //Overflow detection. Just skip this value.
        *cpuusage=-1.;
        return 0;
    } else {
        *cpuusage = (timeSample.tms_stime - lastSysCPU) +
                    (timeSample.tms_utime - lastUserCPU);
        *cpuusage /= (now - lastCPU);
        *cpuusage /= numProcessors;
        *cpuusage *= 100;
    }
    lastCPU = now;
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;
    return 1;
}

/*******************************
/proc/ /status
包含了所有CPU活跃的信息，该文件中的所有值都是从系统启动开始累计到当前时刻。

[root@localhost ~]# cat /proc/self/status
Name: cat
State: R (running)
SleepAVG: 88%
Tgid: 5783
Pid: 5783
PPid: 5742
TracerPid: 0
Uid: 0 0 0 0
Gid: 0 0 0 0
FDSize: 256
Groups: 0 1 2 3 4 6 10
VmSize: 6588 kB
VmLck: 0 kB
VmRSS: 400 kB
VmData: 144 kB
VmStk: 2040 kB
VmExe: 14 kB
VmLib: 1250 kB
StaBrk: 0804e000 kB
Brk: 088df000 kB
StaStk: bfe03270 kB
ExecLim: 0804c000
Threads: 1
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: 0000000000000000
SigCgt: 0000000000000000
CapInh: 0000000000000000
CapPrm: 00000000fffffeff
CapEff: 00000000fffffeff


输出解释
参数 解释
Name 应用程序或命令的名字
State 任务的状态，运行/睡眠/僵死/
SleepAVG 任务的平均等待时间(以nanosecond为单位)，交互式任务因为休眠次数多、时间长，它们的 sleep_avg 也会相应地更大一些，所以计算出来的优先级也会相应高一些。
Tgid 线程组号
Pid 任务ID
Ppid 父进程ID
TracerPid 接收跟踪该进程信息的进程的ID号
Uid Uid euid suid fsuid
Gid Gid egid sgid fsgid
FDSize 文件描述符的最大个数，file->fds
Groups
VmSize(KB) 任务虚拟地址空间的大小 (total_vm-reserved_vm)，其中total_vm为进程的地址空间的大小，reserved_vm：进程在预留或特殊的内存间的物理页
VmLck(KB) 任务已经锁住的物理内存的大小。锁住的物理内存不能交换到硬盘 (locked_vm)
VmRSS(KB) 应用程序正在使用的物理内存的大小，就是用ps命令的参数rss的值 (rss)
VmData(KB) 程序数据段的大小（所占虚拟内存的大小），存放初始化了的数据； (total_vm-shared_vm-stack_vm)
VmStk(KB) 任务在用户态的栈的大小 (stack_vm)
VmExe(KB) 程序所拥有的可执行虚拟内存的大小，代码段，不包括任务使用的库 (end_code-start_code)
VmLib(KB) 被映像到任务的虚拟内存空间的库的大小 (exec_lib)
VmPTE 该进程的所有页表的大小，单位：kb
Threads 共享使用该信号描述符的任务的个数，在POSIX多线程序应用程序中，线程组中的所有线程使用同一个信号描述符。
SigQ 待处理信号的个数
SigPnd 屏蔽位，存储了该线程的待处理信号
ShdPnd 屏蔽位，存储了该线程组的待处理信号
SigBlk 存放被阻塞的信号
SigIgn 存放被忽略的信号
SigCgt 存放被俘获到的信号
CapInh Inheritable，能被当前进程执行的程序的继承的能力
CapPrm Permitted，进程能够使用的能力，可以包含CapEff中没有的能力，这些能力是被进程自己临时放弃的，CapEff是CapPrm的一个子集，进程放弃没有必要的能力有利于提高安全性
CapEff Effective，进程的有效能力
*******************************************/
/// 获取当前进程内存和虚拟内存使用量(单位为KB)，返回: 0失败，非零成功
int get_curprocess_memory_usage(uint64_t* mem, uint64_t* vmem)
{
    if(!mem && !vmem) {
        return 1;
    }
    char cmdline[LINE_LENGTH];
    sprintf(cmdline, "/proc/%d/status", getpid());
    FILE* file = fopen(cmdline, "r");
    while (fgets(cmdline, LINE_LENGTH, file) != NULL) {
        if (strncmp(cmdline, "VmSize:", 7) == 0) {
            if(1!=sscanf(cmdline,"VmSize: %llu %*s",vmem)) {
                fclose(file);
                return 0;
            }
        }
        if (strncmp(cmdline, "VmRSS:", 6) == 0) {
            if(1!=sscanf(cmdline,"VmRSS: %llu %*s",mem)) {
                fclose(file);
                return 0;
            }
        }
    }
    fclose(file);
    return 1;
}

#else
#error "no supported os"
#endif

#endif//COMMON_PROCESS_MEM_CPU_H
