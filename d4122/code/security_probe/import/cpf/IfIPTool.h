//////////////////////////////////////////////////////////////////////////
//IfIPTool.h

//�����������IP��ַ�Ĺ�����

#pragma once


#include "cpf/cpf.h"

namespace CPF
{

	//���õ�ַ�����gw=0����ʾҪɾ��gw,gw==-1��ʾgateway���䣬������
	CPF_CLASS
	int SetIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric);
	
	CPF_CLASS
	int SetIfIPByCmd_Dhcp(const char * if_name);

	//���õ�ַ�����gw=0����-1��ʾ������gw
	CPF_CLASS
	int AddIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric);

	//ɾ����ַ�����gw=0��ʾ���޸����أ�gw==-1��ʾɾ���������أ�������ʾɾ��ָ������
	//���ip==0����ʾ��IP��ַ���޸ģ�ֻɾ������
	CPF_CLASS
	int DelIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 gw);

	CPF_CLASS
	int SetIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dns1_ipaddr,ACE_UINT32 dns2_ipaddr);
	CPF_CLASS
	int SetIfDNSByCmd_Dhcp(const char * if_name);

	//ɾ��dns��ַ��dhsipaddr==-1��ʾɾ������dns��������ʾɾ��ָ��dns
	CPF_CLASS
	int DelIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dhsipaddr);

};

