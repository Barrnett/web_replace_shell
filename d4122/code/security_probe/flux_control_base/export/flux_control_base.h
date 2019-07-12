//////////////////////////////////////////////////////////////////////////
//flux_control_base.h

#pragma once

#include "cpf/TimeStamp32.h"
#include "PacketIO/PacketIO.h"
#include "cpf/ostypedef.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "flux_control_base_export.h"
#include "tcpip_mgr_common_init.h"

//���������ڴ�Ĵ�С,�������ֽڶ��롣
//���pbufferΪNULL,��ֻ���������ڴ�Ĵ�С�������п���
FLUX_CONTROL_BASE_CLASS int Package_OnePacketInfoEx(IN char * pbuffer,const PACKET_INFO_EX& packet_ex);

//���������ڴ�Ĵ�С,�������ֽڶ���
FLUX_CONTROL_BASE_CLASS int UnPackage_OnePacketInfoEx(IN const char * pbuffer,PACKET_INFO_EX& packet_ex,const CTimeStamp32& cur_ts);


//��������������������������(line_type==2)
//����������DMZ������(line_type==1)
//������������DMZ������(line_type==0)

//���ʷ���������·����
typedef enum{

	ASLT_INVALID = -1,
	ASLT_I2DMZ = 0,
	ASLT_O2DMZ = 1,
	ASLT_O2I = 2,

}AS_LINE_TYPE;

