//////////////////////////////////////////////////////////////////////////
//ACE_MemmapLog_Msg.h

#pragma once

#include "cpf/cpf.h"
#include "ace/Log_Msg.h"

class CMemmap_Callback;

class CPF_CLASS ACE_Memmap_Log_Msg : public ACE_Log_Msg
{
public:
	ACE_Memmap_Log_Msg(void);
	virtual ~ACE_Memmap_Log_Msg(void);

public:
	BOOL init(const char * filename,unsigned int bufsize=100*1024);
	void fini();

private:
	CMemmap_Callback *	m_pMemmapCallback;


};
