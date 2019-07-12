//////////////////////////////////////////////////////////////////////////
//HostEtherCardMgr.h 一个主机的以太网卡的管理

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CHostEtherCardMgr
{
public:
	typedef struct tagONE_ETHER_CARD_INFO
	{
		int		_iPktMaxLen;				// 发送的时候最大的包长

		//下面三个对于linux都是一样的，就是etho,eth1等信息
		char	_szLinkName[MAX_PATH];		// 设备的Link 名称，会是一大串GUI(\DEVICE\{E110C6A6-95D5-42A8-83B2-6040D63794EC})
		char	_szDevDesc[MAX_PATH];	// 设备的显示名称，应该和网络属性里边看到的一样(Intel(R) PRO/1000 PL Network Connection)
		char	_szShowName[MAX_PATH];	// 该设备的名称(本地连接)

		BYTE	_szMacAddress[8];	// 设备的MAC地址，比类不提供修改MAC地址的功能

	}ONE_ETHER_CARD_INFO;

public:
	CHostEtherCardMgr(void);
	~CHostEtherCardMgr(void);

#ifdef OS_WINDOWS
private:
	int QuaryCardInfoFromReg_Win();
#endif

#ifdef OS_LINUX
private:
	int QuaryCardInfoFromReg_Linux();
#endif

public:
	//这里面获取所有网卡的基本信息，包括mac地址，名字，描述等
	//返回有多少个网卡
	int Init();

public:
	int GetOneIndexByMAC(const BYTE * mac_addr) const;
	int GetOneIndexByShowName(const char * shoname) const;

public:
	ONE_ETHER_CARD_INFO		m_array_card_info[32];
	int						m_card_count;

};
