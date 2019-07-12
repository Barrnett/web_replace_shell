//////////////////////////////////////////////////////////////////////////
//HostIPMacFinder.h

#pragma once

#include "cpf/THashFinderEx.h"
#include "cpf/CPF_TimerFileOutputMgr.h"

//以IP为关键字，用于记录白名单ip地址和MAC地址的对应关系，并且输出
class CHostIPMacFinder
{
public:
	CHostIPMacFinder(void);
	~CHostIPMacFinder(void);

public:
	BOOL Create(int max_nums);

	void ComeAPacket(const BYTE * macaddr,ACE_UINT32 hostorder_ip,int cur_time);

public:
	void HandleFirstPacket(const char * pTempPath,const char * pPath,
		int interval,const CTimeStamp32& stamp);

	void SaveStatic(const CTimeStamp32& curTs);

	void OnPacketTimer(const CTimeStamp32& stamp);

public:
	typedef struct tagInnerValue
	{
		ACE_UINT64		macaddr;
		ACE_UINT32		hostorder_ip;
		int				update_time;//最近更新时间，如果ip地址对应的mac发生了变化，则记录变化的时间
		int				last_save_time;//被保存的时间
		int				last_active_time;//最近活动时间

		tagInnerValue() {
			macaddr = 0;
			hostorder_ip = 0;
			update_time = 0;
			last_save_time = 0;
			last_active_time = 0;
		}
		~tagInnerValue() 
		{
			macaddr = 0;
			hostorder_ip = 0;
			update_time = 0;
			last_save_time = 0;
			last_active_time = 0;
		}
	}InnerValue, *PInnerValue;

	class CompFun
	{
	public:
		int Hash(ACE_UINT32 hostorder_ip) const {
			return hostorder_ip;
		}
		int Cmp(const tagInnerValue& value, ACE_UINT32 hostorder_ip) const {
			return value.hostorder_ip==hostorder_ip;
		}
	};

	typedef CTHashFinderEx<tagInnerValue, ACE_UINT32, CompFun> IP_Mac_Finder;


private:
	IP_Mac_Finder	m_ip_mac_finder;

private:
	CPF_TimerFileOutputMgr		m_StaticOutputMgr;

private:
	static void S_SaveStatic(const CTimeStamp32& curTs, void * param);
	
};
