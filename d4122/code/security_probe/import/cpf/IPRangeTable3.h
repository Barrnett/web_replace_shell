//////////////////////////////////////////////////////////////////////////
//IPRangeTable3.h
//���ö��ַ���IP��ַ�ν�������Ͳ��ҡ�

//�����ʾҪ��һ���Է����㹻�ĸ�����ֻ�ܰ��մ�С�����˳�����ӣ��������м����ǰ���������
//�����Ԫ���ǣ���ʼ��ַ�������룩
#pragma once

#include "cpf/ostypedef.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4786)

template<typename T>
class CIPRangeTable3
{
public:
	struct IP_RANGE_TYPE
	{
		IP_RANGE_TYPE()
		{	ipaddr = ipmask = 0;	}

		IP_RANGE_TYPE(ACE_UINT32 input_ip)
		{	ipaddr = input_ip; ipmask = ((ACE_UINT32)-1);	}

		IP_RANGE_TYPE(ACE_UINT32 input_ip,ACE_UINT32 themask)
		{	ipaddr = (input_ip&themask);		ipmask = themask;	}

		inline int operator < (const IP_RANGE_TYPE& other) const
		{
			return ipaddr < other.ipaddr;
		}

		inline int compare(ACE_UINT32 input_ip) const
		{
			input_ip = (input_ip&ipmask);

			if( ipaddr < input_ip )
				return -1;
			if( ipaddr > input_ip  )
				return 1;

			return 0;
		}

		ACE_UINT32 ipaddr;//host_order
		ACE_UINT32 ipmask;//host_order

		T			data;
	};

public:
	CIPRangeTable3()
	{
		m_nCurNums = 0;
		m_nMaxNums = 0;
		m_ipTable = NULL;
	}

	~CIPRangeTable3()
	{
		destroy();
	}

public:
	//Ԥ���ռ�
	void init(size_t num)
	{
		if( m_ipTable )
		{
			delete []m_ipTable;
			m_ipTable = NULL;
		}
		m_nCurNums = 0;
		m_nMaxNums = num;

		if( num > 0 )
		{
			m_ipTable = new IP_RANGE_TYPE[num];
		}
	}

	void clear()
	{//���ͷſռ�

		m_nCurNums = 0;
	}

	void destroy()
	{
		if( m_ipTable )
		{
			delete[] m_ipTable;
			m_ipTable = NULL;
		}

		m_nCurNums = 0;
		m_nMaxNums = 0;
	}

	//����û��ܹ���֤�����˳���մ�С�������룬����ʹ������������ӿ�����
	BOOL Push_Back(ACE_UINT32 hostorder_ip,ACE_UINT32 hostorder_mask,const T& t)
	{
		IP_RANGE_TYPE range(hostorder_ip,hostorder_mask);
		range.data = t;

		return Push_Back(range);
	}

	BOOL Push_Back(const IP_RANGE_TYPE& ipRangeType)
	{
		if( m_nCurNums >= m_nMaxNums )
			return false;

#ifdef _DEBUG
		ACE_ASSERT( m_nCurNums == 0 || m_ipTable[m_nCurNums-1] < ipRangeType );
#endif

		m_ipTable[m_nCurNums++] = ipRangeType;

		return true;
	}

	//���ipaddr�ҵ�������true,����type��ָ��������
	inline const T * Find_Binary(ACE_UINT32 hostorder_ipaddr) const
	{
		int lower_index = 0;
		int upper_index = m_nCurNums-1;

		while( upper_index >= lower_index )
		{
			int cur_index = ((lower_index + upper_index)>>1);//�۰�

			int rtn = m_ipTable[cur_index].compare(hostorder_ipaddr);

			if( rtn == 0 )
			{
				return &m_ipTable[cur_index].data;
				break;
			}

			if( rtn > 0 )
			{
				upper_index = cur_index-1;
			}
			else
			{
				lower_index = cur_index+1;
			}
		}

		return NULL;
	}

	//˳�����
	inline const T * Find_Order(ACE_UINT32 hostorder_ipaddr) const
	{
		for(int i = 0; i < m_nCurNums; ++i)
		{
			if( m_ipTable[i].compare(hostorder_ipaddr) == 0 )
			{
				return &m_ipTable[i].data;
			}
		}

		return NULL;
	}

public:
	const IP_RANGE_TYPE * GetIPTable() const
	{
		return m_ipTable;
	}

	inline int GetMaxNums() const
	{
		return m_nMaxNums;
	}

	inline int GetCurNums() const
	{
		return m_nCurNums;
	}

private:
	int				m_nCurNums;
	int				m_nMaxNums;
	IP_RANGE_TYPE *	m_ipTable;

};

