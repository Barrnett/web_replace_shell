//////////////////////////////////////////////////////////////////////////
//IPRangeTable2.h
//���ö��ַ���IP��ַ�ν�������Ͳ��ҡ�

//�����ʾҪ��һ���Է����㹻�ĸ�����ֻ�ܰ��մ�С�����˳�����ӣ��������м����ǰ���������
//�����Ԫ���ǣ���ʼ��ַ�ͽ�����ַ��
#pragma once

#include "cpf/ostypedef.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4786)

template<typename T>
class CIPRangeTable2
{
public:
	struct IP_RANGE_TYPE
	{
		IP_RANGE_TYPE()
		{	iplower = ipupper = 0;	}

		IP_RANGE_TYPE(ACE_UINT32 ip)
		{	iplower = ipupper = ip;	}

		IP_RANGE_TYPE(ACE_UINT32 ip1,ACE_UINT32 ip2)
		{	iplower = ip1;		ipupper = ip2;	}

		inline int operator < (const IP_RANGE_TYPE& other) const
		{
			return ipupper < other.iplower;
		}

		inline int compare(ACE_UINT32 ip) const
		{
			if( iplower > ip  )
				return 1;
			else if( ipupper < ip )
				return -1;

			return 0;
		}
		ACE_UINT32 iplower;//host_order
		ACE_UINT32 ipupper;//host_order

		T			data;
	};

public:
	CIPRangeTable2()
	{
		m_nCurNums = 0;
		m_nMaxNums = 0;
		m_ipTable = NULL;
	}

	~CIPRangeTable2()
	{
		destroy();
	}

public:
	//Ԥ���ռ�
	void init(size_t num)
	{
		if(m_ipTable)
		{
			delete []m_ipTable;
			m_ipTable = NULL;
		}

		m_nCurNums = 0;
		m_nMaxNums = (int)num;

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
			delete []m_ipTable;
			m_ipTable = NULL;
		}

		m_nCurNums = 0;
		m_nMaxNums = 0;
	}

	//����û��ܹ���֤�����˳���մ�С�������룬����ʹ������������ӿ�����
	BOOL Push_Back(ACE_UINT32 hostorder_ip1,ACE_UINT32 hostorder_ip2,const T& t)
	{

#ifdef _DEBUG
		ACE_ASSERT( hostorder_ip2 >= hostorder_ip1 );
#endif

		IP_RANGE_TYPE range(hostorder_ip1,hostorder_ip2);
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
			int rtn = m_ipTable[i].compare(hostorder_ipaddr);

			if( rtn == 0 )
			{
				return &m_ipTable[i].data;
			}
			else if( rtn > 0 )
			{//����Ҫ�������ˣ���Ϊ������ģ��������ֵ�ȵ�ǰ�Ļ�ҪС���Ǳ��Ժ�Ľ���С
				return NULL;
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

