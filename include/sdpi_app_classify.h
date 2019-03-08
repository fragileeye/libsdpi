#ifndef _SDPI_APP_CLASSIFY_H
#define _SDPI_APP_CLASSIFY_H

typedef enum _sdpi_app_category
{
	SDPI_APP_CATEGORY_UNKNOWN, 			// 无法识别的数据包
	SDPI_APP_CATEGORY_NETWORK, 			// 网络基础设施的数据包
	SDPI_APP_CATEGORY_VIDEO, 			// 视频
	SDPI_APP_CATEGORY_TVLIVE, 			// 直播
	SDPI_APP_CATEGORY_GAME, 			// 游戏
	SDPI_APP_CATEGORY_CHAT, 			// 聊天
	SDPI_APP_CATEGORY_MAIL, 			// 邮箱
	SDPI_APP_CATEGORY_SOCIAL_NETWORK, 	// 社交网络
	SDPI_APP_CATEGORY_SHOPPING, 		// 购物
	SDPI_APP_CATEGORY_P2P,				// P2P流(迅雷，emule etc..)
	SDPI_APP_CATEGORY_DOWNLOAD, 		// ftp, http 下载
	SDPI_APP_CATEGORY_DATABASE, 		// database
	SDPI_APP_CATEGORY_REMOTE_ACCESS, 	// 远程访问 （SSH, 3389 etc..)
	SDPI_APP_CATEGORY_WEB_COMMON,		// 普通Web应用（与前面分类可能存在包含关系，但其不指代具体应用）
	SDPI_APP_CATEGORY_OS, 				// 操作系统产生的数据流
	//To be continue...
}sdpi_app_category_t;

typedef enum _sdpi_app_type
{
	//belong to category unknown 
	SDPI_APP_UNKNOWN,

	//belong to category 'network'
	SDPI_APP_NETWORK_ICMP,
	SDPI_APP_NETWORK_TCP,
	SDPI_APP_NETWORK_UDP,

	//belong to category 'video'
	SDPI_APP_VIDEO_BILIBILI, 			//[*HOST*]
	SDPI_APP_VIDEO_YOUKU,				//[*HOST*]
	SDPI_APP_VIDEO_IQIYI,				//[*HOST*]
	SDPI_APP_VIDEO_TENCENT,				//[*HOST*]
	SDPI_APP_VIDEO_SOHU,				//[*HOST*]
	SDPI_APP_VIDEO_QQ,					//[*HOST*]
		
	//belong to category 'live tv'
	SDPI_APP_LIVE_BILIBILI,				//[*HOST*]
	SDPI_APP_LIVE_DOUYU,				//[*HOST*]
	SDPI_APP_LIVE_HUYA,					//[*HOST*]
	SDPI_APP_LIVE_PANDA,				//[*HOST*]
	SDPI_APP_LIVE_HUOMAO,				//[*HOST*]
	SDPI_APP_LIVE_QQ,					//[*HOST*]

	//belong to category 'game'
	SDPI_APP_GAME_STEAM,				//[*IP + HOST*]
	SDPI_APP_GAME_11,					//[*IP + HOST*]
	SDPI_APP_GAME_09,					//[*IP + HOST*]
	SDPI_APP_GAME_LOL,					//[*IP + HOST*]
	SDPI_APP_GAME_GLORY,				//[*IP + HOST*]

	//belong to category 'chat'	
	SDPI_APP_CHAT_WECHAT,				//[*HOST*]
	SDPI_APP_CHAT_QQ,					//[*IP + PAYLOAD*]：QQ协议本身

	//belong to category 'mail'
	SDPI_APP_MAIL_UNKNOWN,				//[*PORT*]
	SDPI_APP_MAIL_163, 					//[*HOST*]
	SDPI_APP_MAIL_QQ, 					//[*HOST*]
	SDPI_APP_MAIL_GMAIL,				//[*HOST*]
	SDPI_APP_MAIL_OUTLOOK,				//[*HOST*]

	SDPI_APP_SHOPPING_TAOBAO,			//[*HOST*]
	SDPI_APP_SHOPPING_TMALL,			//[*HOST*]
	SDPI_APP_SHOPPING_JD,				//[*HOST*]
	SDPI_APP_SHOPPING_AMAZON,			//[*HOST*]			

	//belong to category 'social network'
	SDPI_APP_WEIBO_SINA,				//[*HOST*]
	
	//belong to category 'p2p'
	SDPI_APP_P2P_XUNLEI,				//[*PORT: 12345*]

	//belong to category 'download'
	SDPI_APP_DOWNLOAD_FTP,				//[*PORT：20/21*]		

	//belong to category 'remote access'
	SDPI_APP_REMOTE_ACCESS_SSH, 		//[*PORT: 22/22222*]
	SDPI_APP_REMOTE_ACCESS_TELNET,		//[*PORT: 23*]
	SDPI_APP_REMOTE_ACCESS_RDP,			//[*PORT: 3389*]
	SDPI_APP_REMOTE_ACCESS_TEAMVIEW, 	//[*PORT: 5938*]

	//belong to category 'database'
	SDPI_APP_DATABASE_SQLSERVER, 		//[*PORT: 1433*]
	SDPI_APP_DATABASE_MYSQL,			//[*PORT: 3306*]

	//belong to category 'os'
	SDPI_APP_OS_WINDOWS,
	SDPI_APP_OS_LINUX,

	//belong to common website
	SDPI_APP_COMMON_QQ,
	SDPI_APP_COMMON_BAIDU,
	SDPI_APP_COMMON_360,  				//oh, fuck 360!
	SDPI_APP_COMMON_SOHU, 

}sdpi_app_type_t;

#endif 