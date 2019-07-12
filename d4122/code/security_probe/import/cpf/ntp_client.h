//////////////////////////////////////////////////////////////////////////
//ntp_client.h

#include "cpf/cpf.h"

//返回0表示正确，返回-1表示错误
int CPF_CLASS GetNTPTime(ACE_UINT32 ntp_server,ACE_UINT16 port,struct timeval * now_time);

