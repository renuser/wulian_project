#include "project.h"
#define GET_TEMP _IOR('y', 0, int)
#define GET_HUM _IOR('y', 1, int)
int fd1, fd2, fd3, fd4, fd5, fd6, fd7, fd8;
int fanstate = 0;
int motostate = 0;
int beepstate = 0;
int temp_up = 0;
int temp_low = 0;
int hum_up = 0;
int hum_low = 0;
int illu_up = 0;
int illu_low = 0;
int dev0orset1 = 0;
float v_temp = 0;
float v_hum = 0;
float v_iull = 0;
/*读取消息线程*/
void* msgget_thread(void* argv)
{
    pthread_t tid1, tid2,tid3;
    Msg_t msg;
    pthread_detach(pthread_self());
    while (1) {
        /*读取消息队列*/
        msgrcv(msgqid, &msg, MSGSIZE, -5, 0);
        /*区分消息类型 根据消息类型创建功能线程处理事件*/
        switch (msg.msgtype) {
        case 1:
            /*创建环境信息线程*/
            if (pthread_create(&tid1, NULL, env_thread, &msg) != 0) {
                ERRLOG("pthread_creat error");
            }
            break;
        case 2:
            /*创建阈值设置线程*/
            if (pthread_create(&tid2, NULL, limit_thread, &msg) != 0) {
                ERRLOG("pthread_creat error");
            }
            break;
        case 3:
            /*创建设备控制线程*/
            if (pthread_create(&tid3, NULL, dev_thread, &msg) != 0) {
                ERRLOG("pthread_creat error");
            }
            break;
        default:
            break;
        }
    }
    close(fd1);
    close(fd2);
    close(fd4);
    close(fd5);
    close(fd6);
    close(fd7);
    close(fd8);
    pthread_exit(NULL);
}

/*环境信息线程*/
void* env_thread(void* argv)
{
    Msg_t* msg = (Msg_t*)argv;
    pthread_detach(pthread_self());
    char current_file[256] = "/sys/bus/iio/devices/iio:device0/in_voltage0_raw";
    char voltage_file[256] = "/sys/bus/iio/devices/iio:device0/in_voltage1_raw";
    int tmp, hum;
    float rtmp, rhum;
    int data;
    float dataV = 0;
    float dataI = 0;
    int ret;
    int ledstate = 0;
    /*读取设备数据*/
    // 读取电流电压值
    char buf[10] = { 0 };
    if ((fd4 = open(voltage_file, O_RDWR)) < 0) {
        perror("open");
    }
    if ((fd3 = open(current_file, O_RDWR)) < 0) {
        perror("open");
    }
    ret = read(fd3, buf, 10);
    dataI = atoi(buf);
    dataI = ((((float)3300 * dataI) / 65536) / (R_10 + R_100K + R_1K)) * R_1K / R_0D1;
    memset(buf, 0, sizeof(buf));
    ret = read(fd4, buf, 10);
    if (ret < 0) {
        perror("read");
    }
    dataV = atoi(buf);
    dataV = (float)3300 * dataV / 65536;

    // 读取温湿度
    ioctl(fd1, GET_TEMP, &tmp);
    ioctl(fd1, GET_HUM, &hum);
    rtmp = 175.72 * tmp / 65536 - 46.85;
    rhum = 125.0 * hum / 65536 - 6;
    // 读取光照强度
    read(fd2, &data, sizeof(data));
    data = data * 0.35;
    // 读取灯的状态
    read(fd5, &ledstate, sizeof(int));
    /*封装消息*/
    msg->msgtype = msg->r_type;
    msg->env.temp = rtmp;
    msg->env.hume = rhum;
    msg->env.iull = data;
    msg->env.volt = dataV;
    msg->env.led_state = ledstate;
    msg->env.fan_state = fanstate;
    msg->env.moto_state = motostate;
    msg->env.beep_state = beepstate;
    /*发送消息*/
    msgsnd(msgqid, msg, MSGSIZE, 0);
    // 给主线程封装数据
    v_temp = msg->env.temp;
    v_hum = msg->env.hume;
    v_iull = msg->env.iull;
    close(fd3);
    close(fd4);
    /*线程退出*/
    pthread_exit(NULL);
}

/*阈值设置线程*/
void* limit_thread(void* argv)
{
    pthread_detach(pthread_self());
    Msg_t* msg = (Msg_t*)argv;
    dev0orset1 = msg->devc.dev0ORset1; 
    /*为主线程参考变量赋值*/
    hum_low = msg->limit.hume_low;
    hum_up = msg->limit.hume_up;
    temp_up = msg->limit.temp_up;
    temp_low = msg->limit.temp_low;
    illu_up = msg->limit.iull_up;
    illu_low = msg->limit.iull_low;
    /*封装消息*/
    msg->msgtype = msg->r_type;
    /*发送消息*/
    msgsnd(msgqid, msg, MSGSIZE, 0);
    /*线程退出*/
    pthread_exit(NULL);
}

/*设备控制线程*/
void* dev_thread(void* argv)
{
    pthread_detach(pthread_self());
    Msg_t* msg = (Msg_t*)argv;
    /*根据msg.dev 中成员值控制设备*/
    int led1 = 1;
    int valuefan = 5; // 风扇的周期设置越大周期越长，频率越低
    int valuemoto = 500;
    int valuebeep = 20;
    int valueoff = 0; // 关闭风扇等设备
    dev0orset1 = msg->devc.dev0ORset1;
    if (msg->devc.led_ctrl == 1) {
        ioctl(fd5, LED_ON, &led1);
    } else {
        ioctl(fd5, LED_OFF, &led1);
    }
    if (msg->devc.fan_ctrl == 1) {
        fanstate = 1;
        write(fd6, &valuefan, sizeof(valuefan));
    } else {
        fanstate = 0;
        write(fd6, &valueoff, sizeof(valueoff));
    }
    if (msg->devc.moto_ctrl == 1) {
        motostate = 1;
        write(fd7, &valuemoto, sizeof(valuemoto));
    } else {
        motostate = 0;
        write(fd7, &valueoff, sizeof(valueoff));
    }
    if (msg->devc.beep_ctrl == 1){
        beepstate = 1;
        write(fd8,&valuebeep,sizeof(valuebeep));
    }else{
        beepstate = 0;
        write(fd8, &valueoff, sizeof(valueoff));
    }
    /*封装消息*/
    msg->msgtype = msg->r_type;
    /*发送消息*/
    // msgsnd(msgqid, msg, MSGSIZE, 0);
    /*线程退出*/
    pthread_exit(NULL);
}

/*消息队列初始化*/
int msgqueue_init(void)
{
    int msgqid;
    /*获取 key*/
    if ((key = ftok("/home/linux", 'f')) == -1) {
        PRINT_ERROR("ftok error");
    }
    // 创建消息队列
    if ((msgqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        PRINT_ERROR("msgget error");
    }
    /*返回 msgid*/
    return msgqid;
}

int dev_init()
{
    // 温湿度
    if ((fd1 = open("/dev/si7006", O_RDWR)) < 0) {
        PRINT_ERROR("open error");
    }
    // 光强
    if ((fd2 = open("/dev/gAP3216C", O_RDWR)) < 0) {
        PRINT_ERROR("open error");
    }
    // led1灯
    if ((fd5 = open("/dev/gpioDriver", O_RDWR)) == -1) {
        PRINT_ERROR("open error");
    }
    // 风扇
    if ((fd6 = open("/dev/fan_driver", O_RDWR)) < 0) {
        perror("open error");
        return -1;
    }
    // 震动马达
    if ((fd7 = open("/dev/vibrator_driver", O_RDWR)) < 0) {
        perror("open error");
        return -1;
    }
    // 蜂鸣器
    if ((fd8 = open("/dev/beeper_driver", O_RDWR)) < 0) {
        perror("open error");
        return -1;
    }
    return 0;
}

/*线程初始化*/
int thread_init(void)
{
    pthread_t tid;
    /*创建消息读取线程*/
    if (pthread_create(&tid, NULL, msgget_thread, NULL) != 0) {
        PRINT_ERROR("pthread_creat error");
    }
    return 0;
}
