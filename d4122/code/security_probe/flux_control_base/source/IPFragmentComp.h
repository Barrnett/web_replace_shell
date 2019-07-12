//////////////////////////////////////////////////////////////////////////
//IPFragmentComp.h 

//进行IP分片包的组合


#pragma once

#include "cpf/cpf.h"
#include "cpf/THashFinderEx.h"
#include "tcpip_mgr_common_init.h"


class CIPFragmentComp
{
public:
	CIPFragmentComp(void);
	~CIPFragmentComp(void);

public:
	void init(int max_item);
	void close();

public:
	//如果有返回，true，继续分片组合返回false
	BOOL HandleFragment(PACKET_INFO_EX& packet_info);

	void CheckTimetout(ACE_UINT32 cur_time);

private:

	enum{
		MAX_BUFFER_LEN=4096*2,//最大的缓存区大小
	MAX_FRAGMENT_NUM=10//一个IP包最多分成10个小包
	};

private:

	typedef struct  
	{
		ACE_UINT16 begin;
		ACE_UINT16 end;

	} ONE_FRAGMENT;

	typedef struct {

		ACE_UINT32 srcip;
		ACE_UINT32 dstip;
		ACE_UINT16 id;

	}IP_FRAGMENT_KEY;

	class IP_FRAGMENT_ITEM
	{
	public:
		IP_FRAGMENT_ITEM()
		{
			memset(this, 0, sizeof(*this));

			remain_fragment_nums = 1;

			fragments[0].begin = 0;
			fragments[0].end = MAX_BUFFER_LEN;
		}

		void init()
		{
			srcip = 0;
			dstip = 0;
			id = 0;
		}

		ACE_UINT32 srcip;
		ACE_UINT32 dstip;

		ACE_UINT16 id;

		BYTE	*	buffer;
		int			trans_offset;
		int			ip_offset;
		int			pppoe_offset;//如果不是pppoe的数据，pppoe_offset==0


		ACE_UINT32  last_time;

		ACE_UINT32	total_trans_pkt_len;//传输层的总长度

		ONE_FRAGMENT	fragments[MAX_FRAGMENT_NUM];//没有完成的分片
		int				remain_fragment_nums;//剩余的分片数

		inline void delete_fragment(int index)
		{
			for(int i = index; i < remain_fragment_nums-1; ++i )
			{
				fragments[i] = fragments[i+1];
			}

			--remain_fragment_nums;

			return;
		}

		inline BOOL insert_fragment(int index,ACE_UINT16 begin,ACE_UINT16 end)//在index的位置插入
		{
			if( remain_fragment_nums >= MAX_FRAGMENT_NUM )
			{//已经超过了分片总数

				return false;
			}

			for(int i = remain_fragment_nums; i > index; --i )
			{
				fragments[i] = fragments[i-1];
			}

			fragments[index].begin = begin;
			fragments[index].end = end;

			++remain_fragment_nums;

			return true;
		}
	};


	class CNATHashCompFun
	{
	public:
		inline int Hash(const IP_FRAGMENT_KEY& key) const
		{
			ACE_UINT32	hash = (key.srcip^key.dstip^key.id);

			ACE_UINT16* tmp = (ACE_UINT16*)&hash;
			hash = tmp[0] ^ tmp[1];
			return (int)(hash);
		}

		inline int Cmp(const IP_FRAGMENT_ITEM& value, const IP_FRAGMENT_KEY& key) const
		{
			return (	value.id == key.id
				&& value.srcip == key.srcip 
				&& value.dstip == key.dstip );

		}
	};

private:
	CTHashFinderEx<IP_FRAGMENT_ITEM, IP_FRAGMENT_KEY, CNATHashCompFun> m_ipfrag_finder;

private:
	BOOL HandleFragment_Item(IP_FRAGMENT_ITEM* pItem,PACKET_INFO_EX& packet_info);

private:
	ACE_UINT32	m_last_checktime;

};
