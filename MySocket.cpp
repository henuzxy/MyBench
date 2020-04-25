/*************************************************************************
	> File Name: socket.c
	> Author:henuzxy 
	> Mail: 
	> Created Time: 2019年07月24日 星期三 14时20分04秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<sys/time.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdarg.h>
int my_socket(const char *host,int client_port){
    int sock;
    struct sockaddr_in serv_addr;
    struct hostent *hp = NULL;
    
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    /*这里host可能是IP,也可能是域名*/
    if(inet_addr(host) != INADDR_NONE)
        serv_addr.sin_addr.s_addr = inet_addr(host);
    else{
        hp = gethostbyname(host);
        if(hp == NULL)
            return -1;
        memcpy(&serv_addr.sin_addr,hp->h_addr,hp->h_length);
    }
    serv_addr.sin_port = htons(client_port);

    sock = socket(AF_INET,SOCK_STREAM,0);
   // fprintf(stdout,"sock = %d\n",sock);
    if(sock < 0)
        return sock;
    if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)
        return -1;
    return sock;
}
