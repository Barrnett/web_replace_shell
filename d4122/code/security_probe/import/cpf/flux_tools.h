//////////////////////////////////////////////////////////////////////////
//flux_tools.h

#pragma once

#include "cpf/cpf.h"
#include <string>

typedef struct tagSTAT_SI_ITEM
{
	ACE_UINT64 ddFrame;
	ACE_UINT64 ddByte;

	inline tagSTAT_SI_ITEM()
	{	ddFrame = 0;	ddByte = 0;	}

	inline tagSTAT_SI_ITEM(const tagSTAT_SI_ITEM& other)
	{
		*this = other;
	}

	inline tagSTAT_SI_ITEM(ACE_UINT64 frames,ACE_UINT64 bytes)
	{	ddFrame = frames; ddByte = bytes;	}

	inline tagSTAT_SI_ITEM& operator = (const tagSTAT_SI_ITEM& other)
	{
		if (this != &other)
		{
			ddFrame = other.ddFrame; 
			ddByte = other.ddByte;	
		}

		return *this;
	}


	inline void ComeAPacket(ULONG dwLen)
	{ ++ddFrame; ddByte += (ACE_UINT64)dwLen;	}

	friend tagSTAT_SI_ITEM operator + (const tagSTAT_SI_ITEM& a1,const tagSTAT_SI_ITEM& a2)
	{
		return tagSTAT_SI_ITEM(a1.ddFrame+a2.ddFrame,a1.ddByte+a2.ddByte);
	}

	friend tagSTAT_SI_ITEM operator - (const tagSTAT_SI_ITEM& a1,const tagSTAT_SI_ITEM& a2)
	{
		return tagSTAT_SI_ITEM(a1.ddFrame-a2.ddFrame,a1.ddByte-a2.ddByte);
	}

	friend BOOL operator == (const tagSTAT_SI_ITEM& a1,const tagSTAT_SI_ITEM& a2)
	{
		return (a1.ddFrame==a2.ddFrame&&a1.ddByte==a2.ddByte);
	}

	friend BOOL operator != (const tagSTAT_SI_ITEM& a1,const tagSTAT_SI_ITEM& a2)
	{
		return (!(a1==a2));
	}

} STAT_SI_ITEM;

typedef struct tagSTAT_BI_ITEM
{
	ACE_UINT64 ddSendFrame;
	ACE_UINT64 ddSendByte;
	ACE_UINT64 ddRecvFrame;
	ACE_UINT64 ddRecvByte;

	inline tagSTAT_BI_ITEM()
	{
		ddSendFrame = 0;
		ddSendByte = 0;
		ddRecvFrame = 0;
		ddRecvByte = 0;
	}

	inline tagSTAT_BI_ITEM(ACE_UINT64 ddSendFrame1,ACE_UINT64 ddSendByte1,
		ACE_UINT64 ddRecvFrame1,ACE_UINT64 ddRecvByte1)
	{
		ddSendFrame = ddSendFrame1;
		ddSendByte = ddSendByte1;
		ddRecvFrame = ddRecvFrame1;
		ddRecvByte = ddRecvByte1;
	}


	inline tagSTAT_BI_ITEM(const tagSTAT_BI_ITEM& other)
	{
		*this = other;
	}

	inline tagSTAT_BI_ITEM& operator = (const tagSTAT_BI_ITEM& other)
	{
		if (this != &other)
		{
			ddSendFrame = other.ddSendFrame;
			ddSendByte = other.ddSendByte;
			ddRecvFrame = other.ddRecvFrame;
			ddRecvByte = other.ddRecvByte;
		}
		return *this;
	}
	inline void RecvAPacket(ULONG dwLen)
	{ ++ddRecvFrame; ddRecvByte += (ACE_UINT64)dwLen;	}
	inline void SendAPacket(ULONG dwLen)
	{ ++ddSendFrame; ddSendByte += (ACE_UINT64)dwLen;	}

	inline void ComeAPacket(BOOL bSend,ULONG dwLen)
	{
		if( bSend )
			SendAPacket(dwLen);
		else
			RecvAPacket(dwLen);
	}

	friend tagSTAT_BI_ITEM operator + (const tagSTAT_BI_ITEM& a1,const tagSTAT_BI_ITEM& a2)
	{
		return tagSTAT_BI_ITEM(a1.ddSendFrame+a2.ddSendFrame,a1.ddSendByte+a2.ddSendByte,
			a1.ddRecvFrame+a2.ddRecvFrame,a1.ddRecvByte+a2.ddRecvByte);
	}

	friend tagSTAT_BI_ITEM operator - (const tagSTAT_BI_ITEM& a1,const tagSTAT_BI_ITEM& a2)
	{
		return tagSTAT_BI_ITEM(a1.ddSendFrame-a2.ddSendFrame,a1.ddSendByte-a2.ddSendByte,
			a1.ddRecvFrame-a2.ddRecvFrame,a1.ddRecvByte-a2.ddRecvByte);
	}

	friend BOOL operator == (const tagSTAT_BI_ITEM& a1,const tagSTAT_BI_ITEM& a2)
	{
		return (a1.ddSendFrame==a2.ddSendFrame&&a1.ddSendByte==a2.ddSendByte
			&&a1.ddRecvFrame==a2.ddRecvFrame&&a1.ddRecvByte==a2.ddRecvByte);
	}

	friend BOOL operator != (const tagSTAT_BI_ITEM& a1,const tagSTAT_BI_ITEM& a2)
	{
		return (!(a1==a2));
	}

	inline ACE_UINT64 TotalFrames() const
	{	return (ddSendFrame+ddRecvFrame);	}

	inline ACE_UINT64 TotalBytes() const
	{	return (ddSendByte+ddRecvByte);	}

} STAT_BI_ITEM;


typedef enum{
	FLUX_UNIT_BPS = 0,
	FLUX_UNIT_KBPS = 1,
	FLUX_UNIT_MBPS = 2,
	FLUX_UNIT_GBPS = 3,
	FLUX_UNIT_TBPS = 4,

}FLUX_UNIT_TYPE;

#define CPF_FLUX_bps	(1L)
#define CPF_FLUX_Kbps	(CPF_FLUX_bps*1000L)
#define CPF_FLUX_Mbsp	(CPF_FLUX_Kbps*1000L)
#define CPF_FLUX_Gbsp	(CPF_FLUX_Mbsp*1000L)
#define CPF_FLUX_Tbsp	(((ACE_UINT64)CPF_FLUX_Gbsp)*((ACE_UINT64)1000))

namespace CPF
{
	CPF_CLASS
	FLUX_UNIT_TYPE GetFluxUnitType(ACE_UINT64 bps_data);

	CPF_CLASS
	inline ACE_UINT64 GetShowFlux_int(ACE_UINT64 bps_data,FLUX_UNIT_TYPE type);
	CPF_CLASS
	double GetShowFlux_double(ACE_UINT64 bps_data,FLUX_UNIT_TYPE type);

	CPF_CLASS
	inline ACE_UINT64 GetShowFlux_int(ACE_UINT64 bps_data,OUT FLUX_UNIT_TYPE* type=NULL);
	CPF_CLASS
	double GetShowFlux_double(ACE_UINT64 bps_data,OUT FLUX_UNIT_TYPE* type=NULL);


	CPF_CLASS
		//�����GBPS����ʾ2λС���������Mbps��ʾһλС���������kbps�����£�����ʾС��
	std::string GetFluxInfo(ACE_UINT64 bps_data,int showtype=0,OUT FLUX_UNIT_TYPE* type=NULL);

	CPF_CLASS
		//�̶���ʾ2λС��
		std::string GetFluxInfo_2(ACE_UINT64 bps_data,int showtype=0,OUT FLUX_UNIT_TYPE* type=NULL);

	CPF_CLASS
	//����ֵ��bps.
	//���pstrinfo��=NULL����pstrinfo�ǵ�����GetFluxInfo(ACE_UINT64 ubitSpeed,std::string& info);�Ľ��
	ACE_UINT64 GetFluxInfo(ACE_UINT64 cur_bytes,ACE_UINT64 old_bytes,ACE_UINT32 msec,int showtype,std::string * pstrinfo);

	CPF_CLASS
		//����ֵ��pps
	//���pstrinfo!=NULL����pstrinfo�ǵ�����GetFluxInfo(ACE_UINT64 bps_data,std::string& strnum);�Ľ��
	ACE_UINT32 GetPktSpeed(ACE_UINT64 cur_pkts,ACE_UINT64 old_pkts,ACE_UINT32 msec,int showtype,std::string * pstrinfo);

	
	//showtype==0�ֱ���ʾ"bps","Kbps","Mbps","Gbsp","Tbps"��
	//showtype==1�ֱ���ʾ"","K","M","G","T"��
	//showtype==2�ֱ���ʾ"B","KB","MB","GB","TB"��
	CPF_CLASS		
	std::string GetFluxUnitTitle(FLUX_UNIT_TYPE nFluxUnitType,int showtype=0);

	//������ֵ�����data2<data1������0�����򷵻�data2-data1
	template<typename DATA_TYPE>
	DATA_TYPE Flux_Safe_Diff(DATA_TYPE data2,DATA_TYPE data1)
	{
		if( data2 < data1 )
			return 0;

		return (data2-data1);
	}

}

