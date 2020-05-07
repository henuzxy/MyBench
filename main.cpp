#include<stdio.h>
#include<unistd.h>
#include<sys/param.h>
#include<sys/wait.h>
#include<rpc/types.h>
#include<getopt.h>
#include<strings.h>
#include<time.h>
#include<signal.h>
#include<string.h>
#include "MySocket.h"
#include "InputInfo.h"
#include "CliEngine.h"
/*代码相关的宏定义*/
#define bool int
#define true (1)
#define false (0)

/*缓冲流大小相关的宏定义*/
#define REQUEST_SIZE (2048)
#define BUF_SIZE (1024)

/*结果统计相关的变量*/
int speed = 0;
int failed = 0;
long long int byte_counts = 0;
volatile int timeout = 0;

/*进程管道*/
int pipe_buf[2];
/*服务器网络地址*/
char host[MAXHOSTNAMELEN];//这个宏在rpc/types.h下

/*请求消息*/
char request[REQUEST_SIZE];


/*相关函数*/
void build_request(const char*url);//构造请求消息
static int bench();/*压力测试*/
static void alarm_handler(int signal);/*闹钟信号处理*/
void benchcore(const char* host,const int port,const char* req);
void error_handling(const char* message);
int main(int argc,char* argv[]){
    
    
    if(argc == 1){
        exit(1);
    }
    CliEngine::getinstance().Usage();

    // /*构造请求消息*/ 
    // build_request(argv[optind]);
    // fprintf(stdout,"OK\n");
    // /**/
    // fprintf(stdout,"Webbench --一款网站压力测试程序\n");
    // fprintf(stdout,"测试链接:%s\n",argv[optind]);
    // fprintf(stdout,"运行信息:");
    // fprintf(stdout,"%d 客户端, 运行 %d 秒",clients,bench_time);
    // if(force)
    //     fprintf(stdout,",提前关闭连接");
    // if(proxyhost != NULL)
    //     fprintf(stdout,"，使用代理服务器 %s:%d",proxyhost,proxy_port);
    // if(reload)
    //     fprintf(stdout,",无缓存连接");
    // fprintf(stdout,".\n");
   
    // return bench();
}

void build_request(const char *url){
    char temp[10];
    /**/
    memset(host,0,MAXHOSTNAMELEN);
    memset(request,0,REQUEST_SIZE);
   
    /*
        http0.9只有get这一种方法,不支持缓存机制.
        http1.0有get,post,head三种方法，支持长连接，缓存机制
        http1.1 四种方法都有.
    */
    if(reload && proxyhost != NULL && http == 0)
        http = 1;
    if(method == METHOD_HEAD && http == 0)
        http = 1;
    if(method == METHOD_OPTIONS && http < 2)
        http = 2;
   
    switch(method){
        default:
        case METHOD_GET:
            strcpy(request,"GET ");
            break;
        case METHOD_HEAD:
            strcpy(request,"HEAD ");
            break;
        case METHOD_OPTIONS:
            strcpy(request,"OPTIONS ");
            break;
        case METHOD_TRACE:
            strcpy(request,"TRACE ");
            break;
    }
    
    if(strstr(url,"://") == NULL){
        fprintf(stderr,"\n%s 不是一个合法的URL",url);
        exit(2);
    }
    if(strlen(url) > 1000){
        fprintf(stderr,"URL 长度超过合法值");
        exit(2);
    }
    if(strncasecmp("http://",url,7) != 0){
        fprintf(stderr,"只有http协议是被支持的.");
        exit(2);
    }
    /*主机名起始位置*/
    int pos = strstr(url,"://")-url+3;
    
    if(strchr(url+pos,'/') == NULL){
        fprintf(stderr,"url不合法，地址结束位置必须是 '/'.\n");
        exit(2);
    }
    /*未使用代理服务器时*/
    if(proxyhost == NULL){
        /*获取域名和端口*/
        if(index(url+pos,':') != NULL && index(url+pos,':') < index(url+pos,'/')){
            strncpy(host,url+pos,strchr(url+pos,':')-url-pos);

            memset(temp,0,sizeof(temp));
            /*这里主要要只获得数字部分,否则会有段错误*/
            strncpy(temp,index(url+pos,':')+1,strchr(url+pos,'/')-index(url+pos,':')-1);
            proxy_port = atoi(temp);
            if(proxy_port == 0)
                proxy_port = 80;
        }
        else{
             strncpy(host,url+pos,strchr(url+pos,'/')-url-pos);
//            strncpy(host,url+pos,strcspn(url+pos,"/"));
        }
        /*向request写入请求页,如/index.html */
        strcat(request,strchr(url+pos,'/'));
  //      strcat(request+strlen(request),url+pos+strcspn(url+pos,"/"));
    }
    else{
        strcat(request,url);
    }
    if(http == 1)
        strcat(request," HTTP/1.0");
    else if(http == 2)
        strcat(request," HTTP/1.1");
    strcat(request,"\r\n");
    /*请求消息的请求行，到这里已经填写完成*/

    if(http > 0){
        strcat(request,"User-Agent:webbench " PROGRAM_VERSION "\r\n");
    }
    if(proxyhost == NULL && http > 0){
        strcat(request,"Host: ");
        strcat(request,host);
        strcat(request,"\r\n");
    }
    if(reload && proxyhost == NULL){
        strcat(request,"Pragma: no-cache\r\n");
    }
    if(http > 1){
        strcat(request,"Connection: close\r\n");
    }
    /*消息头填写完毕*/
    if(http > 0)
        strcat(request,"\r\n");
    fprintf(stdout,"\nrequest:\n%s\n",request);
    return;
}
static int bench(){
    pid_t pid = 0;
    FILE *f = NULL;
    int sock;

    struct sigaction act_child;
    act_child.sa_handler = &alarm_handler;
    sigemptyset(&act_child.sa_mask);
    act_child.sa_flags = 0;
    sigaction(SIGCHLD,&act_child,0);
    sock = my_socket(proxyhost == NULL?host:proxyhost,proxy_port);

    if(sock < 0){
        fprintf(stderr,"\n连接server失败.");
        return 1;
    }
    close(sock);

    /*创建管道*/
    if(pipe(pipe_buf) == -1){
        perror("创建管道失败");
        return 3;
    } 
    
    /*创建子进程*/
    for(int i=0;i<clients;++i){
        pid = fork();
        if(pid <= 0){
            sleep(1);
            break;//失败的和子进程(pid == 0)都不再继续fork
        }
    }
    if(pid < 0){
        fprintf(stderr,"子进程创建失败,%d\n",sock);
        perror("创建子进程失败");
        return 3;
    }
    else if(pid == 0){
        /*子进程执行内容*/
        if(proxyhost == NULL){
            benchcore(host,proxy_port,request);
        }
        else{
            benchcore(proxyhost,proxy_port,request);
        }
        /*把结果输入到管道中*/
        f = fdopen(pipe_buf[1],"w");//使用标准IO函数，速度更快
        if(f == NULL){
            perror("打开管道入口失败.");
            return 3;
        }
        fprintf(f,"%d %d %lld\n",speed,failed,byte_counts);//向管道写入记录的信息
        fclose(f);
        return 0;//正常结束
    }
    else{
        f = fdopen(pipe_buf[0],"r");
        if(f == NULL){
            perror("打开管道出口失败.");
            return 3;
        }
        setvbuf(f,NULL,_IONBF,0);

        /*初始化记录变量*/
        speed = 0;
        failed = 0;
        byte_counts = 0;

        while(true){
            /*读取子进程写入的记录信息*/
            int speed_r,failed_r;
            long long int byte_r;
            if(fscanf(f,"%d %d %lld",&speed_r,&failed_r,&byte_r) < 3){
                fprintf(stderr,"某个子进程死亡!\n");
                break;
            } 
            /*统计总量*/
            speed += speed_r;
            failed += failed_r;
            byte_counts += byte_r;
            clients -= 1;

            if(clients == 0)
                break;
        }
        fclose(f);
        fprintf(stdout,"spped = %d\n",speed);
        fprintf(stdout,"速度 = %d 请求数/分, %d 字节数/秒.\n 请求: %d 成功, %d 失败\n",
                (int)((speed+failed)/(bench_time/60.0f)),
                (int)(byte_counts*1.0)/bench_time,speed,failed);
    } 
    return sock; 
}

void benchcore(const char *host,const int port,const char *req){
    int rlen;
    char buf[BUF_SIZE];
    struct sigaction act;
    
    act.sa_handler = &alarm_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if(sigaction(SIGALRM,&act,0) != 0){
        exit(3);
    }
    alarm(bench_time);

    rlen = strlen(req);
    
    while(true){
        bool have_close = false;//用来判断套接字是否已经关闭.
        if(timeout){
            if(failed > 0)
                failed--;
            return;
        }
        int sock =my_socket(host,port);
        if(sock < 0){
            failed++;
            continue;
        }
        if(write(sock,req,rlen) != rlen){
            failed++;
            close(sock);
            continue;
        }
        /*http0.9在受到服务器回复后会自动断开，没有keep-alive
        * 我们可以用shutdown把套接字的输出流关闭，如果关闭失败，那么证明这是一个失败的套接字
        * */
        if(http == 0 && shutdown(sock,SHUT_WR) == -1){
            failed++;
            close(sock);
            continue;
        }
        /*接收服务器的回应.*/
        if(force == false){
            while(true){
                if(timeout)
                    break;
                int read_len = read(sock,buf,BUF_SIZE);

                if(read_len < 0){
                    failed++;
                    close(sock);
                    have_close = true;
                    break;
                }
                else if(read_len == 0){
                    break;
                }
                else{
                    byte_counts += read_len;
                }
            }
        }
        if(have_close == false && close(sock) != 0){
            failed++;
            continue;
        }
        speed++;
    }
}
static void alarm_handler(int signal){
    if(signal == SIGALRM)
        timeout = 1;
    else if(signal == SIGCHLD){
        int status;
        pid_t pid = waitpid(-1,&status,WNOHANG);
    }
}
