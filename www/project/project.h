#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <fcntl.h>
#include <myhead.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#define ESC_KEY 0x1b
#define R_1K 1000
#define R_100K (100 * R_1K)
#define R_10 10
#define R_0D1 0.1
#define LED_ON _IOW('L',1,int)
#define LED_OFF _IOW('L',0,int)
#define GET_CMD_SIZE(cmd) ((cmd >> 16)&0x3fff)
#define ERRLOG(msg)                                         \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        perror(msg);                                        \
        exit(-1);                                           \
    } while (0);

key_t key;
int msgqid;

typedef struct {
    float temp;
    float hume;
    int iull;
    float volt;
    char led_state;
    char fan_state;
    char moto_state;
    char beep_state;
} EnvData_t;

typedef struct {
    int temp_up;
    int temp_low;
    int hume_up;
    int hume_low;
    int iull_up;
    int iull_low;
} Limitset_t;

typedef struct {
    char led_ctrl;
    char fan_ctrl;
    char moto_ctrl;
    char beep_ctrl;
    int dev0ORset1;
} DevCtlr_t;

typedef struct {
    long msgtype;
    int r_type;
    EnvData_t env;
    Limitset_t limit;
    DevCtlr_t devc;
} Msg_t;

// Msg_t msg;
#define MSGSIZE (sizeof(Msg_t) - sizeof(long))
/*读取消息线程*/
void* msgget_thread(void* argv);

/*环境信息线程*/
void* env_thread(void* argv);

/*阈值设置线程*/
void* limit_thread(void* argv);

/*设备控制线程*/
void* dev_thread(void* argv);

/*设备初始化*/
int dev_init(void);

/*消息队列初始化*/
int msgqueue_init(void);

/*线程初始化*/
int thread_init(void);

#endif
