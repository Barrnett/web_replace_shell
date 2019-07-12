/*=========================================================
*	File name	：	adapterobj.cpp
*	Authored by	：	
*	Date		：	2002-1-23 23:19:21
*	Description	：	
*
*	Modify  	：	
*=========================================================*/

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

#include "adapterobj.h"
//#include "Packet.h"

#pragma comment(lib,"Iphlpapi.lib")
//#ifdef _DCARDDRIVE
#if defined (OS_WINDOWS) && (_WIN64)
#	pragma comment( lib, "./winpcap/lib_x64/packet.lib")
#	pragma comment( lib, "./winpcap/lib_x64/wpcap.lib")
#else
#	pragma comment( lib, "./winpcap/lib_win32/packet.lib")
#	pragma comment( lib, "./winpcap/lib_win32/wpcap.lib")
#endif

//#else
//#pragma comment( lib, "./winpcap/Probe.lib")
//#endif //_DCARDDRIVE


adapterobj::adapterobj(INPUT_TYPE type)
{
	m_inputtype = type;
	m_lpAdapter = NULL;
	m_bOpen = FALSE;
	m_OpenAdapterNo = 0;

	memset(m_AdapterName,0x00,sizeof(m_AdapterName));
}

adapterobj::~adapterobj()
{
	Close();
}

void adapterobj::Close()
{
	
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 		D_PacketCloseDriver();
// 	else if (m_lpAdapter)
		PacketCloseAdapter(m_lpAdapter);

	m_lpAdapter = NULL;
	m_bOpen = FALSE;
}

BOOL adapterobj::ResetAdapter()
{ 
/*
	if (INPUTDATATYPE_TAP == m_inputtype)
	{			
		int iRet = D_PacketClearStssInfo();
		if(-1 == iRet)
			return FALSE;
	}*/

	return TRUE;
}

int  adapterobj::EnumAdapterInfo(BOOL bGet)
{
	if (INPUTDATATYPE_TAP == m_inputtype && bGet)
	{
		return 0;
	}

	memset(m_AdapterName,0x00,sizeof(m_AdapterName));
	
	return adapterobj::GetAdaptersName_NT(m_AdapterName);
}

/*
BOOL adapterobj::Open(int open)
{
	Close();

	pcap_if_t*  alldevsp;
	char errbuf[MAX_PATH];
	int iRet = pcap_findalldevs(&alldevsp, errbuf);

	if (iRet == -1)
		return FALSE;

	pcap_if_t*  pDevSearch = alldevsp;
	while(pDevSearch)
	{
		if (0 == open)
		{
			pcap_t* pHandle = pcap_open_live(pDevSearch->name, 1600, true, 3, errbuf);
			pcap_freealldevs(alldevsp);

			if (NULL == pHandle)
			{
				return FALSE;
			}
		}

	}
	if (INPUTDATATYPE_TAP == m_inputtype)
	{
		int iRet = D_PacketOpenDriver();
		if(-1 == iRet)
			return FALSE;
	}	
	else
	{		
		char *pAdapt = GetAdapterInfo(open);
		if(pAdapt==NULL)
		{
			char buf[128];
			strcpy(buf,"Unable find the card for driver");
			MessageBox(NULL,buf,"ERROR",MB_OK|MB_ICONERROR); 
			return FALSE;
		}

		m_lpAdapter = PacketOpenAdapter(pAdapt);
		if (!m_lpAdapter || (m_lpAdapter->hFile == INVALID_HANDLE_VALUE))
		{
			DWORD dwErrorCode = GetLastError();
			char buf[128];
			sprintf(buf,"Unable to open the driver, Error Code : %lx\n",dwErrorCode);
			MessageBox(NULL,buf,"ERROR",MB_OK|MB_ICONERROR); 
			return FALSE;
		}
	}


	m_OpenAdapterNo = open;
	BeOpen();

	return TRUE;
}*/

BOOL adapterobj::Open(int open)
{
	Close();

/*
	if (INPUTDATATYPE_TAP == m_inputtype)
	{
		int iRet = D_PacketOpenDriver();
		if(-1 == iRet)
			return FALSE;
	}	
	else*/
	{		
		char *pAdapt = GetAdapterInfo(open);
		if(pAdapt==NULL)
		{
			char buf[128];
			strcpy(buf,"Unable find the card for driver");
			MessageBox(NULL,buf,"ERROR",MB_OK|MB_ICONERROR); 
			return FALSE;
		}
				
		m_lpAdapter = PacketOpenAdapter(pAdapt);
		if (!m_lpAdapter || (m_lpAdapter->hFile == INVALID_HANDLE_VALUE))
		{
			DWORD dwErrorCode = GetLastError();
			char buf[128];
			sprintf(buf,"Unable to open the driver, Error Code : %lx\n",dwErrorCode);
			MessageBox(NULL,buf,"ERROR",MB_OK|MB_ICONERROR); 
			return FALSE;
		}
	}


	m_OpenAdapterNo = open;
	BeOpen();
		
	return TRUE;
}

BOOL adapterobj::SetHwFilter(ULONG Filter)
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{
// 		int iRet = D_PacketSetFilter(Filter);
// 		if(-1 == iRet)
// 			return FALSE;
// 		return TRUE;
// 	}
// 	else
	{
		if(m_lpAdapter)
			return PacketSetHwFilter(m_lpAdapter,Filter);
		
		return FALSE;
	}
}


BOOL adapterobj::SetDriverBuff(int dim)
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{	
// 		int iRet = D_PacketAllocateBuffer(dim);
// 		if (-1 == iRet)
// 			return FALSE;
// 		return TRUE;
// 	}
// 	else
	{
		//	dim = Packet_WORDALIGN(dim);
		if (m_lpAdapter)
			return PacketSetBuff(m_lpAdapter,dim);
		return FALSE;
	}
}


BOOL adapterobj::SetMinToCopy(int nNum)
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{
// 		READ_FLAGS flag;
// 		flag.nTimeout = 1000;
// 		flag.nValue = nNum;
// 		flag.nReadByType = READ_BY_PACKET;
// 		int iRet = D_PacketSetReadFlags(&flag);
// 		if(-1 == iRet)
// 			return FALSE;
// 		return TRUE;
// 	}
// 	else
	{
		if(m_lpAdapter)
			return PacketSetMinToCopy(m_lpAdapter,nNum);
		return FALSE;
	}
}

BOOL adapterobj::SetReadTimeout(int timeout/*=1000*/)
{
	if (INPUTDATATYPE_TAP == m_inputtype)
	{
		int iRet = 0;//PacketSetReadTimeout(timeout);
		if(-1 == iRet)
			return FALSE;
		return TRUE;
	}
	else
	{
		if(m_lpAdapter)
			return PacketSetReadTimeout(m_lpAdapter,timeout);
		return FALSE;
	}
}


BOOL adapterobj::ReceivePacket(LPPACKET lpPacket,BOOLEAN Sync)
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{
// 		int iRet = D_PacketReceivePacket(m_lpAdapter,lpPacket,Sync);
// 		
// 		if(-1 == iRet)
// 			return FALSE;
// 
// 		return TRUE;
// 	}
// 	else
	{		
		int iRet = PacketReceivePacket(m_lpAdapter,lpPacket,Sync);
		return iRet;
	}
}

char *adapterobj::GetAdapterInfo(int open)
{
	if (INPUTDATATYPE_TAP == m_inputtype)
	{
		return NULL;
	}
	
	int i = EnumAdapterInfo();

	if(i>0 && i>open)
	{
/*
		//DUMPSTATE("打开编号为%d的网卡!\r\n", open);
		ULONG ulMask = 0;
		ULONG ulIP = 0;
		//DUMPSTATE("网卡列表如下:\r\n");
		for(int j=0; j<i; j++)
		{
			if (PacketGetNetInfo(m_AdapterName[j], &ulIP, &ulMask))
			{
				//DUMPSTATE("编号：%d IP地址: %s\r\n", j, DWIPtoString(ulIP));
			}
		}*/
		
		return m_AdapterName[open];
	}

	return NULL;
		
}
//#endif

/*
PACKET* adapterobj::PacketAlloc()
{
	m_lock.Lock();
	PACKET* x;
	x = m_lpPacketCache;
	if (x == NULL)
	{
		x = (PACKET*)m_PacketMem.Alloc();
		x->Buffer = m_BuffMem.Alloc();
	}
	else
	{
		// Return block from cache, update it
		PACKET** x_fp = (PACKET**) x;
		m_lpPacketCache = *x_fp;
		VerifyMem((PACKET*)x);
	}
	x->Length = m_ReadBuffSize;
	x->bIoComplete = FALSE;
	
	m_lock.Unlock();
	
	return x;
}

void adapterobj::PacketFree(PACKET* blk)
{
	m_lock.Lock();
	VerifyMem((PACKET*)blk);
	// Check the block; translates to nothing in release builds
	ASSERT (blk != NULL);
	// Put reference to next free block in first 4 bytes
	PACKET** blk_fp = (PACKET**) blk;
	*blk_fp = m_lpPacketCache;
	// And add block as head of cache
	m_lpPacketCache = (PACKET*)blk;
	m_lock.Unlock();
	
	
}
*/

WORD adapterobj::checksum(USHORT *buffer, int size)
{
	ACE_UINT32 cksum=0;  
	
	while(size >1) {  
		
		cksum+=*buffer++;  
		
		size -=sizeof(USHORT);  
	}  
	
	if(size ) {  
		
		cksum += *(UCHAR*)buffer;  
		
	}  
	
	cksum = (cksum >> 16) + (cksum & 0xffff);  
	
	cksum += (cksum >>16);  
	
	return (USHORT)(~cksum);  
}


BOOL adapterobj::StartCapture()
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{
// 		int iRet = D_PacketStartCapture();
// 
// 		return true;
// 	}

	return TRUE;
}

void adapterobj::StopCapture()
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{
// 		D_PacketStopCapture();
// 	}
}

BOOL adapterobj::GetStats(bpf_stat *stat)
{
// 	if (INPUTDATATYPE_TAP == m_inputtype)
// 	{
// 		DRIVER_STSS_INFO info;
// 		int iRet = D_PacketQureyStssInfo(&info);
// 		stat->bs_drop = info.ulRxLostPacketCount;
// 		stat->bs_recv = info.ulRxPacketCount;
// 		if(-1 == iRet)
// 		{
// 			stat->bs_drop = 0;
// 			stat->bs_recv = 0;
// 			return FALSE;
// 		}
// 
// 		return TRUE;
// 	}
// 	else
	{
		return PacketGetStats(m_lpAdapter,stat);
	}
}

int adapterobj::GetAdaptersName_NT(char AdapterList[][1024])
{
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	if( !(dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4))
	{  
		//unicode strings (winnt)
		WCHAR		tempName[1024*10]; // string that contains a list of th network adapters
		ULONG		AdapterLength = sizeof(tempName)/sizeof(tempName[0]);

		memset(tempName,0x00,sizeof(tempName));

		// Windows NT
		if( PacketGetAdapterNames((char*)tempName, &AdapterLength) == FALSE )
		{
			return 0;
		}

		WCHAR		*temp,*temp1;	

		temp = tempName;
		temp1= tempName;
		int count = 0;
		while(( *temp != '\0' ) || ( *(temp-1) != '\0' ))
		{
			if( *temp == '\0' ) 
			{
				memcpy(AdapterList[count], temp1, (temp-temp1)*2 );
				temp1 = temp+1;
				count++;
			}

			temp++;
		}
		
		return count;				
	}

	return 0;
}

BOOL adapterobj::SendRSTPacket(const char *Buffer,int Buflen)
{
	
	if(m_lpAdapter==NULL)
		return FALSE;

	return FALSE;
/*
	static PACKET packet;
	static char data[100] = {
			//------------  ETHER Header  ------------
			'\x00','\x00','\x00','\x00','\x00','\x00',//dest mac
			'\x00','\x00','\x00','\x00','\x00','\x00',//src mac
			'\x08','\x00',//IP
			
			//------------  IP Header  ------------ 
			'\x45',//IP head
			'\x00',//Type of service
			'\x00','\x28',//Packet length
			'\x00','\x00',//Id
			'\x40','\x00',//Fragmentation Info
			'\x80',//Time to live
			'\x06',//Protocol = TCP
			'\x00','\x00',//Header checksum
			'\x00','\x00','\x00','\x00',//Source address
			'\x00','\x00','\x00','\x00',//Destination address
			
			//------------  TCP Header  ------------
			'\x00','\x00',//Source port
			'\x00','\x00',//Destination port
			'\x00','\x00','\x00','\x00',// Sequence number
			'\x00','\x00','\x00','\x00',//  Ack number
			'\x50',//Data offset
			'\x04',//Flags
			'\x00','\x00',//Window
			'\x00','\x00',//Checksum
			'\x00','\x00',//ptr offset
			'\x00','\x00','\x00','\x00','\x00','\x00',//fill data
	};

	struct RSTPACKET{
		char destMac[6];
		char srcMac[6];
		WORD  Ident;
		DWORD srcIP;
		DWORD destIP;
		WORD  srcPort;
		WORD  destPort;
		DWORD seqNumber;
	};

	packet.Buffer = data;
    packet.Length = 60;
	packet.ulBytesReceived = 0;
	packet.bIoComplete = FALSE;

	RSTPACKET *p = (RSTPACKET *)Buffer;
	char *ptr = data;
    memcpy(ptr,p->destMac,6);
	ptr = data + 6;
    memcpy(ptr,p->srcMac,6);

	ptr = data + 18;
	*(WORD*)(ptr) = p->Ident;

	ptr = data + 26;
	*(DWORD*)(ptr) = p->srcIP;

	ptr = data + 30;
	*(DWORD*)(ptr) = p->destIP;

	ptr = data + 34;
	*(WORD*)(ptr) = p->srcPort;

	ptr = data + 36;
	*(WORD*)(ptr) = p->destPort;

	ptr = data + 38;
	*(DWORD*)(ptr) = p->seqNumber;

	//IP checksum
	ptr = data + 24;
	WORD *sum = (WORD*)(ptr);
	*sum = 0;
	ptr = data +14;
	*sum = checksum((WORD*)ptr,20);

	//TCP checksum
	struct //定义TCP伪首部  
		
	{  
		
		DWORD saddr; //源地址  
		
		DWORD daddr; //目的地址  
		
		char mbz;  
		
		char ptcl; //协议类型  
		
		WORD tcpl; //TCP长度  
		
	}psd_header; 
	
	psd_header.saddr =p->srcIP ;
	psd_header.daddr = p->destIP;
	psd_header.mbz = 0;
	psd_header.ptcl = 6;
	psd_header.tcpl = 0x1400;
	ptr = data + 50;
	sum = (WORD*)(ptr);
	*sum = 0;
	ptr = data +34;
	memcpy(data+60,&psd_header,12);
	*sum = checksum((WORD*)ptr,26+12);
	
	//PacketSetNumWrites(m_lpAdapter,1);
    BOOL bRet = PacketSendPacket(m_lpAdapter,&packet,FALSE);
	return bRet;
	*/
}






#endif//OS_WINDOWS

