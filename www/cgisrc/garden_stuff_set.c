#include "msg.h"

char temp_up[12] = { 0 };
char temp_low[12] = { 0 };
char hum_up[12] = { 0 };
char hum_low[12] = { 0 };
char illu_up[12] = { 0 };
char illu_low[12] = { 0 };
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
int cgiMain(int argc, char const* argv[])
{
    cgiFormString("temp_up", temp_up, sizeof(temp_up));
    cgiFormString("temp_low", temp_low, sizeof(temp_low));
    cgiFormString("hum_up", hum_up, sizeof(hum_up));
    cgiFormString("hum_low", hum_low, sizeof(hum_low));
    cgiFormString("illu_up", illu_up, sizeof(illu_up));
    cgiFormString("illu_low", illu_low, sizeof(illu_low));
    int ret;
    msg.msgtype = 2;
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
    msg.limit.hume_low = atof(hum_low);
    msg.limit.hume_up = atof(hum_up);
    msg.limit.temp_low = atof(temp_low);
    msg.limit.temp_up = atof(temp_up);
    msg.limit.iull_low = atof(illu_low);
    msg.limit.iull_up = atof(illu_up);
    msg.devc.dev0ORset1 = 0; 
    msgsnd(msgqid, &msg, MSGSIZE, 0);
    alarm(5);
    msgrcv(msgqid, &msg, MSGSIZE, recvtype, 0);
    alarm(5);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","Datatempup",temp_up);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","Datatemplow",temp_low);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","Datahumup",hum_up);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","Datahumlow",hum_low);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","Datailluup",illu_up);
    fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","Dataillulow",illu_low);
    printf("Content-Type:text/html\n\n");
    printf("<meta http-equiv='refresh' content='0;url=../greenhouse/garden_stuff.html'/>");
    return 0;
}