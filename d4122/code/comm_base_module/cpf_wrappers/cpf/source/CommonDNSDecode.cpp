//////////////////////////////////////////////////////////////////////////
//CommonDNSDecode.cpp


/*DNS Response */
#define	RESPONSE_POSITION 0X8000

//domain name length
#define DNS_DOMAIN_NAME_LENGTH 256

/* Offsets of fields in the DNS header. */
#define	DNS_ID			0
#define	DNS_FLAGS		2
#define	DNS_QUEST		4
#define	DNS_ANS			6
#define	DNS_AUTH		8
#define	DNS_ADD			10
#define	DNS_HDRLEN		12				/* Length of DNS header. */

/* type values  */
#define T_A             1               /* host address */
#define T_NS            2               /* authoritative name server */
#define T_MD            3               /* mail destination (obsolete) */
#define T_MF            4               /* mail forwarder (obsolete) */
#define T_CNAME         5               /* canonical name */
#define T_SOA           6               /* start of authority zone */
#define T_MB            7               /* mailbox domain name (experimental) */
#define T_MG            8               /* mail group member (experimental) */
#define T_MR            9               /* mail rename domain name (experimental) */
#define T_NULL          10              /* null RR (experimental) */
#define T_WKS           11              /* well known service */
#define T_PTR           12              /* domain name pointer */
#define T_HINFO         13              /* host information */
#define T_MINFO         14              /* mailbox or mail list information */
#define T_MX            15              /* mail routing information */
#define T_TXT           16              /* text strings */
#define T_RP            17              /* responsible person (RFC 1183) */
#define T_AFSDB         18              /* AFS data base location (RFC 1183) */
#define T_X25           19              /* X.25 address (RFC 1183) */
#define T_ISDN          20              /* ISDN address (RFC 1183) */
#define T_RT            21              /* route-through (RFC 1183) */
#define T_NSAP          22              /* OSI NSAP (RFC 1706) */
#define T_NSAP_PTR      23              /* PTR equivalent for OSI NSAP (RFC 1348 - obsolete) */
#define T_SIG           24              /* digital signature (RFC 2535) */
#define T_KEY           25              /* public key (RFC 2535) */
#define T_PX            26              /* pointer to X.400/RFC822 mapping info (RFC 1664) */
#define T_GPOS          27              /* geographical position (RFC 1712) */
#define T_AAAA          28              /* IPv6 address (RFC 1886) */
#define T_LOC           29              /* geographical location (RFC 1876) */
#define T_NXT           30              /* "next" name (RFC 2535) */
#define T_EID           31              /* ??? (Nimrod?) */
#define T_NIMLOC        32              /* ??? (Nimrod?) */
#define T_SRV           33              /* service location (RFC 2052) */
#define T_ATMA          34              /* ??? */
#define T_NAPTR         35              /* naming authority pointer (RFC 2168) */
#define	T_KX			36				/* Key Exchange (RFC 2230) */
#define	T_CERT			37				/* Certificate (RFC 2538) */
#define T_A6			38              /* IPv6 address with indirection (RFC 2874) */
#define T_DNAME         39              /* Non-terminal DNS name redirection (RFC 2672) */
#define T_OPT			41				/* OPT pseudo-RR (RFC 2671) */
#define T_DS            43				/* Delegation Signature(RFC 3658) */
#define T_IPSECKEY      45              /* draft-ietf-ipseckey-rr */
#define T_RRSIG         46              /* future RFC 2535bis */
#define T_NSEC          47              /* future RFC 2535bis */
#define T_DNSKEY        48              /* future RFC 2535bis */
#define T_TKEY			249				/* Transaction Key (RFC 2930) */
#define T_TSIG			250				/* Transaction Signature (RFC 2845) */
#define T_WINS			65281			/* Microsoft's WINS RR */
#define T_WINS_R		65282			/* Microsoft's WINS-R RR */

/* Class values */
#define C_IN			1				/* the Internet */
#define C_CS			2				/* CSNET (obsolete) */
#define C_CH			3				/* CHAOS */
#define C_HS			4				/* Hesiod */
#define	C_NONE			254				/* none */
#define	C_ANY			255				/* any */
#define C_FLUSH         (1<<15)         /* High bit is set for MDNS cache flush */

/* Bit fields in the flags */
#define F_RESPONSE      (1<<15)         /* packet is response */
#define F_OPCODE        (0xF<<11)       /* query opcode */
#define OPCODE_SHIFT	11
#define F_AUTHORITATIVE (1<<10)         /* response is authoritative */
#define F_TRUNCATED     (1<<9)          /* response is truncated */
#define F_RECDESIRED    (1<<8)          /* recursion desired */
#define F_RECAVAIL      (1<<7)          /* recursion available */
#define F_Z		(1<<6)		/* Z */
#define F_AUTHENTIC     (1<<5)          /* authentic data (RFC2535) */
#define F_CHECKDISABLE  (1<<4)          /* checking disabled (RFC2535) */
#define F_RCODE         (0xF<<0)        /* reply code */

/*
F_RCODE
0               No error condition

1               Format error - The name server was
unable to interpret the query.

2               Server failure - The name server was
unable to process this query due to a
problem with the name server.

3               Name Error - Meaningful only for
responses from an authoritative name
server, this code signifies that the
domain name referenced in the query does
not exist.

4               Not Implemented - The name server does
not support the requested kind of query.

5               Refused - The name server refuses to
perform the specified operation for
policy reasons.  For example, a name
server may not wish to provide the
information to the particular requester,
or a name server may not wish to perform
a particular operation (e.g., zone



Mockapetris                                                    [Page 27]

 RFC 1035        Domain Implementation and Specification    November 1987


 transfer) for particular data.

 6-15            Reserved for future use.

*/

#include "cpf/CommonDNSDecode.h"
#include <map>

CCommonDNSDecode::CCommonDNSDecode(void)
{
}

CCommonDNSDecode::~CCommonDNSDecode(void)
{
}

//如果返回0表示请求数据，vt_query_hostname有效，vt_name_ip无效
//如果返回1表示应答数据，vt_query_hostname无效，vt_name_ip有效
//-1表示错误，vt_query_hostname无效，vt_name_ip无效
int CCommonDNSDecode::DNSDecode_Info(IN const char * dns_data,IN size_t dns_len,
						  OUT std::vector<std::string>& vt_query_hostname,
						  OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip)
{
	DNS_PACKET_HEADER dns_header;

	const char * data_header = NULL;
	size_t data_len = 0;

	int type = DNSDecode_Header(dns_data,dns_len,dns_header,data_header,data_len);

	if( 0 == type )
	{
		if( 0 != DSNDecode_Req(dns_header,
			dns_data,dns_len,
			data_header,data_len,
			vt_query_hostname) )
		{
			return -1;
		}
	}
	else if( 1 == type )
	{
		if( 0 != DSNDecode_Rsp(dns_header,
			dns_data,dns_len,
			data_header,data_len,
			&vt_query_hostname,
			vt_name_ip) )
		{
			return -1;
		}
	}

	return type;

}


//如果返回0表示请求数据，如果返回1表示应答数据。
int CCommonDNSDecode::DNSDecode_Header(IN const char * dns_data,IN size_t dns_len,
									 OUT DNS_PACKET_HEADER& dns_header,
									 OUT const char *& data_header,
									 OUT size_t& data_len)
{

	if( dns_len < sizeof(DNS_PACKET_HEADER) )
		return -1;

	memcpy((void *)&dns_header,dns_data,sizeof(DNS_PACKET_HEADER));

	//通过数据头判断是请求数据还是响应数据，对请求数据不处理，只处理响应数据
	ntoh_DnsPacketHeader(dns_header);

	data_header = dns_data + sizeof(DNS_PACKET_HEADER);

	data_len = dns_len - sizeof(DNS_PACKET_HEADER);

	//保留位
	if( (dns_header.flags & 0x70) != 0 )
	{
		return -1;
	}

	//错误码
	if( (dns_header.flags & 0x0f) != 0 )
	{
		return -1;
	}

	if( RESPONSE_POSITION & (dns_header.flags) )
	{
		return 1;
	}

	return 0;
}

int CCommonDNSDecode::DSNDecode_Req(const DNS_PACKET_HEADER& dns_header,
						 //dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
						 const char * dns_data_header,size_t dns_data_len,
						 //query_data_header:query的头部数据指针,从query头部到整个数据尾部的数据长度.
						 const char * query_data_header,size_t query_datalen,
						 OUT std::vector<std::string>&  vt_query_hostname)
{
	//应答数据头部指针
	const char * rspinfo_data_header = NULL;
	//从应答数据头部指针开始到数据尾部的数据长度
	size_t	rspinfo_data_len = 0;

	if( 0 != Deocde_querypart(dns_data_header,dns_data_len,
		query_data_header,query_datalen,
		dns_header.quests,
		rspinfo_data_header, rspinfo_data_len,
		&vt_query_hostname) )
	{
		return -1;
	}

	return 0;
}


//返回0表示正确，-1表示错误，vt_hostname和vt_ip无效
int CCommonDNSDecode::DSNDecode_Rsp(const DNS_PACKET_HEADER& dns_header,
									const char * dns_data_header,size_t dns_data_len,
									const char * query_data_header,size_t query_data_len,
									OUT std::vector<std::string>*  vt_query_hostname,
									OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip)
{
	if( (dns_header.flags & F_RCODE) != 0 )
	{
		return -1;
	}	

	//应答数据头部指针
	const char * rspinfo_data_header = NULL;
	//从应答数据头部指针开始到数据尾部的数据长度
	size_t	rspinfo_data_len = 0;

	if( 0 != Deocde_querypart(
		dns_data_header,dns_data_len,
		query_data_header,query_data_len,
		dns_header.quests,
		rspinfo_data_header, rspinfo_data_len,
		vt_query_hostname) )
	{
		return -1;
	}

	if( rspinfo_data_header == NULL || rspinfo_data_len == 0 )
	{
		return -1;
	}

	if( 0 != Deocde_rsppart(
		dns_data_header,dns_data_len,
		rspinfo_data_header,rspinfo_data_len,
		dns_header.answers,
		vt_name_ip) )
	{
		return -1;
	}

	return 0;
}

int CCommonDNSDecode::Deocde_querypart(	//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
											  const char * dns_data_header,size_t dns_data_len,
											  //query_data_header:query的头部数据指针,从query头部到整个数据尾部的数据长度.
											  const char * query_data_header,size_t query_datalen,
											  size_t req_nums,
											  OUT const char *& rspinfo_data_header,
											  OUT size_t& rspinfo_len,
											  OUT std::vector<std::string> * pvt_query_hostname)
{
	int curlen = (int)query_datalen;

	const char * data = query_data_header;

	for(size_t i=0;i<req_nums;i++)
	{
		//数据丢包的情况下，数据包的实际长度要比协议显示的数据长度短
		if( curlen <= 0 )
		{
			break;
		}

		const char *nextpos = NULL;

		char buf_temp[1024] = {0};

		if( decode_name(dns_data_header,dns_data_len,data,curlen,buf_temp,sizeof(buf_temp),nextpos) <= 0 )
			return -1;

		if( pvt_query_hostname )
		{
			pvt_query_hostname->push_back(buf_temp);
		}

		curlen -= (int)(nextpos - data);

		data = nextpos;

		//ACE_UINT16 type = NBUF_TO_WORD(data);
		data += 2;
		curlen -= 2;

		//ACE_UINT16 classes = NBUF_TO_WORD(data);
		data += 2;
		curlen -= 2;

	}

	if( curlen > 0 )
	{
		rspinfo_data_header = data;

		rspinfo_len = query_datalen - (data-query_data_header);
	}
	else
	{
		rspinfo_data_header = NULL;
		rspinfo_len = 0;
	}

	return 0;
}


int CCommonDNSDecode::Deocde_rsppart(//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
											const char * dns_data_header,size_t dns_data_len,
											const char * rspinfo_data_header,size_t rsp_data_len,
											size_t rsp_nums,
											OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip)
{
	if (rsp_nums <= 0) 
		return -1;

	std::map<std::string,std::string> name_map;//一般的名字和正规的别名影射

	int curlen = (int)rsp_data_len;

	const char * data = rspinfo_data_header;

	for (size_t i=0;i<rsp_nums;i++)
	{
		//数据丢包的情况下，数据包的实际长度要比协议显示的数据长度短
		if(curlen <= 0 )
			break;

		char buf_temp[1024] = {0};

		const char *nextpos = NULL;

		if( decode_name(dns_data_header,dns_data_len,
			data,curlen,buf_temp,sizeof(buf_temp),nextpos) <= 0 )
		{
			return -1;
		}

		curlen -= (int)(nextpos - data);

		data = nextpos;

		ACE_UINT16 type = NBUF_TO_WORD(data);
		data += 2;
		curlen -= 2;

		ACE_UINT16 classes = NBUF_TO_WORD(data);
		data += 2;
		curlen -= 2;

		//ACE_UINT32 ttl = NBUF_TO_UINT(data);
		data += 4;
		curlen -= 4;

		if( curlen < 2 )
		{
			return -1;
		}

		ACE_UINT16 addrlen = NBUF_TO_WORD(data);
		data += 2;
		curlen -= 2;

		if( curlen < addrlen )
		{
			return -1;
		}

		if( classes != 0x0001 )
		{//不是 IN
			data += addrlen;
			curlen -= addrlen;

			continue;
		}

		if( type == 0x0001 )
		{//HOST NAME
			ACE_UINT32 ipaddr = *(ACE_UINT32 *)data;

			std::string strname = buf_temp;

			std::map<std::string,std::string>::iterator it = name_map.find(strname);

			int count = 0;
			
			while( it != name_map.end() )
			{
				strname = it->second;

				it = name_map.find(strname);

				//防止无穷递归
				if( ++count >= 10 )
					break;
			}

			vt_name_ip.push_back(std::make_pair(strname,ipaddr));
		}
		else if( type == 0x0005 )
		{//CNAME

			char aliasname[1024] = {0};

			const char *nextpos = NULL;

			if( decode_name(dns_data_header,dns_data_len,
				data,curlen,aliasname,sizeof(aliasname),nextpos) <= 0 )
			{
				return -1;
			}

			name_map[aliasname] = std::string(buf_temp);
		}

		data += addrlen;
		curlen -= addrlen;
	}

	return 0;

}

//域名由字母、数字和连字符组成，开头和结尾必须是字母或数字，最长不超过63个字符，而且不区分大小写。
//完整的域名总长度不超过255个字符。在实际使用中，每个域名的长度一般小于8个字符.

//如果返回值大于0，表示返回名字的长度，如果<=0表示错误
int CCommonDNSDecode::decode_name(//dns_data_header数据的头部指针,dns_data_len: dns的包的长度,不包含udp的头长度
								  const char * dns_data_header,size_t dns_data_len,
								  const char * data,size_t datalen,
								  char * buf,int buf_len,
								  const char*& next_pos)
{
	const char * pdata_end = data + datalen;

	int buf_offset = 0;

	while( buf_offset < buf_len && data < pdata_end )
	{
		if( ((*data)&0xc0) == 0xc0 )
		{
			ACE_UINT16 name_pointer = NBUF_TO_WORD(data);

			name_pointer &= 0x3fff;

			if( name_pointer >= dns_data_len )
			{
				return -1;
			}

			if( dns_data_header+name_pointer == data )
			{//无限递归了，在某个客户那里出现的情况

				return -1;
			}

			const char *nextpos = NULL;

			int name_len = decode_name(dns_data_header,dns_data_len,
				dns_data_header+name_pointer,dns_data_len-name_pointer,
				buf+buf_offset,buf_len-buf_offset,nextpos);

			if( name_len <= -1 )
			{
				return -1;
			}

			data += 2;
			
			if( data > pdata_end )
				return -1;

			buf_offset += name_len;

			next_pos = data;

			break;
		}
		else
		{
			size_t len = (size_t)(*(BYTE *)data);

			if( len > 63 )
				return -1;

			++data;

			if( data+len > pdata_end )
				return -1;

			if( len == 0 )
			{
				if( buf_offset > 0 )
				{
					*(buf+buf_offset-1) = 0;
				}

				next_pos = data;

				break;
			}

			if( (buf_offset + (int)len >= 260)
				|| (buf_offset + (int)len >= buf_len) )
			{
				return -1;
			}

			memcpy(buf+buf_offset,data,len);
			buf_offset += (int)len;
			*(buf+buf_offset) = '.';
			++buf_offset;

			data += len;
		}
	}

	if( !next_pos )
	{
		return -1;
	}

	return (int)buf_offset;
}


void CCommonDNSDecode::ntoh_DnsPacketHeader(DNS_PACKET_HEADER& DnsPacketHeader)
{
	DnsPacketHeader.id			=	ACE_NTOHS(DnsPacketHeader.id);			//2字节，标识，通过它客户端可以将DNS的请求与应答相匹配；
	DnsPacketHeader.flags		=	ACE_NTOHS(DnsPacketHeader.flags);		//标志：[QR | opcode | AA| TC| RD| RA | zero | rcode ]
	DnsPacketHeader.quests		=	ACE_NTOHS(DnsPacketHeader.quests);		//问题数目；
	DnsPacketHeader.answers		=	ACE_NTOHS(DnsPacketHeader.answers);		//资源记录数目；
	DnsPacketHeader.author		=	ACE_NTOHS(DnsPacketHeader.author) ;		//授权资源记录数目；
	DnsPacketHeader.addition	=	ACE_NTOHS(DnsPacketHeader.addition);	//额外资源记录数目；
}



size_t CCommonDNSDecode::construct_fake_rsp_packet(const char * dns_pkt_query,size_t query_pkt_len,
								char * dns_pkt_rsp,size_t rsp_pkt_len,ACE_UINT32 virtualip_netorder)
{
	DNS_PACKET_HEADER * header = (DNS_PACKET_HEADER *)dns_pkt_query;

	//应答数据头部指针
	const char * rspinfo_data_header_orig = NULL;
	//从应答数据头部指针开始到数据尾部的数据长度
	size_t	rspinfo_data_len_orig = 0;

	if( 0 != Deocde_querypart(dns_pkt_query,query_pkt_len,
		dns_pkt_query+sizeof(DNS_PACKET_HEADER),query_pkt_len-sizeof(DNS_PACKET_HEADER),
		ACE_NTOHS(header->quests),
		rspinfo_data_header_orig, rspinfo_data_len_orig,
		NULL) )
	{
		return -1;
	}

	if( rspinfo_data_header_orig == 0 )
	{
		rspinfo_data_header_orig = dns_pkt_query + query_pkt_len;
	}

	if( rsp_pkt_len < (size_t)(rspinfo_data_header_orig-dns_pkt_query) + sizeof(DNS_RSP) )
	{
		return -1;
	}

	if( (int)(rspinfo_data_header_orig-dns_pkt_query) > 0 )
	{
		memcpy(dns_pkt_rsp,dns_pkt_query,rspinfo_data_header_orig-dns_pkt_query);
	}

	header = (DNS_PACKET_HEADER *)dns_pkt_rsp;

	header->flags = ACE_HTONS(0X8180);

	header->quests = ACE_HTONS(1);
	header->answers = ACE_HTONS(1);
	header->author = ACE_HTONS(0);
	header->addition = ACE_HTONS(0);

	DNS_RSP * prsp = (DNS_RSP *)(dns_pkt_rsp + (rspinfo_data_header_orig - dns_pkt_query));

	prsp->name = ACE_HTONS(0XC00C);
	prsp->type = ACE_HTONS(0X0001);
	prsp->classes = ACE_HTONS(0X0001);
	prsp->ttl = ACE_HTONL(0X0FFFFFFF);
	prsp->len = ACE_HTONS(4);
	*(ACE_UINT32 *)(prsp->data) = virtualip_netorder;

	return rspinfo_data_header_orig - dns_pkt_query + sizeof(DNS_RSP)-1+4;
}

size_t CCommonDNSDecode::construct_error_rsp_packet(const char * dns_pkt_query,size_t query_pkt_len,
									  char * dns_pkt_rsp,size_t rsp_pkt_len,
									  ACE_UINT32 errorcode)
{
	DNS_PACKET_HEADER * header = (DNS_PACKET_HEADER *)dns_pkt_query;

	//应答数据头部指针
	const char * rspinfo_data_header_orig = NULL;
	//从应答数据头部指针开始到数据尾部的数据长度
	size_t	rspinfo_data_len_orig = 0;

	if( 0 != Deocde_querypart(dns_pkt_query,query_pkt_len,
		dns_pkt_query+sizeof(DNS_PACKET_HEADER),query_pkt_len-sizeof(DNS_PACKET_HEADER),
		ACE_NTOHS(header->quests),
		rspinfo_data_header_orig, rspinfo_data_len_orig,
		NULL) )
	{
		return -1;
	}

	if( rspinfo_data_header_orig == 0 )
	{
		rspinfo_data_header_orig = dns_pkt_query + query_pkt_len;
	}

	if( (int)rsp_pkt_len < rspinfo_data_header_orig-dns_pkt_query )
	{
		return -1;
	}

	if( (int)(rspinfo_data_header_orig-dns_pkt_query) > 0 )
	{
		memcpy(dns_pkt_rsp,dns_pkt_query,rspinfo_data_header_orig-dns_pkt_query);
	}

	header = (DNS_PACKET_HEADER *)dns_pkt_rsp;

	header->flags = 0X8180;

	header->flags &= ~F_RCODE;
	header->flags |= (errorcode&F_RCODE);

	header->flags = ACE_HTONS(header->flags);

	header->quests = ACE_HTONS(1);
	header->answers = ACE_HTONS(0);
	header->author = ACE_HTONS(0);
	header->addition = ACE_HTONS(0);

	return rspinfo_data_header_orig - dns_pkt_query;

}
