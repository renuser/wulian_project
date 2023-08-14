#include "project.h"

extern int temp_up;
extern int temp_low;
extern int hum_up;
extern int hum_low;
extern int illu_up;
extern int illu_low;
extern float v_temp;
extern float v_hum;
extern float v_iull;
extern int dev0orset1;
int main(int argc, const char* argv[])
{
    Msg_t msg;
    /*设备初始化*/
    dev_init();
    /*消息队列初始化*/
    msgqid = msgqueue_init();
    /*线程初始化*/
    thread_init();
    /*维护环境*/
    while (1) {
        /*读取环境温湿度数据  用数据与用户阈值进行比较
         * 根据比较结果控制设备的启停操作*/
        if (dev0orset1 == 0) {
            if (v_temp > temp_up && (temp_up != 0)) {
                msg.msgtype = 3;
                msg.devc.fan_ctrl = 1; // 打开空调
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            } else if (v_temp < temp_low) {
                msg.msgtype = 3;
                msg.devc.fan_ctrl = 1; // 打开空调
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            } else {
                msg.msgtype = 3;
                msg.devc.fan_ctrl = 0; // 关闭空调
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            }

            if (v_hum > hum_up && (hum_up != 0)) {
                msg.msgtype = 3;
                // 打开马达，表示灌溉
                msg.devc.moto_ctrl = 1;
                msg.devc.beep_ctrl = 0; // 关闭蜂鸣器
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            } else if (v_hum < hum_low) {
                msg.msgtype = 3;
                msg.devc.moto_ctrl = 0; // 关闭马达,停止灌溉
                msg.devc.beep_ctrl = 1; // 打开蜂鸣器，表示提醒通风
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            } else {
                msg.msgtype = 3;
                msg.devc.moto_ctrl = 0; // 关闭马达
                msg.devc.beep_ctrl = 0; // 关闭蜂鸣器
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            }

            if (v_iull < illu_low) {
                msg.msgtype = 3;
                // 打开灯光
                msg.devc.led_ctrl = 1;
                msg.devc.beep_ctrl = 0; // 关闭蜂鸣器
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            } else if (v_iull > illu_up && (illu_up != 0)) {
                msg.msgtype = 3;
                msg.devc.led_ctrl = 0; // 关闭灯光
                msg.devc.beep_ctrl = 1; // 打开蜂鸣器，提醒光照过强应采取相应措施
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            } else {
                msg.msgtype = 3;
                msg.devc.led_ctrl = 0; // 关闭灯光
                msg.devc.beep_ctrl = 0; // 关闭蜂鸣器
                msgsnd(msgqid, &msg, MSGSIZE, 0);
            }
        }
        sleep(2);
    }

    return 0;
}
