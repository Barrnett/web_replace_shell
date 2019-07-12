//////////////////////////////////////////////////////////////////////////
//ProbeServant .h

#pragma once

#include "cpf/ComImp.h"
#include "cpf/Data_Stream.h"
#include "cpf/NetcomBaseServant.h"
#include "CConfigAndCtrlDoCmd.h"
#include "FCLiveDoCmd.h"

class CCmdServant : public CServantImp
{
public:
	CCmdServant(void);
	virtual ~CCmdServant(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFluxControlObserver * pFluxControlObserver);

public:
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);


private:
	CCConfigAndCtrlDoCmd		m_ConfigAndCtrlDoCmd;

private:
	ACE_Log_File_Msg*			m_pLogInstance;

	enum{ERROR_CODE_SUCESS=0,ERROR_CODE_FAILED=1};

};


class CLiveStatServant : public CServantImp
{
public:
	CLiveStatServant(void);
	virtual ~CLiveStatServant(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFluxControlObserver * pFluxControlObserver);

public:
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	CFCLiveDoCmd				m_FCLiveDoCmd;

private:
	ACE_Log_File_Msg*			m_pLogInstance;


};




class CProbeNetCom : public CNetServerBaseServant<CMyServerLogImp>
{
public:
	CProbeNetCom();
	virtual ~CProbeNetCom();

public:
	bool init(
		int port,int cmd_servantid,int live_servantid,bool multithread,int nMaxClientNum,
		ACE_Time_Value * recv_timeout,
		ACE_Time_Value * send_timeout,
		CFluxControlObserver * pFluxControlObserver,
		ACE_Log_File_Msg * pLogMsg);

	void close();

private:
	CCmdServant *		m_pCmdServant;
	CLiveStatServant *	m_pLiveStatServant;


};
