//////////////////////////////////////////////////////////////////////////
//TS_StatMgmt.h

#pragma once
#include "ace/Task.h"
#include "FluxControlObserver.h"

class CFluxControlObserver;

class CTS_StatMgmt : public ACE_Task_Base
{
public:
	CTS_StatMgmt(void);
	virtual ~CTS_StatMgmt(void);

public:
	CPF_ShareMem 		m_iUnsStatShm;
	UNS_STAT_SHM_S* 	m_pUnsStatShm;

public:
	int f_ModInit(CFluxControlObserver * pObserver);
	int f_ModUpdateCfg(CTS_CONFIG * pNewTsCfg);
	int Start();
	int Stop();
	int Pause();
	int Continue();

private:
	unsigned int m_nTaskRun;
	CFluxControlObserver *	m_pObserver;
	
	virtual int svc(void);
};
