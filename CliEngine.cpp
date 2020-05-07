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
    for(int i=0;i<argc;++i){
        std::cout << argv[i] << std::endl;
    }


    InputInfo info(method,clients,force,reload,proxy_port,proxyhost,bench_time,http);
    return info;
}
}