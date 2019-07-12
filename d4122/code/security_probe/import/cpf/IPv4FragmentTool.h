//////////////////////////////////////////////////////////////////////////
//IPv4FragmentTool.h

//将一个IP包进行分配，有可能被分配的包，本身也是分片包


//使用方法：

//CIPv4FragmentTool tool;
//int count = tool.DoFragment(ip_data,data_len);
//
//for(int i = 0; i < cout; ++i)
//{
//	const BYTE * & data;
//	int data_len = tool.getfragment(i,data);
//}

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/cpf.h"

class CPF_CLASS CIPv4FragmentTool
{
public:
	CIPv4FragmentTool(void);
	~CIPv4FragmentTool(void);

public:
	typedef struct tagFRAMENT_ITEM
	{
		BYTE	data[1600];
		int		data_len;

	}FRAMENT_ITEM;


private:
	FRAMENT_ITEM	m_items[64];
	int				m_item_nums;

public:
	//ip_data原始数据的指针，data_len原始数据的长度，max_fragment_len最大分配的长度，不能大于1600
	//返回个数
	int DoFragment_By_MaxIPLen(const BYTE * mac_data_header,const BYTE * ip_header_data,int ip_total_len,int max_ip_fragment_len);

	int DoFragment_By_FragmentNum(const BYTE * mac_data_header,const BYTE * ip_header_data,int ip_total_len,int fragment_num);

public:
	inline int GetFragment(int index,const BYTE *& data) const
	{
		data = m_items[index].data;

		return m_items[index].data_len;
	}

public:
	// frags为网络序
	static inline BOOL FRAG_DF(unsigned int frags)
	{
		return (frags & 0x40);
	}
	static inline BOOL FRAG_MF(unsigned int frags)
	{
		return (frags & 0x20);
	}
	static inline BOOL FRAG_OFF_ZERO(unsigned int frags)
	{
		return 0 == (ACE_NTOHS(frags) & 0x1FFF);
	}

	static inline BOOL fragment_is_not(unsigned int frags)
	{
		// 设置DF位或者（无MF位并offset为0）
		return FRAG_DF(frags)
			|| (!FRAG_MF(frags) && FRAG_OFF_ZERO(frags) );
	}

	static inline BOOL have_fragment(unsigned int frags)
	{
		return (!fragment_is_not(frags));
	}

	static inline BOOL fragment_is_first(unsigned int frags)
	{
		// 设置MF位并且offset为0
		return FRAG_MF(frags) && FRAG_OFF_ZERO(frags);
	}
	static inline BOOL fragment_is_continue(unsigned int frags)
	{
		// 设置MF位并且offset不为0
		return FRAG_MF(frags) && !FRAG_OFF_ZERO(frags);
	}
	static inline BOOL fragment_is_last(unsigned int frags)
	{
		// 无MF位 并无DF位 并且offset不为0
		return !FRAG_DF(frags) && !FRAG_MF(frags) && !FRAG_OFF_ZERO(frags);
	}

};



class CPF_CLASS CTCPFragmentTool
{
public:
	CTCPFragmentTool(void);
	~CTCPFragmentTool(void);

public:
	typedef struct tagFRAMENT_ITEM
	{
		BYTE	data[1600];
		int		data_len;

	}FRAMENT_ITEM;


private:
	FRAMENT_ITEM	m_items[64];
	int				m_item_nums;

public:
	//ip_data原始数据的指针，data_len原始数据的长度，max_fragment_len最大分配的长度，不能大于1600
	//返回个数
	//total_mac_len，max_mac_fragment_len不包括后面4个字节CRC
	int DoFragment_By_MacMaxFrame(const BYTE * mac_data_header,const BYTE * ip_header_data,const BYTE * trans_header,
		const BYTE * app_header,int total_mac_len,int max_mac_fragment_len);

	int DoFragment_By_AppMaxFrame(const BYTE * mac_data_header,const BYTE * ip_header_data,const BYTE * trans_header,
		const BYTE * app_header,int total_mac_len,int max_app_fragment_len);

	int DoFragment_By_FragmentNum(const BYTE * mac_data_header,const BYTE * ip_header_data,const BYTE * trans_header,
		const BYTE * app_header,int total_mac_len,int fragment_num);

public:
	inline int GetFragment(int index,const BYTE *& data) const
	{
		data = m_items[index].data;

		return m_items[index].data_len;
	}
};
