/*http请求方法的相关宏定义*/
constexpr int METHOD_GET = 0;
constexpr int METHOD_HEAD = 1;
constexpr int METHOD_OPTIONS = 2;
constexpr int METHOD_TRACE  = 3;

class InputInfo{
public:
    /*选项相关变量与其默认值*/
    int method;
    int clients;
    bool force;
    bool reload;
    int proxy_port;
    char* proxyhost;//默认不使用代理
    int bench_time ;
    int http;//http协议版本，0:http0.9 1:http1.0 2:http1.1
    InputInfo():method(METHOD_GET),clients(1),force(false),reload(false),proxy_port(80),proxyhost(nullptr),bench_time(30),http(1){
    }
    InputInfo(int _m,int _c,bool _f,bool _r,int _p,char * _proxy,int _bt,int _h):method(_m),clients(_c),force(_f),reload(_r),proxy_port(_p),proxyhost(_proxy),bench_time(_bt),http(_h){
        
    }
};