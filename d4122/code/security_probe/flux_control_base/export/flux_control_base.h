//////////////////////////////////////////////////////////////////////////
//flux_control_base.h

#pragma once

#include "cpf/TimeStamp32.h"
#include "PacketIO/PacketIO.h"
#include "cpf/ostypedef.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "flux_control_base_export.h"
#include "tcpip_mgr_common_init.h"

//返回所需内存的大小,不计算字节对齐。
//如果pbuffer为NULL,则只返回所需内存的大小。不进行拷贝
FLUX_CONTROL_BASE_CLASS int Package_OnePacketInfoEx(IN char * pbuffer,const PACKET_INFO_EX& packet_ex);

//返回跳过内存的大小,不计算字节对齐
FLUX_CONTROL_BASE_CLASS int UnPackage_OnePacketInfoEx(IN const char * pbuffer,PACKET_INFO_EX& packet_ex,const CTimeStamp32& cur_ts);


//处理外网到内网服务器的数据(line_type==2)
//处理外网到DMZ的数据(line_type==1)
//处理内网访问DMZ的数据(line_type==0)

//访问服务器的线路类型
typedef enum{

	ASLT_INVALID = -1,
	ASLT_I2DMZ = 0,
	ASLT_O2DMZ = 1,
	ASLT_O2I = 2,

}AS_LINE_TYPE;

