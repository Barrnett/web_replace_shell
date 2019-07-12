//////////////////////////////////////////////////////////////////////////
//SimpleFltCondition.h

#pragma once


#include "cpf/cpf.h"
#include "cpf/TimeStamp32.h"
#include "cpf/other_tools.h"
#include "cpf/addr_tools.h"
#include "simple_filter/simple_filter.h"

typedef enum
{
	dir_uncare = 0,
	addr1_to_addr2 = 1,
	addr2_to_addr1 = -1,

}FILTER_DIR;

typedef struct tagIf_FILTER
{
	tagIf_FILTER()
	{
		bNot = false;
		card.first = card.second = -1;
	}

	BOOL Compare(int cardno) const
	{
		if( card.first == -1 || card.second == -1 )
			return true;

		BOOL bIn = (cardno >= card.first && cardno <= card.second );

		return (bNot?(!bIn):bIn);
	}

	BOOL IsEmpty() const
	{
		return ( card.first == -1 || card.second == -1 );
	}

	BOOL	bNot;

	//first=from,second=to
	std::pair<int,int>	card;

}If_FILTER;


typedef struct tagTime_FILTER
{
	tagTime_FILTER()
	{
		bNot = false;

		from = CTimeStamp32::zero;
		to = CTimeStamp32::maxtime;

	}

	tagTime_FILTER& operator =(const tagTime_FILTER& other)
	{
		if( this != &other )
		{
			from = other.from;
			to = other.to;
		}

		return *this;
	}

	BOOL Compare(const CTimeStamp32& ts) const
	{
		BOOL bIn =(ts >= from && ts <= to);

		return (bNot?(!bIn):bIn);
	}

	BOOL IsEmpty() const
	{
		return ( from == CTimeStamp32::zero
			&& to == CTimeStamp32::maxtime );
	}


	BOOL			bNot;

	CTimeStamp32	from;
	CTimeStamp32	to;	

}Time_FILTER;

typedef struct tagMAC_FILTER
{
	tagMAC_FILTER()
	{
		bNot = false;
		dir = dir_uncare;

		memset(macaddr1,0x00,sizeof(macaddr1));
		memset(macaddr2,0x00,sizeof(macaddr2));

		upperproto = -1;
	}

	tagMAC_FILTER& operator =(const tagMAC_FILTER& other)
	{
		if( this != &other )
		{
			memcpy(this,&other,sizeof(*this));
		}

		return *this;
	}

	BOOL Compare(const BYTE * src_mac,const BYTE * dst_mac,int nProtoIndex) const
	{
		BOOL bIn = InnerCompare(src_mac,dst_mac,nProtoIndex);

		return (bNot?(!bIn):bIn);
	}

	BOOL InnerCompare(const BYTE * src_mac,const BYTE * dst_mac,int nProtoIndex) const
	{
		if(upperproto != -1 && nProtoIndex != upperproto )
			return false;

		if( dir == addr1_to_addr2 )
		{
			if( (*(ULONGLONG *)macaddr1 == 0 || CPF::IsMacAddrEqual(macaddr1, src_mac) )
				&& (*(ULONGLONG *)macaddr2 == 0 || CPF::IsMacAddrEqual(macaddr2,dst_mac) ) )
			{
				return true;
			}

		}
		else if( dir == addr2_to_addr1 )
		{
			if( (*(ULONGLONG *)macaddr1 == 0 || CPF::IsMacAddrEqual(macaddr1, dst_mac) )
				&& (*(ULONGLONG *)macaddr2 == 0 || CPF::IsMacAddrEqual(macaddr2,src_mac) ) )
			{
				return true;
			}
		}
		else
		{
			if( (*(ULONGLONG *)macaddr1 == 0 || CPF::IsMacAddrEqual(macaddr1, src_mac) )
				&& (*(ULONGLONG *)macaddr2 == 0 || CPF::IsMacAddrEqual(macaddr2,dst_mac) ) )
			{
				return true;
			}

			if( (*(ULONGLONG *)macaddr1 == 0 || CPF::IsMacAddrEqual(macaddr1, dst_mac) )
				&& (*(ULONGLONG *)macaddr2 == 0 || CPF::IsMacAddrEqual(macaddr2,src_mac) ) )
			{
				return true;
			}
		}

		return false;
	}

	BOOL		bNot;

	FILTER_DIR	dir;

	BYTE		macaddr1[8];//0��ʾ��Ч
	BYTE		macaddr2[8];//0��ʾ��Ч
	int			upperproto;//-1��ʾ����ϵ

}MAC_FILTER;

typedef struct tagIP_FILTER
{
	tagIP_FILTER()
	{
		bNot = false;

		dir = dir_uncare;

		ip1 = ip2 = 0;

		upperproto = -1;
	} 

	tagIP_FILTER& operator =(const tagIP_FILTER& other)
	{
		if( this != &other )
		{
			memcpy(this,&other,sizeof(*this));
		}

		return *this;
	}

	bool Compare(ACE_UINT32 ipSrc,ACE_UINT32 ipDst, int nProtoIndex ) const
	{
		BOOL bIn = InnerCompare(ipSrc,ipDst,nProtoIndex);

		return (bNot?(!bIn):bIn);
	}

	bool InnerCompare(ACE_UINT32 ipSrc,ACE_UINT32 ipDst, int nProtoIndex ) const
	{
		if(upperproto != -1 && nProtoIndex != upperproto )
			return false;

		if( dir == addr1_to_addr2 )
		{
			if( (ip1 == 0 || ip1 == ipSrc) 
				&& (ip2 == 0 || ip2 == ipDst) )
			{
				return true;
			}
		}
		else if( dir == addr2_to_addr1 )
		{
			if( (ip1 == 0 || ip1 == ipDst)
				&& (ip2 == 0 || ip2 == ipSrc) )
			{
				return true;
			}
		}
		else
		{
			if( (ip1 == 0 || ip1 == ipSrc)
				&& (ip2 == 0 || ip2 == ipDst) )
			{
				return true;
			}

			if( (ip1 == 0 || ip1 == ipDst)
				&& (ip2 == 0 || ip2 == ipSrc) )
			{
				return true;
			}	
		}

		return false;
	}

	BOOL		bNot;

	FILTER_DIR	dir;

	ACE_UINT32	ip1;//0��ʾ��Ч,�����ֽ���
	ACE_UINT32	ip2;//0��ʾ��Ч,�����ֽ���
	int			upperproto;//-1��ʾ����ϵ

}IP_FILTER;

typedef struct tagPORT_FILTER
{
	tagPORT_FILTER()
	{
		bNot = false;
		dir = dir_uncare;

		port1 = port2 = 0;
	}  

	tagPORT_FILTER& operator =(const tagPORT_FILTER& other)
	{
		if( this != &other )
		{
			memcpy(this,&other,sizeof(*this));
		}

		return *this;
	}


	bool Compare(ACE_UINT16 portsrc,ACE_UINT16 portdst) const
	{
		BOOL bIn = InnerCompare(portsrc,portdst);

		return (bNot?(!bIn):bIn);
	}


	bool InnerCompare(ACE_UINT16 portsrc,ACE_UINT16 portdst) const
	{
		if( dir == addr1_to_addr2 )
		{
			if( (port1 == 0 || port1 == portsrc)
				&& (port2 == 0 || port2 == portdst) )
			{
				return true;
			}
		}
		else if( dir == addr2_to_addr1 )
		{
			if( (port1 == 0 || port1 == portdst)
				&& (port2 == 0 || port2 == portsrc) )
			{
				return true;
			}
		}
		else
		{
			if( (port1 == 0 || port1 == portsrc)
				&& (port2 == 0 || port2 == portdst) )
			{
				return true;
			}

			if( (port1 == 0 || port1 == portdst)
				&& (port2 == 0 || port2 == portsrc) )
			{
				return true;
			}
		}

		return false;
	}

	BOOL		bNot;


	FILTER_DIR	dir;

	ACE_UINT16	port1;//0��ʾ��Ч,�����ֽ���
	ACE_UINT16	port2;//0��ʾ��Ч,�����ֽ���

}PORT_FILTER;

typedef struct tagDATAPAT_FILTER
{
	tagDATAPAT_FILTER()
	{
		bNot = false;
		offset = 0;
		type = 0;
		patlen = 0;
	}

	tagDATAPAT_FILTER& operator =(const tagDATAPAT_FILTER& other)
	{
		if( this != &other )
		{
			memcpy(this,&other,sizeof(*this));
		}

		return *this;
	}

	bool Compare(void * pdata,int datalen) const
	{
		BOOL bIn = InnerCompare(pdata,datalen);

		return (bNot?(!bIn):bIn);
	}


	bool InnerCompare(void * pdata,int datalen) const
	{
		if( datalen < offset + patlen )
			return false;

		if( 1 == type )
		{
			if( 0 == memcmp((char *)pdata+offset,pat,patlen) )
			{
				return true;
			}
		}
		else if( 0 == type )
		{
			if( CPF::memstr((BYTE *)pdata+offset,datalen-offset,pat,patlen) )
			{
				return true;
			}
		}
		else
		{
			ACE_ASSERT(false);
		}

		return false;

	}

	BOOL bNot;

	int offset;
	int type;//0��ʾ��offset֮��ģ��ƥ�䣬1��ʾ��offset֮��ȷƥ��

	int patlen;
	BYTE pat[256];

}DATAPAT_FILTER;

typedef struct  tagONE_FILTER
{
	tagONE_FILTER()
	{
		bNot = false;
	}

	tagONE_FILTER& operator =(const tagONE_FILTER& other)
	{
		if( this != &other )
		{
			bNot = other.bNot;

			if_filter = other.if_filter;
			vt_time = other.vt_time;

			vt_mac = other.vt_mac;
			vt_ip = other.vt_ip;
			vt_port = other.vt_port;
			vt_data_pat = other.vt_data_pat;
		}

		return *this;
	}

	BOOL IsEmpty() const
	{
		return ( if_filter.IsEmpty()
			&& vt_time.empty()
			&& vt_mac.empty()
			&& vt_ip.empty()
			&& vt_port.empty()
			&& vt_data_pat.empty());
	}

	BOOL	bNot;
	//mac,ip������port���������ϵ

	If_FILTER	if_filter;

	std::vector<Time_FILTER>	vt_time;

	//mac�Ĺ���������vect�еĸ�����Ա�ǻ��ϵ
	std::vector<MAC_FILTER>		vt_mac;

	//ip�Ĺ���������vect�еĸ�����Ա�ǻ��ϵ
	std::vector<IP_FILTER>		vt_ip;

	//port�Ĺ���������vect�еĸ�����Ա�ǻ��ϵ
	std::vector<PORT_FILTER>	vt_port;

	//����ģʽƥ��Ĺ���������vect�еĸ�����Ա�ǻ��ϵ
	std::vector<DATAPAT_FILTER>	vt_data_pat;

}ONE_FILTER;	

class SIMPLE_FILTER_CLASS CSimpleFltCondition
{
public:
	CSimpleFltCondition()
	{
		m_bCardFilter = false;
		m_bIPFilter = false;
		m_bPortFilter = false;
	}

	~CSimpleFltCondition()
	{

	}

	void cal_flt();

	BOOL IsEmpty() const;

	BOOL IsHaveCardFilter () const
	{
		return m_bCardFilter;
	}

	BOOL IsHaveIPFilter() const
	{
		return m_bIPFilter;
	}

	BOOL IsHavePortFilter() const
	{	
		return m_bPortFilter;
	}


public:
	//������������ǻ��ϵ
	std::vector<ONE_FILTER>	m_vt_filter;

private:
	BOOL	m_bCardFilter;
	BOOL	m_bIPFilter;//ip�Ƿ��й�����
	BOOL	m_bPortFilter;//�˿��Ƿ��й�����


};


