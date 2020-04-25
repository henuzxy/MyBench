#include "UserUtils.h"
#include <iostream>
void UserUtils::Usage(){
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