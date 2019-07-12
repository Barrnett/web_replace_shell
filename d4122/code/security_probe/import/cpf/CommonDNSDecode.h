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

	//DNS���ݰ��İ�ͷ
	typedef struct dnsheader
	{
		ACE_UINT16 id;			//2�ֽڣ���ʶ��ͨ�����ͻ��˿��Խ�DNS��������Ӧ����ƥ�䣻

		ACE_UINT16 flags;		//��־��[QR | opcode | AA| TC| RD| RA | zero | rcode ]
		//��16λ�ı�־�У�QRλ�ж��ǲ�ѯ/��Ӧ���ģ�opcode�����ѯ���ͣ�
		//AA�ж��Ƿ�Ϊ��Ȩ�ش�TC�ж��Ƿ�ɽضϣ�RD�ж��Ƿ������ݹ��ѯ��
		//RA�ж��Ƿ�Ϊ���õݹ飬zero����Ϊ0��rcodeΪ�������ֶΡ�
		ACE_UINT16 quests;		//������Ŀ��

		ACE_UINT16 answers;		//��Դ��¼��Ŀ��

		ACE_UINT16 author;		//��Ȩ��Դ��¼��Ŀ��

		ACE_UINT16 addition;	//������Դ��¼��Ŀ��

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
	//�������0��ʾ�������ݣ�vt_query_hostname��Ч��vt_name_ip��Ч
	//�������1��ʾӦ�����ݣ�vt_query_hostname��vt_name_ip����Ч
	//-1��ʾ����vt_query_hostname��Ч��vt_name_ip��Ч
	static int DNSDecode_Info(IN const char * dns_data,IN size_t dns_len,
		OUT std::vector<std::string>&  vt_query_hostname,
		OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip);

	//�������0��ʾ�������ݣ��������1��ʾӦ�����ݡ�
	//-1��ʾ����
	//dns_header:�Ƿ��ص�DNS��ͷ��ʽ
	//data_header:��dnsͷ��������ݣ�data_len��ͷ�������ݵĳ���
	static int DNSDecode_Header(IN const char * dns_data,IN size_t dns_len,
		OUT DNS_PACKET_HEADER& dns_header,
		OUT const char *& data_header,
		 OUT size_t& data_len);

	//����0��ʾ��ȷ��-1��ʾ����
	static int DSNDecode_Req(const DNS_PACKET_HEADER& dns_header,
		//dns_data_header���ݵ�ͷ��ָ��,dns_data_len: dns�İ��ĳ���,������udp��ͷ����
		const char * dns_data_header,size_t dns_data_len,
		//query_data_header:query��ͷ������ָ��,��queryͷ������������β�������ݳ���.
		const char * query_data_header,size_t query_datalen,
		OUT std::vector<std::string>&  vt_query_hostname);

	//����0��ʾ��ȷ��-1��ʾ����vt_hostname��vt_ip��Ч
	//���ص���vt_ip�е�Ԫ���������ֽ����IP��ַ
	static int DSNDecode_Rsp(const DNS_PACKET_HEADER& dns_header,
		//dns_data_header���ݵ�ͷ��ָ��,dns_data_len: dns�İ��ĳ���,������udp��ͷ����
		const char * dns_data_header,size_t dns_data_len,
		//query_data_header:query��ͷ������ָ��,��queryͷ������������β�������ݳ���.
		const char * query_data_header,size_t query_datalen,
		OUT std::vector<std::string>*  vt_query_hostname,
		OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip);

	//-1��ʾ������߿ռ䲻��
	//>0��ʾ���صİ��ĳ���
	static size_t construct_fake_rsp_packet(const char * dns_pkt_query,size_t query_pkt_len,
		char * dns_pkt_rsp,size_t rsp_pkt_len,
		ACE_UINT32 virtualip_netorder);

	//-1��ʾ������߿ռ䲻��
	//>0��ʾ���صİ��ĳ���
	static size_t construct_error_rsp_packet(const char * dns_pkt_query,size_t query_pkt_len,
		char * dns_pkt_rsp,size_t rsp_pkt_len,
		//0-5,һ��Ϊ3��ʾû��������֡�
		ACE_UINT32 errorcode=0x03);

private:
	//����Ӧ���е����󲿷�
	static int Deocde_querypart(
		//dns_data_header���ݵ�ͷ��ָ��,dns_data_len: dns�İ��ĳ���,������udp��ͷ����
		const char * dns_data_header,size_t dns_data_len,
		//query_data_header:query��ͷ������ָ��,��queryͷ������������β�������ݳ���.
		const char * query_data_header,size_t query_datalen,
		size_t req_nums,
		OUT const char *& rspinfo_data_header,
		OUT	size_t& rspinfo_len,
		OUT std::vector<std::string> * pvt_query_hostname = NULL);
	
	//����Ӧ���е�Ӧ�𲿷�
	//���ص���vt_ip�е�Ԫ���������ֽ����IP��ַ
	static int Deocde_rsppart(
		//dns_data_header���ݵ�ͷ��ָ��,dns_data_len: dns�İ��ĳ���,������udp��ͷ����
		const char * dns_data_header,size_t dns_data_len,
		const char * rspinfo_header,size_t rsplen,
		size_t rsp_nums,
		OUT std::vector< std::pair<std::string,ACE_UINT32> >& vt_name_ip);

private:
	//�������ֵ����0����ʾ�������ֵĳ��ȣ����<=0��ʾ����
	static int decode_name(//dns_data_header���ݵ�ͷ��ָ��,dns_data_len: dns�İ��ĳ���,������udp��ͷ����
		const char * dns_data_header,size_t dns_data_len,
		const char * data,size_t datalen,
		char * buf,int buf_len,
		const char*& next_pos);

private:
	static void ntoh_DnsPacketHeader(DNS_PACKET_HEADER& DnsPacketHeader);

};
