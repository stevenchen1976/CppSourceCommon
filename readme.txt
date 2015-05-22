该目录下存放本人收集的源代码或源创的库文件。源代码都写成头文件，用户使用时只需要包含该头文件就行。代码在win+linux平台下通用。
许可协议：bsd. 若收集的代码本身自带有许可协议，则那部分按原创的协议。

☆★○●◎◇◆□■▓⊿※更新日志
#########################################IDE环境设置########################################################################
codeblocks添加common变量=$(CODEBLOCKS)\sdk\common
头文件搜索路径$(#tiff)
在win系统中，运行setx common %CD%
vs中项目属性->C++属性中附加$(common)目录

目录结构说明：
◆源代码类
charset--本地编码格式，uft8,unicode三种格式相互转换
guid--跨平台生成uuid/guid函数(由网上代码修改而来,很多人转载不注明出处,找不到原作者)
thread--跨平台的thread创建与metux锁函数
sysstat--跨平台的系统性能统计函数。
	sysmem_time.h      与系统内存状态和系统运行时间有关的平台无关的接口
	process_mem_cpu.h  获取指定进程cpu与内存占有率的函数
ThreadQueue.h--线程安全的队列
singleton.h----单例模式
zhao4zhong1_log.h--CSDN上的日志类
ifactory.h/iproduct.h--抽象工厂模式框架
MessageBoxTimeout.h--win32 SDK的定时MessageBox

sys--跨平台的通用组件库，在一步一步完成
▓信息
	作者: phata
	修改日期: 2013-9-2
	备注: 看到淘宝的开源平台（http://code.taobao.org/），发现里面有不少有价值的项目。如ocenabase,tfs,tb-common-utils.
	      其中tb-common-utils是淘宝一个底层库（其他库会依赖它），提供了线程，线程池，日志，内存，文件，目录，异常，网络通讯等底层的实现。
	      可惜它只依赖于linux. 平常本人也有收集各种跨平台的通用组件或使用第三方跨平台库，但这些组件或库多多少少都有与当前的需求不太合意的地方。
	      所以本人决定效仿淘宝自己建一个底层库。因此本目录诞生了！

实现的功能		对应的类		需要到的文件			替代方案			备注
智能指针		无			无				stl::shared_ptr/opencv::Ptr	C++有auto_ptr,但缺点很多，C++11收入了其他智能指针，shared_ptr是使用引用计数的。opencv的Ptr也是一个可选择的智能指针
线程管理                tthread::thread		tinythreadpp/*.*(源码位于sdk下)     std::thread			C++11标准有std::thread,但不少编译器不支持，使用TinyThread++库暂时替换
			tthread::mutex
			tthread::recursive_mutex
			tthread::condition_variable
			tthread::lock_guard
			tthread::fast_mutex
日志库 			log4z			log4z/*.*(源码位于sdk下)	无				支持按日期分类的日志库
互斥体                  Mutex,AutoLock          mutex.*                         无                     	 	使用opencv2.4.6中的Mutex。TinyThread++,log4z也都有自己的互斥体与单例类
单例模式		Singleton		singleton.h(源码位于common下)	无				无
文件操作            	File			fileunit.*(源码位于common下)	文件读写或C的			无
目录操作		Directory		directory.*(源码位于common下)	无				目录操作功能比较少
内存映射		ShareMemory		sharememory.*			无				无
INI读写类		INI			INI*.h(源码位于common下)	无				https://code.google.com/p/feather-ini-parser/上的库(1.29版)，wiki有例子
▓新增

▓修改

▓删除

▓漏洞修复



◆库文件类
mycrypto--本人设计的密钥管理(基于openssl，但库不依赖它)动态库
videoio--基于ffmpeg的视频读写库。可读多种视频格式，写只支持avi. lib开头为gcc库。Video开头为vc库。

◆测试工程
Test_common--vc与cb测试工程

####################################################0.1(2013-04-27)##################################################
common诞生。