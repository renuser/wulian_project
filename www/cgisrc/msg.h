#ifndef __MSG_H__
#define __MSG_H__
#include <cgic.h>
#include <myhead.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#define ERRLOG(msg)                                         \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        perror(msg);                                        \
        exit(-1);                                           \
    } while (0);

key_t key;
int msgqid;

typedef struct{
	float temp;
	float hume;
	int iull;
	float volt;
	char led_state;
	char fan_state;  //风扇
	char moto_state;	//电机
	char beep_state;
} EnvData_t;
typedef struct{
	/*温湿度  光强  电压...上下限设置*/
	int temp_up;
	int temp_low;
	int hume_up;
	int hume_low;
	int iull_up;
	int iull_low;
}Limitset_t;

typedef struct{
	/*照明、通风、温控...设备控制*/
	char led_ctrl;
	char fan_ctrl;
	char moto_ctrl;
	char beep_ctrl;
	int dev0ORset1;
}Devctlr_t;

typedef struct{
	long msgtype;// 1 获取环境信息   2  设置阈值   3 设备控制
	int r_type;
	EnvData_t env;
	Limitset_t limit;
	Devctlr_t devc;
}Msg_t;

#define MSGSIZE  (sizeof(Msg_t)-sizeof(long))

#endif