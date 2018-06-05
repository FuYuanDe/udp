/*
 *  Description : linux udp套接字获取报文源地址和源端口(二)
 *  Date        ：20180605
 *  Author      ：mason
 *  Mail        : mrsonko@126.com
 *
 */
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/uio.h> 
#include <stdint.h>  
#include <stdlib.h>
#include <unistd.h>  
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 512
#define log(fmt, arg...) printf("[udptest] %s:%d "fmt, __FUNCTION__, __LINE__, ##arg)

#ifndef NIPQUAD
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]
#endif

void main() {
    int on = 1;
    int sockfd;
    int recv_len, ret, addr_len;
    char buffer[512] = {0};
    struct sockaddr_in saddr;
    struct sockaddr_in local_addr;

    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        log("create socket fail \r\n");
        return ;
    }    

    // 设置监听地址
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY; 
    local_addr.sin_port = htons(3500);
    addr_len = sizeof(struct sockaddr_in);

    // 设置SO_REUSEADDR属性, 地址复用    
    if ((ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) != 0) {
        log("setsockopt reuseaddr fail, ret : %d,error : %d \r\n", ret, errno);
        close(sockfd);
        return ;
    }

    // 绑定本地监听地址
    if (0 != bind(sockfd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in))) {
        printf("bind local listening addr fail，errno : %d \r\n", errno);
        close(sockfd);
        return ;
    }   

    log("Start receiving message: \n");
    while(1) {
        //接收数据
        recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&saddr, &addr_len);
        if(recv_len != -1)
            log("receive msg from : %u:%u:%u:%u:%hu，msg : %s \r\n",
            NIPQUAD(saddr.sin_addr), ntohs(saddr.sin_port), buffer);

        //重置
        memset(buffer, 0, BUFFER_SIZE); 
        memset(&saddr, 0, addr_len);
    }
}