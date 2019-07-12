//////////////////////////////////////////////////////////////////////////
//addr_tools.h

#pragma once

#include "cpf/cpf.h"
#include <string>
#include "cpf/cpf_std.h"
#include <vector>

typedef std::pair<ACE_UINT32, ACE_UINT32> 		IPADDR_RANGE;
typedef std::vector<IPADDR_RANGE>				IPADDR_RANGE_VECT;
typedef std::pair<ACE_UINT16, ACE_UINT16> 		PORT_RANGE;
typedef std::vector< PORT_RANGE >				PORT_RANGE_VET;

typedef std::vector< std::pair<ACE_UINT32,ACE_UINT32> >		VET_IPADDR_RANGE;
typedef std::vector< ACE_UINT16 >							VET_PORT;
typedef std::vector< std::pair<ACE_UINT16,ACE_UINT16> >		VET_PORT_RANGE;
typedef std::vector< ACE_UINT64 >							VET_MACADDR;

typedef struct tagIPv4_TRANSPORT_ADDR
{
	ACE_UINT32 dwIP;//host_order
	ACE_UINT16  wdPort;//host_order

	friend BOOL operator == (const tagIPv4_TRANSPORT_ADDR& a1,const tagIPv4_TRANSPORT_ADDR& a2)
	{
		return (a1.dwIP==a2.dwIP&&a1.wdPort==a2.wdPort);
	}

	friend BOOL operator != (const tagIPv4_TRANSPORT_ADDR& a1,const tagIPv4_TRANSPORT_ADDR& a2)
	{
		return (!(a1==a2));
	}

}IPv4_TRANSPORT_ADDR;

typedef struct tagIPv4_CONNECT_ADDR
{
	//对于接收 数据来说,目的地址和端口是server,源地址和端口是client
	IPv4_TRANSPORT_ADDR client;
	IPv4_TRANSPORT_ADDR server;

	inline int Cmp(const tagIPv4_CONNECT_ADDR &tt) const
	{
		if( client == tt.client && server == tt.server )
			return 1;

		if( server == tt.client && client == tt.server )
			return -1;

		return 0;
	}

	inline int CmpTransAddr(const IPv4_TRANSPORT_ADDR &tt) const
	{
		if( tt == this->client )
			return 1;

		if( tt == this->server )
			return -1;

		return 0;
	}

	inline int Cmp(ACE_UINT32 ip,ACE_UINT16 port) const
	{
		if( client.dwIP == ip && client.wdPort == port )
			return 1;

		if( server.dwIP == ip && server.wdPort == port )
			return -1;

		return 0;
	}

	inline int CmpPort(int hostorder_port) const
	{
		if( client.wdPort == hostorder_port )
			return 1;		
		if( server.wdPort == hostorder_port )
			return -1;
		return 0;
	}

	inline int CmpIP(ACE_UINT32 hostorder_ip) const
	{
		if(client.dwIP == hostorder_ip)
			return 1;
		if(server.dwIP == hostorder_ip)
			return -1;
		return 0;
	}

	inline void dir_set(const tagIPv4_CONNECT_ADDR &tt)
	{
		server = tt.server;
		client = tt.client;
	}

	inline void rev_set(const tagIPv4_CONNECT_ADDR &tt)
	{
		client = tt.server;
		server = tt.client;	
	}

	//客户端的地址和服务器的地址调转
	inline void rev()
	{
		IPv4_TRANSPORT_ADDR temp = client;

		client = server;
		server = temp;
	}

	inline const IPv4_TRANSPORT_ADDR& GetInner(DIR_IO dir) const
	{
		return (dir == DIR_I2O)?client:server;
	}

	inline ACE_UINT32 GetInnerIP(DIR_IO dir) const
	{
		return GetInner(dir).dwIP;
	}

	inline ACE_UINT16 GetInnerPort(DIR_IO dir) const
	{
		return GetInner(dir).wdPort;
	}

	inline const IPv4_TRANSPORT_ADDR& GetOutter(DIR_IO dir) const
	{
		return (dir == DIR_I2O)?server:client;
	}


	inline ACE_UINT32 GetOutterIP(DIR_IO dir) const
	{
		return GetOutter(dir).dwIP;
	}

	inline ACE_UINT16 GetOutterPort(DIR_IO dir) const
	{
		return GetOutter(dir).wdPort;
	}

	//返回内部地址是否是发送方向
	inline BOOL JudgeIP(DIR_IO dir_io,
		ACE_UINT32& inner_ipaddr,
		ACE_UINT32& outter_ipaddr) const
	{
		if( dir_io == DIR_O2I )
		{
			inner_ipaddr = server.dwIP;
			outter_ipaddr = client.dwIP;
			return false;
		}
		else if( dir_io == DIR_I2O )
		{
			inner_ipaddr = client.dwIP;
			outter_ipaddr = server.dwIP;
			return true;
		}
		else
		{
			ACE_ASSERT(FALSE);

			inner_ipaddr = server.dwIP;
			outter_ipaddr = client.dwIP;

			return false;
		}
	}

	//返回内部地址是否是发送方向
	inline BOOL JudgePort(DIR_IO dir_io,
		ACE_UINT16& inner_port,
		ACE_UINT16& outter_port) const
	{
		if( dir_io == DIR_O2I )
		{
			inner_port = server.wdPort;
			outter_port = client.wdPort;
			return false;
		}
		else if( dir_io == DIR_I2O )
		{
			inner_port = client.wdPort;
			outter_port = server.wdPort;
			return true;
		} 
		else
		{
			ACE_ASSERT(FALSE);

			inner_port = server.wdPort;
			outter_port = client.wdPort;

			return false;
		}
	}

}IPv4_CONNECT_ADDR;

//从mac地址的头部分获取源mac和目标mac地址
#define SRC_MAC_ADDR(pMacAddrHeader)	((pMacAddrHeader)+6)
#define DST_MAC_ADDR(pMacAddrHeader)	(pMacAddrHeader)
#define	FORMAT_UINT64_MAC(mac_uint64)	((mac_uint64)&CPF::s_broadcast_mac_int)
#define GET_UINT64_MAC(pmac_addr)		( FORMAT_UINT64_MAC(*(const ACE_UINT64 *)(pmac_addr)) )


//D类地址不分网络地址和主机地址，它的第1个字节的前四位固定为1110。
//　⑵ D类地址范围：224.0.0.0到239.255.255.255
#define D_CLASS_IP(hip)					( ((hip)&0XE0000000)==0XE0000000 )


//判断网络字节序的IP地址是否是广播包
#define IsBroadcast_NIP(netorder_ip)	(((netorder_ip)&0xff000000)==0xff000000)
#define IsBroadcast_HIP(hostorder_ip)	(((hostorder_ip)&0x000000ff)==0x000000ff)

#define IS_BROADCAST_ADDRESS_MAC(pMACAddress) \
	(((*(DWORD*)(pMACAddress)) == 0xFFFFFFFF) \
	&& ((*(DWORD*)(pMACAddress+2)) == 0xFFFFFFFF))

#define IS_GROUP_ADDRESS_MAC(pMACAddress) \
	((*(BYTE*)(pMACAddress)) == 0x01)

namespace CPF
{
	CPF_CLASS
		extern const BYTE s_zero_mac_ptr[8];
	CPF_CLASS
		extern const ACE_UINT64 s_zero_mac_int;

	CPF_CLASS
		extern const BYTE s_broadcast_mac_ptr[8];
	CPF_CLASS
		extern const ACE_UINT64 s_broadcast_mac_int;

	CPF_CLASS
		const char * nip_to_string(ACE_UINT32 nip);

	//ip
	CPF_CLASS
		inline const char * hip_to_string(ACE_UINT32 hip)
	{
		return CPF::nip_to_string(ACE_HTONL(hip));
	}

	CPF_CLASS
		ACE_UINT32 string_to_nip(const char * string_ip);


	CPF_CLASS
		inline ACE_UINT32 string_to_hip(const char * string_ip)
	{
		return ACE_NTOHL(string_to_nip(string_ip));
	}

	CPF_CLASS
		std::string get_iprange_string(ACE_UINT32 beging_hip,ACE_UINT32 end_hip,BOOL bSameShowOne=false);

	//解析"192.168.0.1-192.168.0.255"这样的字符串，得到两个ip地址(网络字节序)
	//如果正确就返回true。否则返回false
	CPF_CLASS 
		BOOL ParseIPRange(const char* source, size_t numtchars,
		ACE_UINT32& begin_nip,ACE_UINT32& end_nip,char sep=('-'));

	CPF_CLASS 
		inline BOOL ParseIPRange(const char * source,ACE_UINT32& begin_nip,ACE_UINT32& end_nip,char sep=('-'))
	{
		return CPF::ParseIPRange(source,ACE_OS::strlen(source),begin_nip,end_nip,sep);
	}

	CPF_CLASS
		int is_valid_ip_string(IN const char * text);

	CPF_CLASS
		int is_valid_mask_string(IN const char * text);

	CPF_CLASS
		int is_valid_mask(IN ACE_UINT32 hipmask);

	//mac

	CPF_CLASS
		inline BOOL IsMacAddrEqual(const void * macaddr1,const void * macaddr2)
	{
		return ( *(ACE_UINT32 *)(macaddr1) == *(ACE_UINT32 *)(macaddr2)
			&& *(ACE_UINT16 *)((BYTE *)macaddr1+4) == *(ACE_UINT16 *)((BYTE *)macaddr2+4) );
	}

	
	CPF_CLASS
		inline BOOL IsZeroMac(const BYTE * macaddr)
	{
		return (!macaddr 
			|| CPF::IsMacAddrEqual(macaddr,s_zero_mac_ptr) );
	}

	CPF_CLASS
		inline BOOL IsBroadcastMac(const BYTE * macaddr)
	{
		return CPF::IsMacAddrEqual(macaddr,s_broadcast_mac_ptr);
	}

	CPF_CLASS
		inline BOOL IsZeroOrBroadcastMac(const BYTE * macaddr)
	{
		return (!macaddr 
			|| CPF::IsZeroMac(macaddr)
			|| CPF::IsBroadcastMac(macaddr));
	}

	CPF_CLASS
		inline void MacAddrCopy(void * dst_mac,const void * src_mac)
	{
		*(ACE_UINT32 *)dst_mac = *(ACE_UINT32 *)src_mac;
		*(ACE_UINT16 *)((BYTE *)dst_mac+4) = *(ACE_UINT16 *)((BYTE *)src_mac+4);
	}

	//MAC地址到字符串的转换,sep表示间隔字符，
	//如果sep=0,那么输出的是010203040506.text的空间长度至少为13个字节。
	//如果间隔字符不为0,比如为':',则输出,01:02:03:04:05:06,其他字符（'-','_'）类推。
	//这时text的空间长度至少为18个字节。
	CPF_CLASS
		void mac_addr_to_a(IN const BYTE * pMacAddr,OUT ACE_TCHAR * text,
		IN ACE_TCHAR sep=ACE_TEXT('-'),IN bool upp=true);

	CPF_CLASS
		const char * mac_addr_to_a(IN const BYTE * pMacAddr,
		IN ACE_TCHAR sep=ACE_TEXT('-'),IN bool upp=true);


	//字符串到MAC地址的转换,sep表示间隔字符，如果sep=NULL，表示不需要输出间隔符号
	//这个函数能够识别010203040506，返回时:*sep=0;
	//也能够识别01:02:03:04:05:06，或者其它一个可见字符的间隔字符。
	//如果转换成功，返回true。否则返回false
	CPF_CLASS
		bool mac_a_to_addr(IN const ACE_TCHAR * text,OUT BYTE * pMacAddr,OUT ACE_TCHAR * sep=NULL);

	//字符串到MAC地址的转换,输出到macddr里面的头6个字节。后两个字节设置成0
	CPF_CLASS
		inline bool mac_a_to_addr2(IN const char * text,OUT ACE_UINT64& macddr,OUT ACE_TCHAR * sep=NULL)
		{
			macddr = 0;
			return mac_a_to_addr(text,(BYTE *)&macddr,sep);
		}

	//看输入的字符串是否是符合MAC地址格式
	CPF_CLASS
		bool is_valid_mac_string(IN const char * text,OUT ACE_TCHAR * sep=NULL);

	CPF_CLASS
		inline BOOL IsMACIn(const void * macaddr,const std::vector<ACE_UINT64>& vt_mac)
		{
			if( !macaddr )
				return false;

			ACE_UINT64 intmac = 0;
			CPF::MacAddrCopy(&intmac,macaddr);

			return CPF::find(vt_mac,intmac);
		}

	CPF_CLASS
		//如果里面有错误的，返回FALSE，如果全部正确返回TRUE
		BOOL TransMACStringToVector(const char * pMacs,std::vector<ACE_UINT64>& vt_macs,char sep=(','));

	CPF_CLASS
		std::string TransMACVectorToString(const std::vector<ACE_UINT64>& vt_macs,char sep=(','));

	CPF_CLASS
		//如果里面有错误的，返回FALSE，如果全部正确返回TRUE
		//将192.168.0.1,192.168.0.6,192.168.0.10转换成vector中的整数
		//vt_ips保存的是主机字节序ip
		BOOL TransIPStringToVector(const char * pIPs,std::vector<ACE_UINT32>& vt_ips,char sep=(','));

	CPF_CLASS
		//和TransIPStringToVector进行相反的动作
		//vt_ips保存的是主机字节序ip	
		std::string TransIPVectorToString(const std::vector<ACE_UINT32>& vt_ips,char sep=(','));


	CPF_CLASS
		//将192.168.0.1,192.168.1.1-192.168.1.254,192.168.2.10-192.168.3.100
		//vt_ips保存的是主机字节序ip1-ip2
		//如果读入有错误，返回false，返回false的时候，程序会尽量转换
		BOOL TransIPRangeStringToVector(const char * pIPs,
		std::vector< std::pair<ACE_UINT32,ACE_UINT32> >& vt_iprange,
		char sep=(','));

	CPF_CLASS
		//和TransIPStringToVector进行相反的动作
		std::string TransIPRangeVectorToString(
		const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >& vt_iprange,
		char sep=(','));

	CPF_CLASS
		//如果读入有错误，返回false，返回false的时候，程序会尽量转换
		BOOL TransPortRangeStringToVector(const char * str_port,
		std::vector< std::pair<ACE_UINT16,ACE_UINT16> >& vt_port_range,
		char sep=(','));

	CPF_CLASS
		std::string TransPortRangeVectorToString(
		const std::vector< std::pair<ACE_UINT16,ACE_UINT16> >& vt_port_range,
		char sep=(','));


	CPF_CLASS
		void TransPortStringToVector(const char * str_port,
		std::vector< ACE_UINT16 >& vt_port_range,
		char sep=(','));

	CPF_CLASS
		std::string TransPortVectorToString(
		const std::vector< ACE_UINT16 >& vt_port_range,
		char sep=(','));


	CPF_CLASS
		void TransPort32StringToVector(const char * str_port,
		std::vector< ACE_UINT32 >& vt_port_range,
		char sep=(','));

	CPF_CLASS
		std::string TransPort32VectorToString(
		const std::vector< ACE_UINT32 >& vt_port_range,
		char sep=(','));


	// 在一个IP范围数组中，查找指定IP
	CPF_CLASS
	inline bool _find_ip_in_range_vector(ACE_UINT32 comp_ip, const VET_IPADDR_RANGE& ip_range)
	{
		for (size_t i=0; i<ip_range.size(); ++i)
		{
			if (ip_range[i].first <= comp_ip && comp_ip <= ip_range[i].second)
				return true;
		}
		return false;
	}

	//任何一个IP符合都返回
	CPF_CLASS
	inline bool _find_ip_in_range_vector(ACE_UINT32 ip1,ACE_UINT32 ip2, const VET_IPADDR_RANGE& ip_range)
	{
		for (size_t i=0; i<ip_range.size(); ++i)
		{
			if (ip_range[i].first <= ip1 && ip1 <= ip_range[i].second)
				return true;

			if (ip_range[i].first <= ip2 && ip2 <= ip_range[i].second)
				return true;
		}
		return false;
	}

	CPF_CLASS
	inline bool _find_mac_in_vector(const BYTE* comp_mac, const VET_MACADDR& mac_vet)
	{		
		for (size_t i=0; i<mac_vet.size(); ++i)
		{
			ACE_UINT64 mac_addr = mac_vet[i];
			if (	*(const ACE_UINT32*)&mac_addr == *(const ACE_UINT32*)comp_mac
				&&	((const ACE_UINT16*)&mac_addr)[2] == ((const ACE_UINT16*)comp_mac)[2]
			)
			{
				return true;
			}
		}
		return false;
	}

	// 在一个MAC数组中，查找指定mac，任何一个mac符合都返回
	CPF_CLASS
	inline bool _find_mac_in_vector(const BYTE* comp_mac1, const BYTE* comp_mac2, const VET_MACADDR& mac_vet)
	{		
		for (size_t i=0; i<mac_vet.size(); ++i)
		{
			ACE_UINT64 mac_addr = mac_vet[i];
			if (	*(const ACE_UINT32*)&mac_addr == *(const ACE_UINT32*)comp_mac1
				&&	((const ACE_UINT16*)&mac_addr)[2] == ((const ACE_UINT16*)comp_mac1)[2]
			)
			{
				return true;
			}
			if (	*(const ACE_UINT32*)&mac_addr == *(const ACE_UINT32*)comp_mac2
				&&	((const ACE_UINT16*)&mac_addr)[2] == ((const ACE_UINT16*)comp_mac2)[2]
			)
			{
				return true;
			}
		}
		return false;
	}

	// 在一个端口数组中，查找指定端口
	CPF_CLASS inline bool _find_port_in_vector(ACE_UINT16 comp_port, const VET_PORT_RANGE& port_vet)
	{
		for (size_t i=0; i<port_vet.size(); ++i)
		{
			if (port_vet[i].first <= comp_port && port_vet[i].second >= comp_port )
				return true;
		}
		return false;
	}

	CPF_CLASS inline BOOL _ip_in_mask(ACE_UINT32 ip, ACE_UINT32 dip, ACE_UINT32 dmask)
	{
		if ( (ip&dmask) == (dip&dmask) )
			return TRUE;
		else
			return FALSE;
	}

	CPF_CLASS inline BOOL _port_in_range(ACE_UINT16 port, const PORT_RANGE& port_range)
	{
		if ( (port_range.first<=port) && (port<=port_range.second) )
			return TRUE;
		else
			return FALSE;
	}

	CPF_CLASS
		int _delete_ip_in_range_vector(ACE_UINT32 comp_ip, BOOL bSplitIPRange,VET_IPADDR_RANGE& ip_range);

	CPF_CLASS	
		int _delete_mac_in_vector(const BYTE* comp_mac1, const BYTE* comp_mac2, VET_MACADDR& mac_vet);

}



namespace CIPRangeFunc
{
	CPF_CLASS inline void _ipmask_to_range(ACE_UINT32 hip, ACE_UINT32 hmask, OUT IPADDR_RANGE& hip_range)
	{
		hip_range.first = hip & hmask;
		hip_range.second = hip | (~hmask);

		BYTE* p = (BYTE*)&hip_range.first;
		if (*p == 0x0)
			*p = 0x1;

		p = (BYTE*)&hip_range.second;
		if (*p == 0xff)
			*p = 0xfe;
	}

	CPF_CLASS inline BOOL _ip_in_range(ACE_UINT32 ip, const IPADDR_RANGE& ip_range)
	{
		if ( (ip_range.first<=ip) && (ip<=ip_range.second) )
			return TRUE;
		else
			return FALSE;
	}

	//判断ip是否全部落在this范围地址内
	CPF_CLASS inline int IsInThis(const ACE_UINT32& ip,const IPADDR_RANGE& hip_range)
	{
		return hip_range.first <= ip && hip_range.second >= ip;
	}

	//判断other是否全部落在this范围地址内
	CPF_CLASS inline int IsInThis(const IPADDR_RANGE& other,const IPADDR_RANGE& hip_range)
	{
		return (IsInThis(other.first,hip_range)&&IsInThis(other.second,hip_range));
	}

	CPF_CLASS inline int IsInThis(const std::vector< IPADDR_RANGE >& vt_ip_range,const IPADDR_RANGE& hip_range)
	{
		for(size_t i = 0; i < vt_ip_range.size(); ++i)
		{
			if( !IsInThis(vt_ip_range[i],hip_range) )
				return 0;
		}

		return 1;
	}

	//注意比较大小，是比较两个互相不交叉的IP地址范围。如果地址交叉，则两个函数不能使用
	CPF_CLASS inline int IsLittler (const IPADDR_RANGE& hip_range1,const IPADDR_RANGE& hip_range2)
	{
		return hip_range1.second < hip_range2.first;
	}

	CPF_CLASS inline int IsBigger (const IPADDR_RANGE& hip_range1,const IPADDR_RANGE& hip_range2)
	{
		return hip_range1.first > hip_range2.second;
	}
}
