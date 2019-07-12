//////////////////////////////////////////////////////////////////////////
//IF_IPADDR_EX.h

#include "flux_control_common.h"
#include "FCNetIfConfig_base.h"
#include "cpf/TinyXmlEx.h"

enum{MAX_SUB_IF_NUM = 8};

class FLUX_CONTROL_COMMON_CLASS	SUB_IF_IPADDR
{
public:
	SUB_IF_IPADDR();
	~SUB_IF_IPADDR();

	void close();

public:
	SUB_IF_IPADDR& operator = (const SUB_IF_IPADDR& other);
	int operator == (const SUB_IF_IPADDR& other) const;
	int operator != (const SUB_IF_IPADDR& other) const;

	BOOL IsCfgAddrConfigModified(const SUB_IF_IPADDR& other) const;

public:
	inline BOOL IsInSameSubNet(ACE_UINT32 in_ipaddr) const
	{//和网口的地址是不是同一个网段

		return ( (ipaddr&mask)==(in_ipaddr&mask) );
	}

public:
	BOOL save(CTinyXmlEx& xml_writter,TiXmlNode * ip_addr_ex_node) const;
	BOOL load(CTinyXmlEx& xml_writter,TiXmlNode * ip_addr_ex_node);

public:
	ACE_UINT32	ipaddr;
	ACE_UINT32  mask;
	ACE_UINT32  gateway;
	ACE_UINT64	gateway_mac;//关于防止ARP欺骗，一般是0

	ACE_UINT64	clone_mac;//一般是0
};

class FLUX_CONTROL_COMMON_CLASS IF_IPADDR_EX
{
public:
	IF_IPADDR_EX();
	~IF_IPADDR_EX();

public:
	IF_IPADDR_EX& operator = (const IF_IPADDR_EX& other);
	int operator == (const IF_IPADDR_EX& other) const;
	int operator != (const IF_IPADDR_EX& other) const;

	BOOL IsCfgAddrConfigModified(const IF_IPADDR_EX& other) const;

public:
	void close()
	{
		for(size_t i = 0; i < sizeof(vt_sub_addr)/sizeof(vt_sub_addr[0]); ++i)
		{
			vt_sub_addr[i].close();
		}

		sub_if_nums = 0;

		router_table.clear();
	}

public:
	SUB_IF_IPADDR	vt_sub_addr[MAX_SUB_IF_NUM];
	int				sub_if_nums;

public:
	std::vector<CStaticRouterTableItem>		router_table;//路由表，如果为空，表示不需要路由，是本地地址

public:
	//看在哪个子接口的网段中
	inline int FindSubIf_seg(ACE_UINT32 in_ipaddr) const
	{
		for(int i = 0; i < sub_if_nums; ++i)
		{
			if( vt_sub_addr[i].IsInSameSubNet(in_ipaddr) )
			{
				return i;
			}
		}

		return -1;
	}

	//看是哪个子接口的IP
	inline int FindSubIf_addr(ACE_UINT32 in_ipaddr) const
	{
		for(int i = 0; i < sub_if_nums; ++i)
		{
			if( vt_sub_addr[i].ipaddr == in_ipaddr )
			{
				return i;
			}
		}

		return -1;
	}

	//看是否属于这个接口管理的范围
	inline int FindSubIf_dst(ACE_UINT32 in_ipaddr) const
	{
		//先判断是否在网卡本身的地址网段
		int sub_index = FindSubIf_seg(in_ipaddr);

		if( sub_index == -1 )
		{			
			//是否在路由表中
			for(size_t i = 0; i < router_table.size(); ++i)
			{
				if( router_table[i].match(in_ipaddr) )
				{
					sub_index = FindSubIf_seg(router_table[i].next_hop_ip);
					break;;
				}
			}
		}

		return sub_index;
	}


	inline BOOL GetNextHopIP(ACE_UINT32 dest_ip,ACE_UINT32& nexthop_ip,ACE_UINT64& next_hop_mac) const
	{
		if( FindSubIf_seg(dest_ip) != -1 )
		{
			nexthop_ip = dest_ip;
			next_hop_mac = 0;
			return true;
		}
		else 
		{
			if( router_table.size() == 0 )
			{
				nexthop_ip = vt_sub_addr[0].gateway;
				next_hop_mac = vt_sub_addr[0].gateway_mac;

				return true;
			}
			else
			{
				for(size_t i = 0; i < router_table.size(); ++i)
				{
					if( router_table[i].match(dest_ip) )
					{
						nexthop_ip = router_table[i].next_hop_ip;
						next_hop_mac = router_table[i].next_hop_mac;
						return true;
					}
				}
			}			
		}

		return false;
	}


public:
	inline ACE_UINT32 GetIP(int sub_index) const
	{
		return vt_sub_addr[sub_index].ipaddr;
	}

	inline ACE_UINT32 GetMask(int sub_index) const
	{
		return vt_sub_addr[sub_index].mask;
	}

	inline ACE_UINT32 GetGateWay(int sub_index) const
	{
		return vt_sub_addr[sub_index].gateway;
	}

	inline ACE_UINT64 GetGateWayMac(int sub_index) const
	{
		return vt_sub_addr[sub_index].gateway_mac;
	}

	inline ACE_UINT64 GetCloneMac(int sub_index) const
	{
		return vt_sub_addr[sub_index].clone_mac;
	}


};
