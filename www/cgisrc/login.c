#include <cgic.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#define ERRLOG(msg)                                         \
    do {                                                    \
        printf("%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        perror(msg);                                        \
        exit(-1);                                           \
    } while (0);

#define DATABASE "../database/user.db"
sqlite3* users_init()
{
    sqlite3* users_db = NULL;
    int ret = sqlite3_open(DATABASE, &users_db);
    if (ret != SQLITE_OK) {
        ERRLOG("sqlite3_open error");
    }
    printf("打开[%s]数据库成功!\n", DATABASE);
    return users_db;
}

typedef struct users {
    int userid;
    char nickname[16];
    char password[16];
} users_t;
char uid[12] = { 0 };
char password[12] = { 0 };

int login_callback(void* arg, int column, char** fvalues, char** fnames)
{
    users_t* user = (users_t*)arg;
    user->userid = atoi(fvalues[0]);
    strcpy(user->nickname, fvalues[1]);
    strcpy(user->password, fvalues[2]);
    return 0;
}
int cgiMain(int argc, const char* argv[])
{
    cgiFormString("ID", uid, sizeof(uid));
    cgiFormString("PASSWORD", password, sizeof(password));
    users_t* user = (users_t*)malloc(sizeof(users_t));
    sqlite3* userdb = users_init();
    char sql[128] = { 0 };
    sprintf(sql, "select * from users where uid=%d and pwd='%s'", atoi(uid), password);
    if (sqlite3_exec(userdb, sql, login_callback, user, NULL) != SQLITE_OK) {
        //ERRLOG("sqlite3_exec error");
    }
    
    if ((user->userid == atoi(uid)) && (strcmp(user->password, password) == 0) && ((atoi(uid) != 0) && (password != NULL))) {
        // 登录成功
        fprintf(cgiOut,"Set-Cookie: %s=%s;  path=/\r\n","username",user->nickname);
        printf("Content-Type:text/html\n\n");
        printf("<html>");
        printf("<body>");
        printf("<script>");
        printf("document.location.href='../Iot_select.html'");
        printf("</script>");
        printf("</body>");
        printf("</html>");
    } else {
        // 登陆失败
        printf("Content-Type:text/html\n\n");
        printf("<head>");
        printf("<meta charset='UTF-8'>");
        printf("<title>登陆失败</title>");
        printf("</head>");
        printf("<body>");
        printf("<center>");
        printf("<div>");
        printf("<h1>登陆失败,password or id error!</h1>");
        printf("</div>");
        printf("<div>");
        printf("<h2>");
        printf("<a href='../index.html'>点击返回再次登录</a>");
        printf("</h2>");
        printf("</div>");
        printf("<img src='../imge/780.png'>");
        printf("</center>");
        printf("</body>");
        printf("</html>");
        return -1;
    }
    return 0;
}