//////////////////////////////////////////////////////////////////////////
//IPv4FragmentTool.h

//��һ��IP�����з��䣬�п��ܱ�����İ�������Ҳ�Ƿ�Ƭ��


//ʹ�÷�����

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
	//ip_dataԭʼ���ݵ�ָ�룬data_lenԭʼ���ݵĳ��ȣ�max_fragment_len������ĳ��ȣ����ܴ���1600
	//���ظ���
	int DoFragment_By_MaxIPLen(const BYTE * mac_data_header,const BYTE * ip_header_data,int ip_total_len,int max_ip_fragment_len);

	int DoFragment_By_FragmentNum(const BYTE * mac_data_header,const BYTE * ip_header_data,int ip_total_len,int fragment_num);

public:
	inline int GetFragment(int index,const BYTE *& data) const
	{
		data = m_items[index].data;

		return m_items[index].data_len;
	}

public:
	// fragsΪ������
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
		// ����DFλ���ߣ���MFλ��offsetΪ0��
		return FRAG_DF(frags)
			|| (!FRAG_MF(frags) && FRAG_OFF_ZERO(frags) );
	}

	static inline BOOL have_fragment(unsigned int frags)
	{
		return (!fragment_is_not(frags));
	}

	static inline BOOL fragment_is_first(unsigned int frags)
	{
		// ����MFλ����offsetΪ0
		return FRAG_MF(frags) && FRAG_OFF_ZERO(frags);
	}
	static inline BOOL fragment_is_continue(unsigned int frags)
	{
		// ����MFλ����offset��Ϊ0
		return FRAG_MF(frags) && !FRAG_OFF_ZERO(frags);
	}
	static inline BOOL fragment_is_last(unsigned int frags)
	{
		// ��MFλ ����DFλ ����offset��Ϊ0
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
	//ip_dataԭʼ���ݵ�ָ�룬data_lenԭʼ���ݵĳ��ȣ�max_fragment_len������ĳ��ȣ����ܴ���1600
	//���ظ���
	//total_mac_len��max_mac_fragment_len����������4���ֽ�CRC
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
