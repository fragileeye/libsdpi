# libsdpi

libsdpi是一个轻量级的dpi库，主要思想是可定制化与可配置化开发，尽管配置主要修改xx_config.h头文件，但是实在没精力再去写解析的代码来进行配置，感觉那些东西都不重要了。该项目是suiyan的第一个个人项目，作为2018年的一个成果，特此留念。

使用方法：
	1) 编译静态库：(Windows下直接CMake，Linux下不推荐CMake)
		a)Windows系统
		在VS新建静态链接库的项目，导入所有.h和.c的文件，编译即可生成libsdpi.lib；随后在程序中#pragma comment(lib, "libsdpi")即完成库的导入。
		b)Linux系统
		gcc -c *.c 生成.o文件，随后rcs将所有.o文件链接成.a文件；编译程序的时候加上.a库文件的路径名即可。

	2) 包含头文件: 
		#include<sdpi_core_manager.h>

	3) 定制开发:
		a) sdpi_load_core_manager()加载core_manager,并在程序结束(如果不是死循环服务)卸载sdpi_unload_core_manager();
		b) core_manager->register_inspect注册类型为core_context的上下文，用以定制开发识别模块;
		c) core_manager->inspect_packet()识别原始数据包(帧)并得到识别结果，core_manager->inspect_struct()识别结构化数据并得到识别结果;

有疑问请看代码以及漂流瓶联系:
	除非"真香",不然这个库不会更新了~实在是没有时间啊~
