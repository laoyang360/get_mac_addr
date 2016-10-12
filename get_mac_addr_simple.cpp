//show mac addr list
//20161009 pm16:28 ·

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
#include <iostream>
#include <string>
using namespace std;

#define MAX_LENTH 1024

//虚拟或者无效地址列表
const char* g_Virtual_Mac[] =
{
	"00:05:69", //vmware1
	"00:0C:29", //vmware2
	"00:50:56", //vmware3
	"00:1C:42", //parallels1
	"00:03:FF", //microsoft virtual pc
	"00:0F:4B", //virtual iron 4
	"00:16:3E", //red hat xen , oracle vm , xen source, novell xen
	"08:00:27"  //virtualbox
};


/**
**@breif: 判定是否为虚拟网卡地址，是会有打印输出。
**@param: [1]pszMacAddr:mac地址; [2]size:mac地址长度，预留。
**@return: 空
**/
void IsVirtualNetAddr(const char* pszMacAddr, int  size)
{

	string strMacAddr(pszMacAddr);
	string strSubMac = strMacAddr.substr(0,8);
	//cout << "strSubMac = " << strSubMac.c_str() << endl;

	for (int i = 0; i < sizeof(g_Virtual_Mac)/sizeof(char*); ++i)
	{
		if (0 == strcmp(strSubMac.c_str(), g_Virtual_Mac[i]))
		{
			printf("\n %s  is Virtual network card address! \n", pszMacAddr);
			break;
		}
	}
}

/**
**@breif: 获取对应接口的mac地址，如eth0,eth1,ethX
**@param: [1]sz_ethx, ethX的名称。
**@return: >0,成功; <=0 失败。
**/
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
	printf("Ip:%s\t", address);

	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1)
	{
		perror("ioctl error");
		return -1;
	}
	
	mac = (unsigned char*)(ifr.ifr_hwaddr.sa_data);
	printf("MacAddr:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	
	char szMacAddr[1024] = {0};
	sprintf(szMacAddr, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	(void)IsVirtualNetAddr(szMacAddr, 1 + strlen(szMacAddr));
}

/**
**@breif: 获取对应网口名称，如：eth0,eth1,ethX。
**@param: [1][out]name：获取的名称; [2]p：传入的buffer
**@return: >=0,成功; <0 失败。
**/
char * get_name(char *name, char *p) 
{ 
    while (isspace(*p)) 
    p++; 
    while (*p) { 
    if (isspace(*p)) 
        break; 
    if (*p == ':') {    /* could be an alias */ 
        char *dot = p, *dotname = name; 
        *name++ = *p++; 
        while (isdigit(*p)) 
        *name++ = *p++; 
        if (*p != ':') {    /* it wasn't, backup */ 
        p = dot; 
        name = dotname; 
        } 
        if (*p == '\0') 
        return NULL; 
        p++; 
        break; 
    } 
    *name++ = *p++; 
    } 
    *name++ = '\0'; 
    return p; 
} 
 
/*
// get /proc/net/dev interface name list into buffer 
// return 0 if success 
[root@xx get_mac_addr-master]# cat /proc/net/dev
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo: 1820775   28013    0    0    0     0          0         0  1820775   28013    0    0    0     0       0          0
  eth0: 376931443116 4170807778    0    0    0     0          0   3496038 50952894925944 34269685429    0    0    0     0       0          0
  eth1: 6554025120 109233564    0    0    0     0          0         0 24852886  123117    0    0    0     0       0          0
*/
/**
**@breif: 获取/proc/net/dev的接口的列表信息（第1列）
**@param: 空
**@return: =0,成功; <0 失败。
**/
int get_procnet_list() 
{ 
    FILE *fh; 
    char buf[512] = {0}; 
    fh = fopen("/proc/net/dev", "r"); 
    if (!fh) 
	{
        return -1; 
	}
    fgets(buf, sizeof buf, fh); /* eat title lines */
	//printf("buf = %s\n", buf);
    fgets(buf, sizeof buf, fh);
	//printf("buf = %s\n", buf);
	int icnt = 1;
    while (fgets(buf, sizeof buf, fh)) 
    { 
        char name[IFNAMSIZ] = {0}; 
        get_name(name, buf); 
		if (0 == strcmp(name, "lo"))
		{
			continue;
		}
		printf("%d  interface:%s\t", icnt++, name);
		get_macaddr_from_ethx(name);
        //add_interface_name(name); 
    } 
    fclose(fh); 
    return 0; 
} 


/*
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
				
        printf("sz_ethx = %s\n", sz_ethx);
		get_macaddr_from_ethx(sz_ethx);
		printf("\n\n");
        num++;
    }


    fclose(pf);
	return 0;
}*/

int main(int argc, char **argv)
{
//	system("./get_ethx.sh");
//	printf("%d: [%s]----\n", strlen(mac), mac);
//	printLine((char*)("/var/ethx.txt"));
	(void)get_procnet_list();
	
	return 0;
}
