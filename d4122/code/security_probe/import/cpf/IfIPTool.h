//////////////////////////////////////////////////////////////////////////
//IfIPTool.h

//针对网卡设置IP地址的工具类

#pragma once


#include "cpf/cpf.h"

namespace CPF
{

	//设置地址，如果gw=0，表示要删除gw,gw==-1表示gateway不变，不设置
	CPF_CLASS
	int SetIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric);
	
	CPF_CLASS
	int SetIfIPByCmd_Dhcp(const char * if_name);

	//设置地址，如果gw=0或者-1表示不设置gw
	CPF_CLASS
	int AddIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric);

	//删除地址，如果gw=0表示不修改网关，gw==-1表示删除所有网关，其他表示删除指定网关
	//如果ip==0，表示对IP地址不修改，只删除网关
	CPF_CLASS
	int DelIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 gw);

	CPF_CLASS
	int SetIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dns1_ipaddr,ACE_UINT32 dns2_ipaddr);
	CPF_CLASS
	int SetIfDNSByCmd_Dhcp(const char * if_name);

	//删除dns地址，dhsipaddr==-1表示删除所有dns，其他表示删除指定dns
	CPF_CLASS
	int DelIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dhsipaddr);

};

