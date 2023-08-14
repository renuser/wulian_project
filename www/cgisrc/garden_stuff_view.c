#include "msg.h"

char cook[20] = {0}; 
volatile int recvtype = 0;
char *s = cook; 
Msg_t msg;
void alarm_handle(int signo){
    msgrcv(msgqid,&msg,MSGSIZE,1,IPC_NOWAIT);
	msgrcv(msgqid,&msg,MSGSIZE,recvtype,IPC_NOWAIT);
	printf("Content-Type:text/html;charset=utf-8\n\n");
	printf("<html>");
	printf("<body>");
	printf("<h3>网络有原因导致数据不可达</h3>");
	printf("</body>");
	printf("</html>");
	exit(0);
}
int cgiMain(int argc, char const* argv[])
{
	char *cookieString = getenv("HTTP_COOKIE");
    if (cookieString != NULL) {
        // 指定的Cookie名称
        const char *targetCookie = "username";
        // 在Cookie字符串中查找指定的Cookie值
        char *targetValue = strstr(cookieString, targetCookie);
        if (targetValue != NULL) {
            targetValue += strlen(targetCookie) + 1; // 跳过等号
            char *endOfValue = strchr(targetValue, ';');
            if (endOfValue != NULL) {
                *endOfValue = '\0'; // 截断字符串以获取指定的Cookie值
            }
			memcpy(cook,targetValue,sizeof(cook));
        } 
    }


    signal(SIGALRM,alarm_handle);
    int ret;
    msg.msgtype = 1;
    if ((key = ftok("/home/linux",'f')) == -1) {
         PRINT_ERROR("ftok error");
    }
    // 创建消息队列
    if ((msgqid = msgget(key, IPC_CREAT | 0666)) == -1) {
        PRINT_ERROR("msgget error");
    }
    //memcpy(cook,cgiCookie[0],128);
    while(0!=*s){
		recvtype+=*s;
		s++;	
	}
	int a = 0;
    int b = 100;
    msg.r_type = recvtype + (a + (b - a + 1) * rand() / (RAND_MAX + 1.0));
    
    //发消息
    msgsnd(msgqid,&msg,MSGSIZE,0);
    //收消息
    alarm(5);
    msgrcv(msgqid,&msg,MSGSIZE,recvtype,0);
    printf("Content-Type:text/html;charset=utf-8\n\n");
	printf("<html>");
	printf("<body bgcolor=\"#02c874\">");
	printf("<center>");
	printf("<center>");	
	printf("<table border = \"0.1\" align=\"center\">");
	printf("<tr>");
	printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">用户名:</td>");
	printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">%s</td>",cook);
	printf("</tr>");
	printf("</table>");
	printf("<h4>环境信息</h4>");
	printf("<table border = \"0.3\" align=\"center\">");
	printf("<th bgcolor=\"#0080FF\"> 温度(c) </th>");
	printf("<th bgcolor=\"#0080FF\"> 湿度(hum) </th>");
	printf("<th bgcolor=\"#0080FF\"> 光照(lux) </th>");
	printf("<th bgcolor=\"#0080FF\"> 电压(v) </th>");
	printf("<tr>");
	printf("<td style='text-align: center;' bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">%.2f</td>",msg.env.temp);
	printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"120\" height=\"20\">%.2f</td>",msg.env.hume);
	printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"115\" height=\"20\">%d</td>",msg.env.iull);
	printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">%.2f</td>",msg.env.volt/1000);
	printf("</tr>");
	printf("</table>");
	printf("<h4>设备状态</h4>");
	printf("<table border = \"0.5\" align=\"center\">");
	printf("<th bgcolor=\"#0080FF\"> 照明 </th>");
	printf("<th bgcolor=\"#0080FF\"> 空调 </th>");
	printf("<th bgcolor=\"#0080FF\"> 通风 </th>");
	printf("<th bgcolor=\"#0080FF\"> 警报 </th>");
	printf("<tr>");
	if(1==msg.env.led_state){
		printf("<td style='text-align: center;' bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">开</td>");

	}else{
		printf("<td style='text-align: center;' bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">关</td>");

	}
	if(1==msg.env.fan_state){
		printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">开</td>");

	}else{
		printf("<td  style='text-align: center;' bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">关</td>");

	}
	if(1==msg.env.moto_state){
		printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">开</td>");

	}else{
		printf("<td style='text-align: center;' bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">关</td>");

	}
	if(1==msg.env.beep_state){
		printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">开</td>");
	}else{
		printf("<td style='text-align: center;'  bgcolor=\"#00FFFF\" width=\"80\" height=\"20\">关</td>");
	}
	printf("</tr>");
	printf("</table>");
	printf("</center>");
	printf("</center>");
	printf("</body>");
	printf("</html>");
	//printf("<head><meta http-equiv=\"refresh\" content=\"3\"</head>");//自动刷新网页  1秒钟刷一次
    return 0;
}