// ÀÓioctl»ñ¾»úÉ±¸µÄACµØ·

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<netinet/ip.h>
#include<net/if_arp.h>
#include<sys/sysctl.h>

#include <stdio.h>
#include <stdlib.h>
#define MAX_LENTH 1024


int get_macaddr_from_ethx(char* sz_ethx)
{
	if (sz_ethx == NULL || strlen(sz_ethx) == 0)
	{
		printf("Usage :  ethX interface\n");
		return 1;
	}

	struct sockaddr_in *addr;
	struct ifreq ifr;
	unsigned char *mac;
	char *address;
	int sockfd;
	
	//printf("sz_ethx = %s\n", sz_ethx);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(ifr.ifr_name, sz_ethx, IFNAMSIZ - 1);
	if (ioctl(sockfd, SIOCGIFADDR, &ifr) == -1)
	{
		perror("ioctl");
		return -1;
	}
	addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	address = inet_ntoa(addr->sin_addr);
	printf("inet addr: %s\n", address);

	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1)
	{
		perror("ioctl error");
		return -1;
	}
	
	mac = (unsigned char*)(ifr.ifr_hwaddr.sa_data);
	printf("mac address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

int printLine(char *path)
{
    char buffer[MAX_LENTH] = {0};//以一个字符数组为缓存,按行读取内容,每次按一个字符串来处理
    int num = 1;
    FILE *pf = fopen(path,"r");
    if(pf == NULL)
    {
        perror("file not open ");
		return -1;
    }
    while(fgets(buffer,MAX_LENTH, pf)!=NULL)
    {
		//printf("buffer = %s strlen(buffer) = %d\n", buffer, strlen(buffer));
		char sz_ethx[10] = {0};
		int ibuf_len = strlen(buffer);
		memcpy(sz_ethx, buffer, ibuf_len - 1);
				
       // printf("sz_ethx = %s\n", sz_ethx);
		get_macaddr_from_ethx(sz_ethx);
		printf("\n\n");
        num++;
    }


    fclose(pf);
	return 0;
}

int main(int argc, char **argv)
{
	system("./get_ethx.sh");
//	printf("%d: [%s]----\n", strlen(mac), mac);
	printLine("/var/ethx.txt");
	
	return 0;
}
