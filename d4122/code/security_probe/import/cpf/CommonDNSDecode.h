//////////////////////////////////////////////////////////////////////////
//CommonDNSDecode.h


#pragma once

#include "cpf/cpf.h"
#include <utility>

class CPF_CLASS CCommonDNSDecode
{
public:
	CCommonDNSDecode(void);
	~CCommonDNSDecode(void);

public:
#pragma pack(push)
#pragma pack(1)

	//DNS数据包的包头
	typedef struct dnsheader
	{
		ACE_UINT16 id;			//2字节，标识，通过它客户端可以将DNS的请求与应答相匹配；

		ACE_UINT16 flags;		//标志：[QR | opcode | AA| TC| RD| RA | zero | rcode ]
		//在16位的标志中：QR位判断是查询/响应报文，opcode区别查询类型，
		//AA判断是否为授权回答，TC判断是否可截断，RD判断是否期望递归查询，
		//RA判断是否为可用递归，zero必须为0，rcode为返回码字段。
		ACE_UINT16 quests;		//问题数目；

		ACE_UINT16 answers;		//资源记录数目；

		ACE_UINT16 author;		//授权资源记录数目；

		ACE_UINT16 addition;	//额外资源记录数目；

	}DNS_PACKET_HEADER;

	typedef struct tagDNS_RSP
	{
		ACE_UINT16 name;
		ACE_UINT16 classes;
		ACE_UINT16 type;
		ACE_UINT32 ttl;
		ACE_UINT16 len;
		char	data[1];

	}DNS_RSP;

#pragma pack(pop)

public:
	//如果返回0表示请求数据，vt_query_hostname有效，vt_name_ip无效
	//如果返回1表示应答数据，vt_query_hostname和vt_name_ip都有效
	//-1表示错误，vt_query_hostname无效，vt_name_ip无效
	static int DNSDecode_Info(IN const char * dns_data,IN size_t dns_len,
		OUT std::vector<std::string>&  vt_query_hostname,
		OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip);

	//如果返回0表示请求数据，如果返回1表示应答数据。
	//-1表示错误
	//dns_header:是返回的DNS包头格式
	//data_header:是dns头后面的数据，data_len是头后面数据的长度
	static int DNSDecode_Header(IN const char * dns_data,IN size_t dns_len,
		OUT DNS_PACKET_HEADER& dns_header,
		OUT const char *& data_header,
		 OUT size_t& data_len);

	//返回0表示正确，-1表示错误，
	static int DSNDecode_Req(const DNS_PACKET_HEADER& dns_header,
		//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
		const char * dns_data_header,size_t dns_data_len,
		//query_data_header:query的头部数据指针,从query头部到整个数据尾部的数据长度.
		const char * query_data_header,size_t query_datalen,
		OUT std::vector<std::string>&  vt_query_hostname);

	//返回0表示正确，-1表示错误，vt_hostname和vt_ip无效
	//返回的中vt_ip中的元素是主机字节序的IP地址
	static int DSNDecode_Rsp(const DNS_PACKET_HEADER& dns_header,
		//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
		const char * dns_data_header,size_t dns_data_len,
		//query_data_header:query的头部数据指针,从query头部到整个数据尾部的数据长度.
		const char * query_data_header,size_t query_datalen,
		OUT std::vector<std::string>*  vt_query_hostname,
		OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip);

	//-1表示错误或者空间不够
	//>0表示返回的包的长度
	static size_t construct_fake_rsp_packet(const char * dns_pkt_query,size_t query_pkt_len,
		char * dns_pkt_rsp,size_t rsp_pkt_len,
		ACE_UINT32 virtualip_netorder);

	//-1表示错误或者空间不够
	//>0表示返回的包的长度
	static size_t construct_error_rsp_packet(const char * dns_pkt_query,size_t query_pkt_len,
		char * dns_pkt_rsp,size_t rsp_pkt_len,
		//0-5,一般为3表示没有这个名字。
		ACE_UINT32 errorcode=0x03);

private:
	//解码应答中的请求部分
	static int Deocde_querypart(
		//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
		const char * dns_data_header,size_t dns_data_len,
		//query_data_header:query的头部数据指针,从query头部到整个数据尾部的数据长度.
		const char * query_data_header,size_t query_datalen,
		size_t req_nums,
		OUT const char *& rspinfo_data_header,
		OUT	size_t& rspinfo_len,
		OUT std::vector<std::string> * pvt_query_hostname = NULL);
	
	//解码应答中的应答部分
	//返回的中vt_ip中的元素是主机字节序的IP地址
	static int Deocde_rsppart(
		//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
		const char * dns_data_header,size_t dns_data_len,
		const char * rspinfo_header,size_t rsplen,
		size_t rsp_nums,
		OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip);

private:
	//如果返回值大于0，表示返回名字的长度，如果<=0表示错误
	static int decode_name(//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
		const char * dns_data_header,size_t dns_data_len,
		const char * data,size_t datalen,
		char * buf,int buf_len,
		const char*& next_pos);

private:
	static void ntoh_DnsPacketHeader(DNS_PACKET_HEADER& DnsPacketHeader);

};
