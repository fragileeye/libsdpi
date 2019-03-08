#ifndef _SDPI_HOST_CONFIG_H
#define _SDPI_HOST_CONFIG_H
#include "sdpi_flow_parser.h"

typedef struct _sdpi_host_config
{
	char					*host_name;
	sdpi_inspect_result_t	result;
}sdpi_host_config_t;

static sdpi_inspect_result_t
sdpi_unknown_host_result = \
	{SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_UNKNOWN, SDPI_APP_CATEGORY_UNKNOWN, "host unknown"};

static const sdpi_host_config_t 
sdpi_host_config_list[] = \
{
	//视频
	{ "bilibili.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_BILIBILI, SDPI_APP_CATEGORY_VIDEO, "bilibili video"} },
	{ "youku.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_YOUKU, SDPI_APP_CATEGORY_VIDEO, "youku video" } },
	{ "tudou.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_YOUKU, SDPI_APP_CATEGORY_VIDEO, "tudou video" } },
	{ "iqiyi.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_IQIYI, SDPI_APP_CATEGORY_VIDEO, "iqiyi video" } },
	{ "tv.sohu.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_SOHU, SDPI_APP_CATEGORY_VIDEO, "sohu video" } },
	{ "pv.sohu.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_SOHU, SDPI_APP_CATEGORY_VIDEO, "sohu video" } },
	{ "v.qq.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_VIDEO_QQ, SDPI_APP_CATEGORY_VIDEO, "qq video" } },

	//直播
	{ "live.bilibili.com",	{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_LIVE_BILIBILI, SDPI_APP_CATEGORY_TVLIVE, "bilibili live" } },
	{ "douyu.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_LIVE_DOUYU, SDPI_APP_CATEGORY_TVLIVE, "douyu live" } },
	{ "huya.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_LIVE_HUYA, SDPI_APP_CATEGORY_TVLIVE, "huya live" } },
	{ "panda.tv", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_LIVE_PANDA, SDPI_APP_CATEGORY_TVLIVE, "panda live" } },
	{ "huomao.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_LIVE_HUOMAO, SDPI_APP_CATEGORY_TVLIVE, "huomao live" } },
	{ "live.qq.com",		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_LIVE_QQ, SDPI_APP_CATEGORY_TVLIVE, "qq live" } },

	//聊天
	{ "wx.qq.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_CHAT_WECHAT, SDPI_APP_CATEGORY_CHAT, "wechat" } },
	{ "wx2.qq.com",			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_CHAT_WECHAT, SDPI_APP_CATEGORY_CHAT, "wechat" } },

	//邮箱
	{ "mail.163.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_163, SDPI_APP_CATEGORY_MAIL, "wangyi mail" } },
	{ "mail.126.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_163, SDPI_APP_CATEGORY_MAIL, "wangyi mail" } },
	{ "mail.yeah.net", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_163, SDPI_APP_CATEGORY_MAIL, "wangyi mail" } },
	{ "mail.qq.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_QQ, SDPI_APP_CATEGORY_MAIL, "qq mail" } },
	{ "outlook.live.com", 	{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_OUTLOOK, SDPI_APP_CATEGORY_MAIL, "outlook mail" } },
	{ "office365.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_OUTLOOK, SDPI_APP_CATEGORY_MAIL, "outlook mail" } },
	{ "gmail.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_MAIL_163, SDPI_APP_CATEGORY_MAIL, "gmail mail" } },

	//购物
	{ "taobao.com",  		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_SHOPPING_TAOBAO, SDPI_APP_CATEGORY_SHOPPING, "taobao shopping" } },
	{ "tmall.com",  		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_SHOPPING_TMALL, SDPI_APP_CATEGORY_SHOPPING, "tmall shopping" } },
	{ "jd.com",  			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_SHOPPING_JD, SDPI_APP_CATEGORY_SHOPPING, "jd shopping" } },
	{ "amazon.cn",  		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_SHOPPING_TAOBAO, SDPI_APP_CATEGORY_SHOPPING, "amazon shopping" } },

	//社交网站
	{ "weibo.com",  		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_WEIBO_SINA, SDPI_APP_CATEGORY_SOCIAL_NETWORK, "sina weibo" } },

	//访问较广的普遍应用
	{ "qq.com",  			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_COMMON_QQ, SDPI_APP_CATEGORY_WEB_COMMON, "qq common" } },
	{ "tencent.com", 		{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_COMMON_QQ, SDPI_APP_CATEGORY_WEB_COMMON, "tencent common" } },
	{ "baidu.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_COMMON_BAIDU, SDPI_APP_CATEGORY_WEB_COMMON, "baidu common" } },
	{ "360.cn", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_COMMON_360, SDPI_APP_CATEGORY_WEB_COMMON, "360 common" } },
	{ "sohu.com", 			{ SDPI_INSPECTED_BY_PAYLOAD, SDPI_APP_COMMON_SOHU, SDPI_APP_CATEGORY_WEB_COMMON, "sohu common" } },
};

#endif