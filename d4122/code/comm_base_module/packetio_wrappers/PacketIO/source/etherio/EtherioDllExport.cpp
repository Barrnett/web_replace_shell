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

		//��ֹ���������������
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
// ȡ�豸MAC��ַ�����뻺����Ҫ��С��6���ֽ�
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

//�õ�����"��������|��������1"
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
// ͨ��MAC��ַ��ð忨��index
//		pszMacAddr - MAC��ַ�ַ�����������0a0b0c0d0e0f��ʽ������0a:0b:0c...��
//			�м��������ַ�����������ִ�Сд
//		return: -1 - input error, else - index
//
ETHERIO_API int EtherIO_GetOneIndexByMAC(IN const char* pszMacAddr)
{
	ASSERT(pszMacAddr);
	if (NULL == pszMacAddr)
		return -1;

	// �ж�����
	size_t iLen = strlen(pszMacAddr);
	if (EN_MAC_FULL_LEN!=iLen && EN_MAC_SELF_LEN!=iLen)
		return -1;

	char macSrc[32];
	char macTmp[32];
	char* pszTemp = (char*)macTmp;
	char chSep;

	// Դת����д
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
//��������"��������|��������1",������벻�Ϸ�����û���ҵ�������0
ETHERIO_API int EtherIO_GetMultiIndexByShowName(IN const char *pszShowName)
{
	ASSERT(pszShowName);
	if (NULL == pszShowName || pszShowName[0] == 0 )
		return 0;

	DWORD dwIndexLst = 0;	// �ϳɺ��index�б�

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
	// �����ַ����ָ�
	if (NULL == pszMacLst)
		return 0;

	DWORD dwIndexLst = 0;	// �ϳɺ��index�б�
	int iIndex = 0;
	char sep = ':';
	const char* pTemp = pszMacLst;

	char macTemp[32];

	// ��ΪҪ�������ָ���������һ��������MAC��ַ����Ӧ��Ϊ17���ַ�
	while(strlen(pTemp) >= EN_MAC_FULL_LEN)	// >= EN_MAC_FULL_LEN)
	{
		// ͨ���ָ������ж�һ��MAC��ַ����ʼλ��
		sep = pTemp[2];
		if ('-'!=sep && ':'!= sep)
		{
			pTemp ++;
			continue;
		}

		// �ҵ�һ��MAC����ʼλ��,ͨ�����MAC�ַ�������index
		strncpy(macTemp, pTemp, EN_MAC_FULL_LEN);
		macTemp[EN_MAC_FULL_LEN] = '\0';

		iIndex = EtherIO_GetOneIndexByMAC(macTemp);
		if (iIndex >= 0)
		{
			dwIndexLst |= (1 << iIndex);
		}
		// ����������MAC��ַ������ֱ�ӷ���
		else
		{
			return 0;
		}

		// ��������ַ���
		pTemp += EN_MAC_FULL_LEN;
	}

	return dwIndexLst;

}




//,������벻�Ϸ�����û���ҵ�������-1
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
// ȡ�豸���ʣ���λ��kbps��0 �� -1��Ч
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
// ȡ�豸�Ľ���ͳ��
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
	FILE* fd = fopen("/proc/net/dev", "r");//��ϵͳ�ļ��鿴�����ӿ�  
	
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
//					���ղ���                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//typedef UINT64 ETHERIO_RECV_HANDLE;

//
// �������ն���
//		dwDevLst - ��λ��ʾ�ĳ�Ա�б�
//		dwBufLen - ָ�����ջ�������С�����ֽ�Ϊ��λ
//		���Ϊ���ն�������0Ϊʧ��
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


// ɾ�����ն�������Ϊ�ϱߴ����Ľ��ն�����
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
// �������գ�����Ϊ�ϱߴ����Ľ��ն�����
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
// ֹͣ���գ�����Ϊ�ϱߴ����Ľ��ն�����
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
//  2��ȡ���ݵķ�ʽ�������Ի���
//		EtherIO_GetPacket��EtherIO_SkipPacket�ɶ�ʹ��
//		EtherIO_GetNextPacket����ʹ��
//
//////////////////////////////////////////////////////////////////////////

//
// ȡ����
//		recvHandle - Ϊ�ϱߴ����Ľ��ն�����
//		OUT pPacket - ����������ݰ��ĵ�ַ
//		OUT stamp - ����������ݰ���ʱ���
//		OUT dwAttrib - ���������������
//		OUT dwOriginalLen - ����ԭʼ����
// 
//	 ����ֵ
//		>0 - ��ʾ�����ݰ�������ֵ��ʾ���ݰ��ĳ���
//		=0 - ��ǰ�ް�
//		<0 - ��������״̬������ֵ�ο�<CommonQueueManager.h>�ļ���PACKET_TYPE_XX�Ķ���
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
		*dwOriginalLen = hdr.len + 4;	// CRC�ĳ���
		// �����±���ʵֻ������2���ֽڣ�Magic Number��������Ϊ�˺�windows�汾���ݣ�����Ҫ���ش���CRC�ĳ��ȣ���Ҫ�ϱߵ�app��֤��ʹ�����2��byte
		
		if (-2 == hdr.extended_hdr.if_index)
		{
			// ��ʱ��
			return -2;
		}


		static const u_int16_t u16_magic_value = RING_MAGIC_VALUE << 8 | RING_MAGIC_VALUE;
		u_int16_t* tail_padding = (u_int16_t*)(*pPacket + hdr.caplen);
		if( u16_magic_value == *tail_padding )//ĩβ���ֽ����0
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
// ������ǰ���ݰ�
// 
ETHERIO_API void EtherIO_SkipPacket(IN ETHERIO_RECV_HANDLE recvHandle)
{
	(void)recvHandle;
	return;	
}

// 
// ȡ���ݣ�ʹ���������ȡ���ݵ�ʱ�򣬲���Ҫskip����
//	
ETHERIO_API int EtherIO_GetNextPacket(
									  IN ETHERIO_RECV_HANDLE recvHandle, 
									  OUT PUCHAR *pPacket, 
									  OUT EIO_TIME *stamp,
									  OUT DWORD *card_index, 
									  OUT DWORD *dwOriginalLen)
{
	ASSERT(NULL != recvHandle);
	static int _iLastPktLen = 0;	// ��¼��һ�εİ����ȣ��Ա�skip

	if (_iLastPktLen > 0)
	{
		EtherIO_SkipPacket(recvHandle);
		_iLastPktLen = 0;
	}

	_iLastPktLen = EtherIO_GetPacket(recvHandle, pPacket, stamp, card_index, dwOriginalLen);

	return _iLastPktLen;
}



//
// ȡ���ն���Ľ���ͳ��
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
// ȡ���ջ�������ʹ���ֽ���
//
//		recvHandle - Ϊ�ϱߴ����Ľ��ն�����
//		OUT pTotalLen - ����������������ܳ���
// 
//	 ����ֵ
//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
//
//	 ˵��
//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�
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
//					���Ͳ���                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//
// �������Ͷ���
//		iIndex - Ҫ���͵��豸������
//		dwBufLen - ָ�����ջ�������С�����ֽ�Ϊ��λ
//		���Ϊ���Ͷ�������0Ϊʧ��
//
ETHERIO_API ETHERIO_SEND_HANDLE EtherIO_CreateSendObj(
		SEND_CREATE_PARA*  pSendPara)
{
	ASSERT(pSendPara);
	ASSERT(pSendPara->dwCardIndex < CDeviceAccess::s_vetDevice.size());

	if ( pSendPara->dwCardIndex >= CDeviceAccess::s_vetDevice.size() )
		return NULL;

	// �������
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
// ɾ�����Ͷ�������Ϊ�ϱߴ����ķ��Ͷ�����
//
ETHERIO_API bool EtherIO_ReleaseSendObj(IN ETHERIO_SEND_HANDLE sendHandle)
{
	ASSERT(NULL != sendHandle);
	if (NULL == sendHandle)
		return false;

	return ((CDeviceAccess*)sendHandle)->SendRelease();
}

//
// �������ͣ�����Ϊ�ϱߴ����ķ��Ͷ�����
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
// ֹͣ���ͣ�����Ϊ�ϱߴ����ķ��Ͷ�����
//
ETHERIO_API bool EtherIO_StopSend(IN ETHERIO_SEND_HANDLE sendHandle)
{
	ASSERT(NULL != sendHandle);
	if (NULL == sendHandle)
		return false;

	return ((CDeviceAccess*)sendHandle)->SendStop();
}

//
// ȡ���͵�״̬
//		sendHandle - ���Ͷ�����
//		OUT sendInfo - ���ط��͵���Ϣ
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
// ���÷��͵�ʱ�侫��
//		sendHandle - ���Ͷ�����
//		enTimeMode - ʱ�侫��
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
// ��������
//		sendHandle - ���Ͷ�����
//		pPacket - ���ݰ�ָ��
//		dwPktLen - ���ݰ�����
//		dwReTryTimes - ���Դ���
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
// �ӷ��ͻ���������һ���ڴ�
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ����ĳ���
//		dwReTryTimes - ���Դ���
//		���ػ�������ָ��
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
// ֪ͨ���ͻ�������ʹ���˵����ݳ���
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ʹ���˵ĳ���
//		dwReTryTimes - ���Դ���
//
//	 ע�⣺ʹ�ô˺���ǰ��Ҫ�ȵ���EtherIO_AllocSendBuffer����һ����������
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
// ��������(����ʱ��꡵ķ�ʽ)
//		sendHandle - ���Ͷ�����
//		pPacket - ���ݰ�ָ��
//		dwPktLen - ���ݰ�����
//		dwReTryTimes - ���Դ���
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
// �ӷ��ͻ���������һ���ڴ�(����ʱ��꡵ķ�ʽ)
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ����ĳ���
//		dwReTryTimes - ���Դ���
//		���ػ�������ָ��
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
// ֪ͨ���ͻ�������ʹ���˵����ݳ���(����ʱ��꡵ķ�ʽ)
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ʹ���˵ĳ���
//		dwReTryTimes - ���Դ���
//
//	 ע�⣺ʹ�ô˺���ǰ��Ҫ�ȵ���EtherIO_AllocSendBuffer����һ����������
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
// ����BLOCK���ͷ�ʽ�µĴ���������
//		sendHandle - ���Ͷ�����
//		pSendData - Ӧ�ó��򿪱ٲ����õ����ݻ�����ָ��
//
// bool ��Ϊ int
//	0 -- �ɹ�
//	-1 -- ���ݳ��ȴ���
//	-2 -- ��ǰ�ķ���ģʽ����
//	-3 -- ��������������
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
// ����BLOCK���ͷ�ʽ�µķ��Ͳ���
//		sendHandle - ���Ͷ�����
//		pSendData - Ӧ�ó��򿪱ٲ����õķ��Ͳ���ָ��
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
// ȡ���ͻ�������ʹ���ֽ���
//
//		recvHandle - Ϊ�ϱߴ����Ľ��ն�����
//		OUT pTotalLen - ����������������ܳ���
//
//	 ����ֵ
//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
//
//	 ˵��
//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�

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
//					��Դ����                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
// �ͷ������������������Դ
//
ETHERIO_API void EtherIO_ResourceReleaseAll()
{
	CDeviceAccess::s_ResourceReleaseAll();
}
//
// �ͷ������б�����ռ�õ�������Դ
//
ETHERIO_API void EtherIO_ResourceReleaseAllMine()
{
	CDeviceAccess::s_ResourceReleaseAllMine();
}
//
// �ͷ������зǱ�����ռ�õ�������Դ
//
ETHERIO_API void EtherIO_ResourceReleaseAllOther()
{
	CDeviceAccess::s_ResourceReleaseAllOther();
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					OID ����                                            //
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
��������ͷ��֡����䳤��
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


