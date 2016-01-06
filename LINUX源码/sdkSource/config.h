/*
 _
(_)  _ __     __ _   _   _   _ __     ___   __      __
| | | '_ \   / _` | | | | | | '_ \   / _ \  \ \ /\ / /
| | | |_) | | (_| | | |_| | | | | | | (_) |  \ V  V /
|_| | .__/   \__,_|  \__, | |_| |_|  \___/    \_/\_/
    |_|              |___/

读写配置文件
author : nxd
time	: 2015.8.5
*/

#include <stdio.h>


#define NEWLINE "\n"


#define MAX_CFG_FILELEN 16384
#define CONFIG_FILE_NAME "ipaynowSDK.cfg"

/* 定义配置文件字段 */
#define CFG_MID					"mid"
#define CFG_TID					"tid"
#define CFG_KEY					"key"

#define CFG_IS_AUTO_SIGN		"is_auto_sign"
#define CFG_SIGNUP_TIMES		"signup_times"

#define CFG_WXQUERY_TIME		"wxquery_time"
#define CFG_WXQUERY_TOTAL_TIME	"wxquery_total_time"

#define CFG_ALIPAY_TIME			"alipay_time"
#define CFG_ALIPAY_TOTAL_TIME	"alipay_total_time"

#define CFG_BATCHNO				"batchno"
#define CFG_TRANSNO				"transno"
#define CFG_IS_SIGNUP			"is_signup"

#define CFG_MAK					"mak"
#define CFG_PIK					"pik"
#define CFG_IS_USEFUL			"is_useful"

#define CFG_IP					"ip"
#define CFG_PORT				"port"

#define CFG_CONNECT_OVERTIME	"connect_overtime"
#define CFG_SEND_OVERTIME		"send_overtime"
#define CFG_RECEIVE_OVERTIME	"receive_overtime"


/*读取配置文件内容*/
int CFG_GetProfileString(char *conf_name, char *config_buff);

/*修改配置文件内容 修改内容为 "path=9527" 不可只填9527*/
int CFG_SetProfileString(char *conf_name, char *config_buff);

/*删除配置*/
int CFG_DelProfileString(char *conf_name);