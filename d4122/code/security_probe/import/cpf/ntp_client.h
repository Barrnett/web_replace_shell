//////////////////////////////////////////////////////////////////////////
//ntp_client.h

#include "cpf/cpf.h"

//����0��ʾ��ȷ������-1��ʾ����
int CPF_CLASS GetNTPTime(ACE_UINT32 ntp_server,ACE_UINT16 port,struct timeval * now_time);

