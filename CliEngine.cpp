#include "CliEngine.h"
#include <iostream>
#include <getopt.h>
#include <string.h>
namespace CLI{
#define PROGRAM_VERSION "v1.0.3"
    
void CliEngine::Usage(){
    static std::string UseInfo =  "webbench [选项参数]...URL\n"
        "-f\t --force\t\t\t不用等待服务器响应\n"
        "-r\t --reload\t\t\t重新请求加载(无缓存)\n"
        "-t\t --time <sec>\t\t\t运行时间，单位：秒，默认为30秒\n"
        "-p\t --proxy <IP:Port>\t\t使用代理服务器发送请求\n"
        "-c\t --clients <num>\t\t创建客户端个数,默认为1\n"
        "-9\t --http09\t\t\t使用http0.9协议\n"
        "-1\t --http10\t\t\t使用http1.0协议\n" 
        "-2\t --http11\t\t\t使用http1.1协议\n"
        "\t --get\t\t\t\tGET请求方式\n"
        "\t --head\t\t\t\tHEAD请求方式\n"
        "\t --options\t\t\tOPTIONS请求方式\n"
        "\t --trace\t\t\tTRACE请求方式\n"
        "-?|-h\t --help\t\t\t\t显示帮助信息\n"
        "-v\t --version\t\t\t显示版本信息\n"
    ;
    std::cout << UseInfo << std::endl;
}

InputInfo CliEngine::GetInput(int argc,char *argv[]){
    
    /*选项相关变量与其默认值*/
    int method = METHOD_GET;
    int clients = 1;
    bool force = false;
    bool reload = false;
    int proxy_port = 80;
    char* proxyhost = NULL;//默认不使用代理
    int bench_time = 30;
    int http = 1;//http协议版本，0:http0.9 1:http1.0 2:http1.1
    cout << "Begin" << endl;
/*长选项数组*/
static const struct option long_options[] = {
    {"force",no_argument,&force,true},
    {"reload",no_argument,&reload,true},
    {"time",required_argument,NULL,'t'},
    {"help",no_argument,NULL,'h'},
    {"http09",no_argument,&http,0},
    {"http10",no_argument,&http,1},
    {"http11",no_argument,&http,2},
    {"get",no_argument,&method,METHOD_GET},
    {"head",no_argument,&method,METHOD_HEAD},
    {"options",no_argument,&method,METHOD_OPTIONS},
    {"trace",no_argument,&method,METHOD_TRACE},
    {"version",no_argument,NULL,'v'},
    {"proxy",required_argument,NULL,'p'},
    {"clients",required_argument,NULL,'c'},
    {NULL,0,NULL,0}
};
    int opt = 0;
    int options_index = 0;
    char *temp = nullptr;
/*短参数选项*/
    const char optstring[] = "frt:h?912vp:c:";

    while((opt = getopt_long(argc,argv,optstring,long_options,&options_index)) != EOF){
        switch(opt){
            case 0:
                break;//参数值被放入对应变量，返回
            case 'f':
                force = true;
                break;
            case 'r':
                reload = true;
                break;
            case 't':
                bench_time = atoi(optarg);
                break;
            case 'h':
            case '?':
                Usage();
                exit(0);//带有该参数，程序会直接结束
            case '9':
                http = 9;break;
            case '1':
                http = 1;break;
            case '2':
                http = 2;break;
            case 'v':
                fprintf(stdout,PROGRAM_VERSION "\n");
                exit(0);
            case 'p':
                /*代理服务器格式 server:port*/
                temp = strrchr(optarg,':');/*返回最后一个':',的位置*/
                proxyhost = optarg;
                if(temp == NULL)    break;
                if(temp == optarg){
                    fprintf(stderr,"Error in option --proxy %s:缺少主机名",optarg);
                    exit(1);
                }
                if(temp == optarg+strlen(optarg)-1){
                    fprintf(stderr,"Error in option --proxy %s:缺少端口号",optarg);
                    exit(1);
                }
                *temp = '\0';
                proxy_port = atoi(temp+1);
                break;
            case 'c':
                clients = atoi(optarg);
                break;
        }
        if(optind == argc){
        fprintf(stderr,"webbench:没有链接URL\n");
            Usage();
            exit(0);
        }
        /*考虑用户输入错误的情况,使用默认值*/
        if(clients <= 0)
            clients = 1;
        if(bench_time <= 0)
            bench_time = 30;
        }
        //测试getopt函数
    fprintf(stdout,"force = %d,reload = %d,bench_time = %d,http = %d,proxyhost = %s,proxy_port = %d,clients = %d\n",force,reload,bench_time,http,proxyhost,proxy_port,clients);

    InputInfo info(method,clients,force,reload,proxy_port,proxyhost,bench_time,http);
    return info;
}
}