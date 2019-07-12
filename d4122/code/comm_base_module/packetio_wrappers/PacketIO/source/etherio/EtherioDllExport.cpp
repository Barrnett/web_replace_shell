#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <net/if.h> 
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <sys/stat.h> 
#include <stddef.h>  
#include <net/if.h>
#include "pfring.h"
#include "EtherioDllExport.h"
#include "DeviceAccess.h"
#include "NetUtilities.h"

#include <iostream>
using namespace::std;

#define ETH_NAME_LENGTH 128
#define ETH_NUM		32
#define ETH_KERN_NUM_MAX  64
#define LINE_LENGTH	512

#define DEV_LIST_MASK  0x00000001

#define ETHTOOL_GSET    0x00000001 /* Get settings. */  


typedef struct ether_handle
{
	pfring* p_fd;
	BOOL bWaitPacketEvent;
}handle,*p_handle;

typedef struct eth_info
{
	char eth_name[ETH_NAME_LENGTH];
	UCHAR mac[6];
	int index;
	int kern_index;
}ethinfo;


struct ethtool_cmd {  
        __u32   cmd;  
        __u32   supported;      /* Features this interface supports */  
        __u32   advertising;    /* Features this interface advertises */  
        __u16   speed;          /* The forced speed, 10Mb, 100Mb, gigabit */  
        __u8    duplex;         /* Duplex, half or full */  
        __u8    port;           /* Which connector port */  
        __u8    phy_address;  
        __u8    transceiver;    /* Which transceiver to use */  
        __u8    autoneg;        /* Enable or disable autonegotiation */  
        __u32   maxtxpkt;       /* Tx pkts before generating tx int */  
        __u32   maxrxpkt;       /* Rx pkts before generating rx int */  
        __u32   reserved[4];  
};

enum if_item  
{  
    RX_BYTES = 0,  
    RX_PACKETS,  
    RX_ERRS,  
    RX_DROP,  
    RX_FIFO,  
    RX_FRAME,  
    RX_COMPRESSED,  
    RX_MULTICAST,  
    TX_BYTES,  
    TX_PACKETS,  
    TX_ERRS,  
    TX_DROP,  
    TX_FIFO,  
    TX_COLLS,  
    TX_CARRIER,  
    TX_COMPRESSED,  
    IF_ITEM_MAX  
};



ethinfo local_ethinfo[ETH_NUM];
int kern_index_array[ETH_KERN_NUM_MAX] = {0};
int count = 0;

ETHERIO_API  int EtherIO_InitializeEx2_(IN const char* pszFileName, IN const char* pszSrvName, IN int iType, BOOL bStaticLoad)
{
	(void)pszFileName;
	(void)pszSrvName;
	(void)bStaticLoad;

	ASSERT(pszFileName && pszSrvName);

	CDeviceAccess::s_iOpenInstance ++;

	if( CDeviceAccess::s_iOpenInstance < 1 )
	{
		ASSERT(FALSE);

		CDeviceAccess::s_iOpenInstance = 1;
	}

	if (1 == CDeviceAccess::s_iOpenInstance)
	{
		// once is enough
		int count = CDeviceAccess::s_Initialize(pszFileName, pszSrvName, iType);
		strcpy(CDeviceAccess::s_strSrvName, pszSrvName);

		return count;
	}
	else
	{
		if (0 != strcmp(pszSrvName, CDeviceAccess::s_strSrvName))
		{
			return EN_ERR_SERVICE_NAME;
		}

		return CDeviceAccess::s_vetDevice.size();
	}
}

ETHERIO_API int EtherIO_Initialize(BOOL bStaticLoad )
{
	return EtherIO_InitializeEx2(SYS_FILE_NAME, SYS_SERVICE_NAME, EN_ACCESS_DATA, bStaticLoad);
}

ETHERIO_API int EtherIO_InitializeEx2(
									  IN const char* pszFileName, 
									  IN const char* pszSrvName,
									  IN int iType,
									  IN BOOL bStaticLoad)

{
	(void)pszFileName;
	(void)pszSrvName;
	(void)bStaticLoad;

	
	if (count == 0)
	{

		char * p_find; //usrd for find ':'
		struct ifreq ifr;

		FILE* f = fopen("/proc/net/dev", "r");

		if(!f)
		{
			fprintf(stderr, "Open /proc/net/dev failed!errno:%d\n", errno);
			return count;
		}

		int s = socket(AF_INET, SOCK_DGRAM, 0);//create socket for get eth's mac info 

		if(s<0)
		{
			fprintf(stderr, "socket create  failed!errno:%d\n", errno);
			return count;
		}

		char szLine[LINE_LENGTH];
		fgets(szLine, sizeof(szLine), f);
		fgets(szLine, sizeof(szLine), f);

		while(fgets(szLine, sizeof(szLine), f))
		{
			char szName[ETH_NAME_LENGTH] = {0};
			sscanf(szLine, "%s", szName);
			int nLen = strlen(szName);

			if (nLen <= 0)
			{
				continue;
			}
			if ((p_find = strchr(szName,	':') )!= NULL)
			{
				*p_find = 0;
			}

			if (strcmp(szName, "lo") == 0)
			{
				continue;
			}


			strcpy(ifr.ifr_name, szName);

			if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0)
			{
				continue;
			}

			memcpy(local_ethinfo[count].mac,ifr.ifr_hwaddr.sa_data,6);
			strncpy(local_ethinfo[count].eth_name,szName,nLen);
			local_ethinfo[count].index = count;
			local_ethinfo[count].kern_index = if_nametoindex(local_ethinfo[count].eth_name);
			kern_index_array[local_ethinfo[count].kern_index] = local_ethinfo[count].index;
			count++;
		}

/*
		for (int i=0; i<count; ++i)
		{
			printf("%d: \t %s \t %d[%d] \t %08X%02X\n", i, local_ethinfo[i].eth_name, local_ethinfo[i].index, local_ethinfo[i].kern_index,
				*(unsigned int*)&local_ethinfo[i].mac[0], *(unsigned short*)&local_ethinfo[i].mac[4]);
		}
		printf("\n");
*/
		fclose(f); 
		close(s);
		f = NULL;
	}

	return EtherIO_InitializeEx2_(SYS_FILE_NAME, SYS_SERVICE_NAME, EN_ACCESS_DATA, bStaticLoad);
	
	return 1;
}



ETHERIO_API int EtherIO_InitializeEx(IN const char* pszFileName,IN const char* pszSrvName, BOOL bStaticLoad)
{
	(void)pszFileName;
	(void)pszSrvName;
	return EtherIO_Initialize(bStaticLoad);
}
ETHERIO_API void EtherIO_UnInitialize()
{
	CDeviceAccess::s_iOpenInstance --;

	if (CDeviceAccess::s_iOpenInstance == 0)
	{
		CDeviceAccess::s_Uninitialize(CDeviceAccess::s_strSrvName);
	}
	else if( CDeviceAccess::s_iOpenInstance < 0 )
	{
		ASSERT(false);

		//防止反复调用这个函数
		CDeviceAccess::s_iOpenInstance = 0;
	}
	return;
}

ETHERIO_API int EtherIO_GetDeviceCount()
{
	return count;
}


ETHERIO_API const char* EtherIO_GetDeviceName(int iIndex)
{
	if((iIndex < 0 )|| (iIndex > 31))
	{
		return NULL;
	}

	return local_ethinfo[iIndex].eth_name;
}

//(Intel(R) PRO/1000 PL Network Connection)
ETHERIO_API const char* EtherIO_GetDevDesc(int iIndex)
{
	ASSERT(iIndex < CDeviceAccess::s_vetDevice.size());
	if(iIndex >= (int)CDeviceAccess::s_vetDevice.size())
		return NULL;

	return CDeviceAccess::s_vetDevice[iIndex]->GetDevDesc();
}
#ifdef OS_LINUX
ETHERIO_API int EtherIO_GetDeviceKernIndex(int iIndex)
{
	if((iIndex < 0 )|| (iIndex > 31))
	{
		return -1;
	}

	if(local_ethinfo[iIndex].eth_name[0] == '\0')
	{
		return -1;
	}
	
	return local_ethinfo[iIndex].kern_index;

}
#endif
//
// 取设备MAC地址，输入缓冲区要不小于6个字节
//
ETHERIO_API void EtherIO_GetDeviceMac(int iIndex, OUT UCHAR* pMacAddr)
{
	if((iIndex < 0 )|| (iIndex > 31))
	{
		return;
	}
	
	memcpy(pMacAddr,local_ethinfo[iIndex].mac,6);
	
	return;
}

//得到名字"本地连接|本地连接1"
ETHERIO_API void EtherIO_GetShowNameByMultiIndex(int index_array, OUT char * text)
{
	text[0] = 0;

	if( index_array != -1 )
	{		
		for( int i= 0; i < 32; ++i)
		{
			if( index_array & (1<<i) )
			{
				const char * showname = EtherIO_GetDeviceName(i);

				if( text[0] == 0 )
				{
					strcpy(text,showname);
				}
				else
				{
					strcat(text,"|");
					strcat(text,showname);
				}
			}
		}
	}

	return;
}




//
// 通过MAC地址获得板卡的index
//		pszMacAddr - MAC地址字符串，可以是0a0b0c0d0e0f形式，或者0a:0b:0c...，
//			中间以任意字符间隔，不区分大小写
//		return: -1 - input error, else - index
//
ETHERIO_API int EtherIO_GetOneIndexByMAC(IN const char* pszMacAddr)
{
	ASSERT(pszMacAddr);
	if (NULL == pszMacAddr)
		return -1;

	// 判断输入
	size_t iLen = strlen(pszMacAddr);
	if (EN_MAC_FULL_LEN!=iLen && EN_MAC_SELF_LEN!=iLen)
		return -1;

	char macSrc[32];
	char macTmp[32];
	char* pszTemp = (char*)macTmp;
	char chSep;

	// 源转换大写
	strcpy(macSrc, pszMacAddr);
	strupr(macSrc);
	for(int i=0;i<ETH_NUM;i++)
	{
		if (EN_MAC_SELF_LEN == iLen)
		{
			CDeviceAccess::GetMacAddrString(local_ethinfo->mac, pszTemp, NULL);
		}
		else
		{
			chSep = pszMacAddr[2];
			CDeviceAccess::GetMacAddrString(local_ethinfo->mac, pszTemp, &chSep);
		}

		if (0 == strcmp(macSrc, macTmp))
		{
			return i;
		}
	}

	return -1;
}

ETHERIO_API int EtherIO_GetMultiIndexByShowNameOrMAC(
	IN const char* card_info)
{	
	int dwIndexLst = EtherIO_GetMultiIndexByShowName(card_info);

	if( dwIndexLst != 0 )
		return dwIndexLst;

	dwIndexLst = EtherIO_GetMultiIndexByMAC(card_info);

	if( dwIndexLst != 0 )
		return dwIndexLst;

	return 0;
}
//传入名字"本地连接|本地连接1",如果输入不合法或者没有找到，返回0
ETHERIO_API int EtherIO_GetMultiIndexByShowName(IN const char *pszShowName)
{
	ASSERT(pszShowName);
	if (NULL == pszShowName || pszShowName[0] == 0 )
		return 0;

	DWORD dwIndexLst = 0;	// 合成后的index列表

	char szCopyName[MAX_PATH] = {0};

	strncpy(szCopyName,pszShowName,sizeof(szCopyName)-1);
	szCopyName[sizeof(szCopyName)-1] = 0;
	char * pcur = szCopyName;

	while( pcur && pcur[0] )
	{
		char * pnext = strchr(pcur,'|');

		if( pnext )
		{
			*pnext = 0;
			++pnext;
		}

		if( pnext - pcur == 1 )
		{
			pcur = pnext;		

			continue;
		}

		int iIndex = EtherIO_GetOneIndexByShowName(pcur);

		if (iIndex >= 0)
		{
			dwIndexLst |= (1 << iIndex);
		}
		else
		{
			return 0;
		}

		pcur = pnext;		
	}

	return dwIndexLst;
}

ETHERIO_API int EtherIO_GetMultiIndexByMAC(IN const char* pszMacLst)
{
	// 输入字符串分割
	if (NULL == pszMacLst)
		return 0;

	DWORD dwIndexLst = 0;	// 合成后的index列表
	int iIndex = 0;
	char sep = ':';
	const char* pTemp = pszMacLst;

	char macTemp[32];

	// 因为要求必须带分隔符，所以一个完整的MAC地址长度应改为17个字符
	while(strlen(pTemp) >= EN_MAC_FULL_LEN)	// >= EN_MAC_FULL_LEN)
	{
		// 通过分隔符来判断一个MAC地址的起始位置
		sep = pTemp[2];
		if ('-'!=sep && ':'!= sep)
		{
			pTemp ++;
			continue;
		}

		// 找到一个MAC的起始位置,通过这个MAC字符串查找index
		strncpy(macTemp, pTemp, EN_MAC_FULL_LEN);
		macTemp[EN_MAC_FULL_LEN] = '\0';

		iIndex = EtherIO_GetOneIndexByMAC(macTemp);
		if (iIndex >= 0)
		{
			dwIndexLst |= (1 << iIndex);
		}
		// 如果有输入的MAC地址错误，则直接返回
		else
		{
			return 0;
		}

		// 跳过这个字符串
		pTemp += EN_MAC_FULL_LEN;
	}

	return dwIndexLst;

}




//,如果输入不合法或者没有找到，返回-1
ETHERIO_API int EtherIO_GetOneIndexByShowName(IN const char *pszShowName)
{
	if(NULL == pszShowName)
	{
		return -1;
	}

	for(int i=0;i<ETH_NUM;i++)
	{
		if(strcmp(pszShowName,local_ethinfo[i].eth_name) == 0)
		{
			return i;
		}
		else
		{
			continue;
		}
	}

	return -1;
}
//
// 取设备速率，单位是kbps，0 或 -1无效
//
ETHERIO_API int EtherIO_GetDeviceLinkSpeed(int iIndex)
{
	if((iIndex < 0 )|| (iIndex > 31))
	{
		return -1;
	}

	int s = socket(AF_INET,SOCK_DGRAM,0);

	if (s < 0) {  
		fprintf(stderr,"Open control socket failed! error no:%d\n",errno);
		return -1;  
	}

	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));  
	strcpy(ifr.ifr_name, local_ethinfo[iIndex].eth_name);	

	struct ethtool_cmd ep;
	ep.cmd = ETHTOOL_GSET;//ethtool  get setting
	ifr.ifr_data = (caddr_t)&ep;

	int err = ioctl(s, SIOCETHTOOL, &ifr);  

	
	if (err != 0) 
	{ 
		printf(" ioctl is erro .\n");  
		return -1;	
	}

	return ep.speed;
		
}

//
// 取设备的接收统计
//
ETHERIO_API bool EtherIO_GetDeviceStss(int iIndex, OUT DRIVER_STSS_INFO* pStss)
{
	if((iIndex < 0 )|| (iIndex > 31))
	{
		return false;
	}

	if(NULL == pStss)
	{
		fprintf(stderr,"the input pointer is invalid!\n");
	}

	char * p_find;
	FILE* fd = fopen("/proc/net/dev", "r");//打开系统文件查看网卡接口  
	
	if(!fd)
	{
		fprintf(stderr, "Open /proc/net/dev failed!errno:%d\n", errno);
		return false;
	}

	char szLine[LINE_LENGTH];
	fgets(szLine, sizeof(szLine), fd);
	fgets(szLine, sizeof(szLine), fd);

	while(fgets(szLine, sizeof(szLine), fd))
	{
		char szName[ETH_NAME_LENGTH] = {0};
		sscanf(szLine, "%s", szName);
		int nLen = strlen(szName);

		string infos = szLine;
		int p1 = infos.find_first_of(":",0);

		if (nLen <= 0)
		{
			continue;
		}
		if ((p_find = strchr(szName,	':') ) != NULL)
		{
			*p_find = 0;
		}

		if (strcmp(szName, "lo") == 0)
		{
			continue;
		}

		if(strcmp(szName,local_ethinfo[iIndex].eth_name) != 0)
		{
			continue;
		}
		else
		{
		
			unsigned long long data[32] = {0};  

			sscanf(szLine+p1+1, "%llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",   
       			data + RX_BYTES,  
       			data + RX_PACKETS,  
       			data + RX_ERRS,  
       			data + RX_DROP,  
       			data + RX_FIFO,  
       			data + RX_FRAME,  
       			data + RX_COMPRESSED,  
       			data + RX_MULTICAST,  
         
       			data + TX_BYTES,  
       			data + TX_PACKETS,  
       			data + TX_ERRS,  
       			data + TX_DROP,  
       			data + TX_FIFO,  
       			data + TX_COLLS,  
       			data + TX_CARRIER,  
       			data + TX_COMPRESSED);  

			pStss->recvFrames = data[RX_PACKETS];
			pStss->recvBytes = data[RX_BYTES];
			pStss->lostFrames = data[RX_DROP];
			
			fclose(fd);
			return true;
		}
		


	}

	fclose(fd);
	return false;
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					接收部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//typedef UINT64 ETHERIO_RECV_HANDLE;

//
// 创建接收对象，
//		dwDevLst - 以位表示的成员列表，
//		dwBufLen - 指定接收缓冲区大小，以字节为单位
//		输出为接收对象句柄，0为失败
//
ETHERIO_API ETHERIO_RECV_HANDLE EtherIO_CreateReceiveObj(DWORD dwDevLst, DWORD dwBufLen,BOOL bWaitPacketEvent/*=TRUE*/,USHORT watermark /*= 1*/)//param dwBufLen not be used
{
	if(dwDevLst == 0)
	{
		return 0;//error
	}
	
	int index = 0;
	DWORD devlist = dwDevLst;
	DWORD tmp = 0;
	char dev_list[1024] = {0}; //use for pfring_open, as the first parameter of pfring_open
	p_handle create_handle = NULL;
	create_handle = (p_handle)malloc(sizeof(handle));

	if(NULL == create_handle)
	{
		fprintf(stderr, "malloc failed!\n");
		return NULL;
	}
	create_handle->bWaitPacketEvent = bWaitPacketEvent;
		
	for(index=0;index<32;index++)
	{
		if( (tmp = devlist & DEV_LIST_MASK) == 0 )
		{
			devlist = devlist >> 1;
			continue;
		}
		else
		{
			devlist = devlist >> 1;
			if(strlen(dev_list) == 0)
			{
				strcat(dev_list,local_ethinfo[index].eth_name);
			}
			else
			{
				strcat(dev_list,",");
				strcat(dev_list,local_ethinfo[index].eth_name);
			}
		}
	}

	pfring  *pd = NULL;
	UINT32 snaplen = 1600;
	UINT32 flags = 0;
	flags |= PF_RING_LONG_HEADER;
  	flags |= PF_RING_PROMISC;

	pd = pfring_open(dev_list, snaplen, flags);

	pfring_set_poll_watermark(pd,watermark);
	pfring_set_poll_duration(pd,5);

	if(pd == NULL)
	{
		free(create_handle);
		return 0;//error
	}

	packet_direction direction = rx_only_direction;
	int rc = pfring_set_direction(pd, direction);
	if(rc != 0)
	{
    	fprintf(stderr, "pfring_set_direction returned [rc=%d]\n", rc);
	}

	if((rc = pfring_set_socket_mode(pd, recv_only_mode)) != 0)
	{
		fprintf(stderr, "pfring_set_socket_mode returned [rc=%d]\n", rc);
	}

	pfring_enable_rss_rehash(pd);
	create_handle->p_fd = pd;

	return (ETHERIO_RECV_HANDLE)create_handle;


	
}


// 删除接收对象，输入为上边创建的接收对象句柄
//
ETHERIO_API void EtherIO_ReleaseReceiveObj(IN ETHERIO_RECV_HANDLE recvHandle)
{
	p_handle p_rcv_handle = (p_handle)recvHandle;

	if(p_rcv_handle == NULL)
	{
		fprintf(stderr, "invalid handle\n");
	}

	pfring_close(p_rcv_handle->p_fd);
	p_rcv_handle->p_fd = NULL;
	free(p_rcv_handle);
}


ETHERIO_API int EtherIO_WaitForRecvPacket(IN ETHERIO_RECV_HANDLE recvHandle,DWORD dwms)
{
	ASSERT(NULL != recvHandle);

	sched_yield();

	return 0;
}


//
// 启动接收，输入为上边创建的接收对象句柄
//
ETHERIO_API bool EtherIO_StartReceive(
	IN ETHERIO_RECV_HANDLE recvHandle,
	IN bool bEnableTimePkt,
	IN EN_TIME_MODE enTimeMode)
{
	p_handle p_rcv_handle = (p_handle)recvHandle;

	if(p_rcv_handle == NULL)
	{
		fprintf(stderr, "invalid handle\n");
	}
	
	pfring_enable_time_pkt(p_rcv_handle->p_fd,bEnableTimePkt);
	
	en_pf_time_mode time_mode;
	if(enTimeMode == EN_TIME_STAMP_MODE_NONE)
		time_mode = en_time_stamp_mode_none;
	else if(enTimeMode == EN_TIME_STAMP_MODE_HIGH)
		time_mode = en_time_stamp_mode_high;
	else if(enTimeMode == EN_TIME_STAMP_MODE_HIGH_NO)
		time_mode = en_time_stamp_mode_high_no;
	else 
		time_mode = en_time_stamp_mode_high_no;
	pfring_set_time_mode(p_rcv_handle->p_fd,time_mode);
	
	if (pfring_enable_ring(p_rcv_handle->p_fd) != 0) 
	{
    	printf("Unable to enable ring !\n");
//   	pfring_close(pd);
//		m_pd = NULL;

		return false;
  	}

	return true;
}

// 
// 停止接收，输入为上边创建的接收对象句柄
//
ETHERIO_API bool EtherIO_StopReceive(IN ETHERIO_RECV_HANDLE recvHandle)
{
	p_handle p_rcv_handle = (p_handle)recvHandle;

	if(p_rcv_handle == NULL)
	{
		fprintf(stderr, "invalid handle\n");
	}

	if(pfring_disable_ring(p_rcv_handle->p_fd) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}

//////////////////////////////////////////////////////////////////////////
//
//  2种取数据的方式，不可以混用
//		EtherIO_GetPacket和EtherIO_SkipPacket成对使用
//		EtherIO_GetNextPacket单独使用
//
//////////////////////////////////////////////////////////////////////////

//
// 取数据
//		recvHandle - 为上边创建的接收对象句柄
//		OUT pPacket - 返回这个数据包的地址
//		OUT stamp - 返回这个数据包的时间辍
//		OUT dwAttrib - 返回这个包的属性
//		OUT dwOriginalLen - 包的原始长度
// 
//	 返回值
//		>0 - 表示有数据包，返回值表示数据包的长度
//		=0 - 当前无包
//		<0 - 描述错误状态，具体值参考<CommonQueueManager.h>文件中PACKET_TYPE_XX的定义
//
ETHERIO_API int EtherIO_GetPacket(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT PUCHAR *pPacket, 
		OUT EIO_TIME *stamp,
		OUT DWORD *card_index, 
		OUT DWORD *dwOriginalLen)
{
	p_handle p_rcv_handle = (p_handle)recvHandle;

	if(p_rcv_handle == NULL)
	{
		fprintf(stderr, "invalid handle\n");
	}
	u_int8_t wait_for_packet = p_rcv_handle->bWaitPacketEvent;
	int rc = 0;
	struct pfring_pkthdr hdr;
	memset(&hdr, 0, sizeof(hdr));
	
		
	u_char* buffer_p = NULL;
	if((rc = pfring_recv(p_rcv_handle->p_fd, &buffer_p, 0, &hdr, wait_for_packet)) > 0) 
	{	

		*pPacket = buffer_p + hdr.extended_hdr.parsed_header_len;
		stamp->sec= hdr.ts.tv_sec;
		stamp->ns = hdr.ts.tv_usec*1000;	//+hdr.extended_hdr.timestamp_ns;
		*card_index = kern_index_array[hdr.extended_hdr.if_index];
		*dwOriginalLen = hdr.len + 4;	// CRC的长度
		// 这里下边其实只补充了2个字节（Magic Number），但是为了和windows版本兼容，我们要返回带有CRC的长度，需要上边的app保证不使用最后2个byte
		
		if (-2 == hdr.extended_hdr.if_index)
		{
			// 定时包
			return -2;
		}


		static const u_int16_t u16_magic_value = RING_MAGIC_VALUE << 8 | RING_MAGIC_VALUE;
		u_int16_t* tail_padding = (u_int16_t*)(*pPacket + hdr.caplen);
		if( u16_magic_value == *tail_padding )//末尾两字节填充0
		{
			*tail_padding = 0;
		}

		return hdr.caplen+4;
					
	} else if(rc == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	} 

	
}


//
// 跳过当前数据包
// 
ETHERIO_API void EtherIO_SkipPacket(IN ETHERIO_RECV_HANDLE recvHandle)
{
	(void)recvHandle;
	return;	
}

// 
// 取数据，使用这个函数取数据的时候，不需要skip操作
//	
ETHERIO_API int EtherIO_GetNextPacket(
									  IN ETHERIO_RECV_HANDLE recvHandle, 
									  OUT PUCHAR *pPacket, 
									  OUT EIO_TIME *stamp,
									  OUT DWORD *card_index, 
									  OUT DWORD *dwOriginalLen)
{
	ASSERT(NULL != recvHandle);
	static int _iLastPktLen = 0;	// 记录上一次的包长度，以便skip

	if (_iLastPktLen > 0)
	{
		EtherIO_SkipPacket(recvHandle);
		_iLastPktLen = 0;
	}

	_iLastPktLen = EtherIO_GetPacket(recvHandle, pPacket, stamp, card_index, dwOriginalLen);

	return _iLastPktLen;
}



//
// 取接收对象的接收统计
//
ETHERIO_API bool EtherIO_GetRecvStss(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT DRIVER_STSS_INFO* pStss)
{
	p_handle p_rcv_handle = (p_handle)recvHandle;

	if(NULL == p_rcv_handle)
	{
		fprintf(stderr, "invalid handle\n");
	}

	if(NULL == pStss)
	{
		fprintf(stderr,"the input pointer is invalid!\n");
	}

	pfring_stat pfringStat;
	if(pfring_stats(p_rcv_handle->p_fd, &pfringStat) >= 0)
	{
		pStss->recvFrames = pfringStat.recv;
		pStss->lostFrames = pfringStat.drop;
		return true;
	}

	return false;

}

//
// 取接收缓冲区的使用字节数
//
//		recvHandle - 为上边创建的接收对象句柄
//		OUT pTotalLen - 返回这个缓冲区的总长度
// 
//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）
ETHERIO_API DWORD EtherIO_GetRecvBufferUsed(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT DWORD* pTotalLen)
{
	p_handle p_rcv_handle = (p_handle)recvHandle;

	if(p_rcv_handle == NULL)
	{
		fprintf(stderr, "invalid handle\n");
	}

	u_int32_t insert_off =  p_rcv_handle->p_fd->slots_info->insert_off;
	u_int32_t remove_off = p_rcv_handle->p_fd->slots_info->remove_off;
	pfring* ring = p_rcv_handle->p_fd;
	if(pTotalLen)
		*pTotalLen = ring->slots_info->tot_mem - sizeof(FlowSlotInfo);
	if(insert_off>=remove_off)
	{
		return (insert_off-remove_off);
	}
	else
	{
		return (ring->slots_info->tot_mem - sizeof(FlowSlotInfo)-(remove_off-insert_off));
	}
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					发送部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//
// 创建发送对象，
//		iIndex - 要发送的设备索引，
//		dwBufLen - 指定接收缓冲区大小，以字节为单位
//		输出为发送对象句柄，0为失败
//
ETHERIO_API ETHERIO_SEND_HANDLE EtherIO_CreateSendObj(
		SEND_CREATE_PARA*  pSendPara)
{
	ASSERT(pSendPara);
	ASSERT(pSendPara->dwCardIndex < CDeviceAccess::s_vetDevice.size());

	if ( pSendPara->dwCardIndex >= CDeviceAccess::s_vetDevice.size() )
		return NULL;

	// 参数检查
	switch(pSendPara->enSendMode)
	{
	case EN_SENDMODE_NONE:	return NULL;
	case EN_SENDMODE_FIFO:
	//case EN_SENDMODE_FIFO_TIMESTAMP:
		if (0 == pSendPara->dwBufferLen)
			return NULL;
		break;
	default:
		break;
	}

	CDeviceAccess *pSend = CDeviceAccess::s_vetDevice[pSendPara->dwCardIndex];
	ASSERT(pSend);
	if(NULL == pSend)
		return NULL;

	bool bRet = pSend->SendCreate(pSendPara);

//printf("EtherIO_CreateSendObj,  idx=%d, hand=%X\n", pSendPara->dwCardIndex, (int)pSend);
	return ( (bRet) ? (ETHERIO_SEND_HANDLE)pSend : NULL );
}

//
// 删除发送对象，输入为上边创建的发送对象句柄
//
ETHERIO_API bool EtherIO_ReleaseSendObj(IN ETHERIO_SEND_HANDLE sendHandle)
{
	ASSERT(NULL != sendHandle);
	if (NULL == sendHandle)
		return false;

	return ((CDeviceAccess*)sendHandle)->SendRelease();
}

//
// 启动发送，输入为上边创建的发送对象句柄
//
ETHERIO_API bool EtherIO_StartSend(
		IN ETHERIO_SEND_HANDLE sendHandle,
		IN EN_TIME_MODE enTimeMode)
{
	ASSERT(NULL != sendHandle);
	if (NULL == sendHandle)
		return false;

	bool bRet = ((CDeviceAccess*)sendHandle)->SetTimeMode(enTimeMode);
	if (!bRet)
		return false;

	return ((CDeviceAccess*)sendHandle)->SendStart();
}

//
// 停止发送，输入为上边创建的发送对象句柄
//
ETHERIO_API bool EtherIO_StopSend(IN ETHERIO_SEND_HANDLE sendHandle)
{
	ASSERT(NULL != sendHandle);
	if (NULL == sendHandle)
		return false;

	return ((CDeviceAccess*)sendHandle)->SendStop();
}

//
// 取发送的状态
//		sendHandle - 发送对象句柄
//		OUT sendInfo - 返回发送的信息
//
ETHERIO_API bool EtherIO_GetSendInfo(
		IN ETHERIO_SEND_HANDLE sendHandle,
		OUT GET_SEND_INFO *pSendInfo)
{
	ASSERT(NULL != sendHandle);
	ASSERT(NULL != pSendInfo);
	if (NULL==sendHandle || NULL==pSendInfo)
		return false;

	return ((CDeviceAccess*)sendHandle)->SendGetInfo(*pSendInfo);
}


//
// 设置发送的时间精度
//		sendHandle - 发送对象句柄
//		enTimeMode - 时间精度
//
ETHERIO_API bool EtherIO_SetSendTimeMode(
		IN ETHERIO_SEND_HANDLE sendHandle,
		IN EN_TIME_MODE enTimeMode)
{
	ASSERT(NULL != sendHandle);
	if (NULL==sendHandle)
		return false;

	return ((CDeviceAccess*)sendHandle)->SetTimeMode(enTimeMode);
}


//
// 发送数据
//		sendHandle - 发送对象句柄
//		pPacket - 数据包指针
//		dwPktLen - 数据包长度
//		dwReTryTimes - 重试次数
//
ETHERIO_API bool EtherIO_SendPacket(
		IN ETHERIO_SEND_HANDLE sendHandle,
		IN PUCHAR pPacket,
		IN DWORD dwPktLen,
		IN DWORD dwReTryTimes)
{
	ASSERT(NULL != sendHandle);
	ASSERT(NULL != pPacket);

	return EtherIO_SendPacketWithTimestamp(sendHandle,
		pPacket,dwPktLen,dwReTryTimes,-1);
}


//
// 从发送缓冲区申请一段内存
//		sendHandle - 发送对象句柄
//		dwWriteLength - 申请的长度
//		dwReTryTimes - 重试次数
//		返回缓冲区的指针
//
ETHERIO_API PVOID EtherIO_AllocSendBuffer(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength, 
		IN DWORD dwReTryTimes)
{
	ASSERT(NULL != sendHandle);

	return EtherIO_AllocSendBufferWithTimestamp(sendHandle,dwWriteLength,dwReTryTimes,-1);
}


//
// 通知发送缓冲区，使用了的数据长度
//		sendHandle - 发送对象句柄
//		dwWriteLength - 使用了的长度
//		dwReTryTimes - 重试次数
//
//	 注意：使用此函数前，要先调用EtherIO_AllocSendBuffer申请一个缓冲区，
//
ETHERIO_API bool EtherIO_FinishedSendBuffer(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength)
{
	ASSERT(NULL != sendHandle);
// 	if (NULL==sendHandle)
// 		return false;

	return EtherIO_FinishedSendBufferWithTimestamp(sendHandle,dwWriteLength);;
}



//
// 发送数据(带有时间辍的方式)
//		sendHandle - 发送对象句柄
//		pPacket - 数据包指针
//		dwPktLen - 数据包长度
//		dwReTryTimes - 重试次数
//
ETHERIO_API bool EtherIO_SendPacketWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle,
		IN PUCHAR pPacket,
		IN DWORD dwPktLen,
		IN DWORD dwReTryTimes,
		IN DWORD dwOffset)
{
	ASSERT(NULL != sendHandle);
	ASSERT(NULL != pPacket);
	return ((CDeviceAccess*)sendHandle)->
		SendPacketWithTimestamp(pPacket, dwPktLen, dwReTryTimes, dwOffset);
}
//
// 从发送缓冲区申请一段内存(带有时间辍的方式)
//		sendHandle - 发送对象句柄
//		dwWriteLength - 申请的长度
//		dwReTryTimes - 重试次数
//		返回缓冲区的指针
//
ETHERIO_API PVOID EtherIO_AllocSendBufferWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength, 
		IN DWORD dwReTryTimes,
		IN DWORD dwOffset)
{
	ASSERT(NULL != sendHandle);
	return ((CDeviceAccess*)sendHandle)->
		AllocateBufferWithTimestamp(dwWriteLength, dwReTryTimes, dwOffset);
}

//
// 通知发送缓冲区，使用了的数据长度(带有时间辍的方式)
//		sendHandle - 发送对象句柄
//		dwWriteLength - 使用了的长度
//		dwReTryTimes - 重试次数
//
//	 注意：使用此函数前，要先调用EtherIO_AllocSendBuffer申请一个缓冲区，
//
ETHERIO_API bool EtherIO_FinishedSendBufferWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength)
{
	ASSERT(NULL != sendHandle);
	return ((CDeviceAccess*)sendHandle)->
		FinishedBufferWithTimestamp(dwWriteLength);
}



//
// 设置BLOCK发送方式下的待发送数据
//		sendHandle - 发送对象句柄
//		pSendData - 应用程序开辟并设置的数据缓冲区指针
//
// bool 改为 int
//	0 -- 成功
//	-1 -- 数据长度错误
//	-2 -- 当前的发送模式错误
//	-3 -- 与驱动交互错误
ETHERIO_API int EtherIO_BlockSetData(
		IN ETHERIO_SEND_HANDLE sendHandle,
		IN BLOCK_SEND_DATA_STAMP* pSendData)
{
	ASSERT(NULL != sendHandle);
	if (NULL==sendHandle)
		return -1;

	return ((CDeviceAccess*)sendHandle)->BlockSetData(pSendData);
}


//
// 设置BLOCK发送方式下的发送参数
//		sendHandle - 发送对象句柄
//		pSendData - 应用程序开辟并设置的发送参数指针
//
ETHERIO_API bool EtherIO_BlockSetPara(
		IN ETHERIO_SEND_HANDLE sendHandle,
		IN BLOCK_SEND_PARA* pSendPara)
{
	ASSERT(NULL != sendHandle);
	if (NULL==sendHandle)
		return false;

	return ((CDeviceAccess*)sendHandle)->BlockSetPara(pSendPara);
}


//
// 取发送缓冲区的使用字节数
//
//		recvHandle - 为上边创建的接收对象句柄
//		OUT pTotalLen - 返回这个缓冲区的总长度
//
//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

ETHERIO_API DWORD EtherIO_GetSendBufferUsed(
		IN ETHERIO_RECV_HANDLE sendHandle,
		OUT DWORD* pTotalLen = NULL)
{
	ASSERT(NULL != sendHandle);
	if (NULL==sendHandle)
		return false;

	DWORD dwtemp = 0;

	if( !pTotalLen )
		pTotalLen = &dwtemp;

	return ((CDeviceAccess*)sendHandle)->GetBufferUsed(*pTotalLen);
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					资源部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
// 释放驱动中所有申请的资源
//
ETHERIO_API void EtherIO_ResourceReleaseAll()
{
	CDeviceAccess::s_ResourceReleaseAll();
}
//
// 释放驱动中本进程占用的所有资源
//
ETHERIO_API void EtherIO_ResourceReleaseAllMine()
{
	CDeviceAccess::s_ResourceReleaseAllMine();
}
//
// 释放驱动中非本进程占用的所有资源
//
ETHERIO_API void EtherIO_ResourceReleaseAllOther()
{
	CDeviceAccess::s_ResourceReleaseAllOther();
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					OID 部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ETHERIO_API bool EtherIO_GetNICStatistic(
		IN int iIndex,
		IN DWORD dwStatType,
		OUT ULONGLONG *ddwValue)
{
	ASSERT(iIndex < (int)CDeviceAccess::s_vetDevice.size());

	ETHERIO_OID_DATA* pOidData = (ETHERIO_OID_DATA*)new
		BYTE[sizeof(ETHERIO_OID_DATA)+4];
	if (NULL == pOidData)
		return false;

	pOidData->CardIndex = iIndex;
	pOidData->Oid = dwStatType;
	pOidData->Length = sizeof(ULONG) * 2;
	*(ULONGLONG*)(pOidData->Data) = 0;

	bool bRet = CDeviceAccess::GetOid(*pOidData);
	if (bRet && NULL!=ddwValue)
		*ddwValue = *(ULONGLONG*)pOidData->Data;

	delete pOidData;

	return bRet;

/*
	ASSERT(iIndex < CDeviceAccess::s_vetDevice.size());

	ETHERIO_OID_DATA oidData;

	oidData.CardIndex = iIndex;
	oidData.Oid = dwStatType;
	oidData.Length = sizeof(ULONG) * 2;
	*(ULONG*)(oidData.Data) = 0;

	bool bRet = CDeviceAccess::GetOid(oidData);
	if (bRet && NULL!=ddwValue)
		*ddwValue = *(ULONG*)oidData.Data;

	return bRet;
*/
}

/*
不包括报头的帧最大传输长度
return:	0 -- failed
		else -- right
*/
ETHERIO_API int EtherIO_GetMaxFrameSize(IN int iIndex)
{
	ASSERT(iIndex < (int)CDeviceAccess::s_vetDevice.size());
	if(iIndex >= (int)CDeviceAccess::s_vetDevice.size())
		return -1;

	return CDeviceAccess::s_vetDevice[iIndex]->GetMaxFrameSize();
}


