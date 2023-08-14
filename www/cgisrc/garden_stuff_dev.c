#include "msg.h"
char cook[120] = { 0 };
volatile unsigned int recvtype = 0;
char* s = cook;
time_t cur_time;
Msg_t msg;
void alarm_handle(int signo)
{
    msgrcv(msgqid, &msg, MSGSIZE, 3, IPC_NOWAIT);
    msgrcv(msgqid, &msg, MSGSIZE, recvtype, IPC_NOWAIT);
    printf("Content-Type:text/html;charset=utf-8\n\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<meta charset=\"utf-8\">\n");
    printf("<script>\n");
    printf("alert(\"网络原因导致数据不可达%d\");\n", recvtype);
    printf("</script>\n");
    printf("<title>错误提示</title>\n");
    printf("</head>\n");
    printf("</html>\n");
    exit(0);
}
char aa[8] = { 0 };
char FAN[8] = { 0 };
char LED1[8] = { 0 };
char BEEP[8] = { 0 };
int cgiMain(int argc, char const* argv[])
{
    // memset(LED,0,sizeof(LED));
    cgiFormString("aa", aa, sizeof(aa));
    cgiFormString("FAN", FAN, sizeof(FAN));
    cgiFormString("led1", LED1, sizeof(LED1));
    cgiFormString("BEEP", BEEP, sizeof(BEEP));
    signal(SIGALRM, alarm_handle);
    int ret;
    msg.msgtype = 3;
    if ((key = ftok("/home/linux", 'f')) == -1) {
        PRINT_ERROR("ftok error");
    }
    // 创建消息队列
    if ((msgqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        PRINT_ERROR("msgget error");
    }
    memcpy(cook, cgiCookie, 128);
    while (0 != *s) {
        recvtype += *s;
        s++;
    }
    int a = 0;
    int b = 100;
    msg.r_type = recvtype + (a + (b - a + 1) * rand() / (RAND_MAX + 1.0));
    if (strcmp(LED1, "ON") == 0) {
        msg.devc.led_ctrl = 1;
    } else {
        msg.devc.led_ctrl = 0;
    }
    if (strcmp(FAN, "ON") == 0) {
        msg.devc.fan_ctrl = 1;
    } else {
        msg.devc.fan_ctrl = 0;
    }
    if (strcmp(BEEP, "ON") == 0) {
        msg.devc.moto_ctrl = 1;
    } else {
        msg.devc.moto_ctrl = 0;
    }
    msg.devc.dev0ORset1 = 1;
    // 发消息
    msgsnd(msgqid, &msg, MSGSIZE, 0);
    // 收消息
    // alarm(5);
    // msgrcv(msgqid, &msg, MSGSIZE, recvtype, 0);
    // alarm(5);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","ledstate",LED1);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","fanstate",FAN);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","motostate",BEEP);

    printf("Content-Type:text/html\n\n");
    printf("<html>");
    printf("<body>");
    printf("<script>");
    printf("document.location.href='../greenhouse/garden_stuff.html'");
    printf("</script>");
    printf("</body>");
    printf("</html>");
    return 0;
}