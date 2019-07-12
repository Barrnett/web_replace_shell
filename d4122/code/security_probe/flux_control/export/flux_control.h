//////////////////////////////////////////////////////////////////////////
//flux_control.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/flux_tools.h"
#include "PacketIO/RecvDataSource.h"
#include "PacketIO/WriteDataStream.h"
#include "PacketIO/SerialDataRecv.h"
#include "PacketIO/SerialDataSend.h"

#include "flux_control_export.h"
#include "Flux_Control_Common_Struct.h"
#include "PacketIO/DataSend_Param_Info.h"

//enum{
//	TEST_MODE_none = -1,
//	TEST_MODE_series = 0,
//	TEST_MODE_parallel = 1,		
//	TEST_MODE_playback = 2,
//	TEST_MODE_series_and_span = 3,
//};


FLUX_CONTROL_API IRecvDataSource * FLUX_CONTROL_GetRecvDataSource();
FLUX_CONTROL_API IWriteDataStream * FLUX_CONTROL_GetWriteDataStream();

class FLUX_CONTROL_CLASS IFCRebootHandler
{
public:
	IFCRebootHandler(){}
	virtual ~IFCRebootHandler(){}

public:
	virtual int Do(int type,int reason) = 0;

};

FLUX_CONTROL_API void FLUX_CONTROL_SetRebootHandler(IFCRebootHandler * pFCRebootHandler);

FLUX_CONTROL_API int FLUX_CONTROL_GetCardNums();
FLUX_CONTROL_API int FLUX_CONTROL_GetCardStat(int pos,STAT_BI_ITEM& item,std::string* p_str_card_name);

//line_type=0表示内网访问internet，line_type=1表示外网访问DMZ
//link_type=1表示tcp，link_type=2表示udp
FLUX_CONTROL_API void FLUX_CONTROL_GetLinkNums(int line_type,int link_type,UINT& total,UINT& cur);

//link_type=1表示tcp，link_type=2表示udp
FLUX_CONTROL_API void FLUX_CONTROL_GetNatLinkNums(int link_type,UINT& total,UINT& cur);


FLUX_CONTROL_API BOOL FLUX_CONTROL_GetChannelFlux(int vm_type,ACE_UINT32 vm_id,
												  unsigned int channel_id,CHANNEL_PERF_STAT_EX& stat);

//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）
FLUX_CONTROL_API ACE_UINT32 FLUX_CONTROL_GetRecvBufferUsed(OUT ACE_UINT32* pTotalLen);


//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

FLUX_CONTROL_API ACE_UINT32 FLUX_CONTROL_GetSendBufferUsed(OUT ACE_UINT32* pTotalLen);

//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

FLUX_CONTROL_API BOOL FLUX_CONTROL_GetAllNetCardInfo(BOOL& bDrvOK,std::vector<std::string>& vt_cardinfo);

FLUX_CONTROL_API BOOL FLUX_CONTROL_StartCapCptData(BOOL bUseFlt,
												const char * flt_filename,
												std::vector< std::string > vt_from_card_name,												
												const Cpt_Write_Param& cpt_param);


FLUX_CONTROL_API BOOL FLUX_CONTROL_StartCapEthrealData(BOOL bUseFlt,
												const char * flt_filename,
												std::vector< std::string > vt_from_card_name,								
												const Ethereal_Write_Param& ethreal_param);

FLUX_CONTROL_API void FLUX_CONTROL_StopCapData();

FLUX_CONTROL_API void FLUX_CONTROL_Do_OutPut();


FLUX_CONTROL_API void FLUX_CONTROL_EanbelTraceCode(BOOL bEnable);


FLUX_CONTROL_API void FLUX_CONTROL_SetStopEventInfo(const char * stop_event_name,int stop_event_interval);

FLUX_CONTROL_API BOOL FLUX_CONTROL_IsGetStopEvent();

FLUX_CONTROL_API CSerialDataRecv* FLUX_CONTROL_GetSerialDataRecv();
FLUX_CONTROL_API CSerialDataSend* FLUX_CONTROL_GetSerialDataSend();