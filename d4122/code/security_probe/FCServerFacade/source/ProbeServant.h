//////////////////////////////////////////////////////////////////////////
//ProbeServant .h

#pragma once

#include "cpf/ComImp.h"
#include "cpf/Data_Stream.h"
#include "cpf/NetcomBaseServant.h"
#include "cpf/ACE_Log_File_Msg.h"

class CProbeServant : public CServantImp
{
public:
	CProbeServant();
	virtual ~CProbeServant(void);

protected:
	void init(ACE_Log_File_Msg* pLogInstance,BOOL log_put_msg=FALSE);

public:
	int test_request();

private:
	ACE_Log_File_Msg*		m_pLogInstance;

	BOOL					m_log_put_msg;

};

class CCConfigAndCtrlDoCmd;
class CFCServerFacade;
class CFCSockManager;

//һ����ִ�е���������½���ʺţ������·�������ͬ����
class CCmdServant : public CProbeServant
{
public:
	CCmdServant();
	virtual ~CCmdServant(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFCServerFacade * pFCServerFacade,
		CFCSockManager* pFCSockManager);

public:
	//��һ���ͻ��ɹ��ĺͷ���������������
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

	CCConfigAndCtrlDoCmd * GetConfigAndCtrlDoCmd()
	{
		return m_pConfigAndCtrlDoCmd;
	}

private:
	CCConfigAndCtrlDoCmd*	m_pConfigAndCtrlDoCmd;//���ƺ������·�������
};

class CSysMaintenanceDoCmd;

class CSysMaintenanceServant : public CProbeServant
{
public:
	CSysMaintenanceServant();
	virtual ~CSysMaintenanceServant(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFCSockManager* pFCSockManager,
		CFCServerFacade * pFCServerFacade);

public:
	//��һ���ͻ��ɹ��ĺͷ���������������
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	CSysMaintenanceDoCmd *	m_pSysMaintenanceDoCmd;

};

class CFCLiveDoCmd;

//ʵʱͳ�ƺ��豸״̬�ģ���Ҫ���ڽ��������Զ�ȡ���ݻ����豸״̬
class CLiveStatServant : public CProbeServant
{
public:
	CLiveStatServant();

	virtual ~CLiveStatServant(void);

public:
	void init(ACE_Log_File_Msg* pLogInstance,
		CFCSockManager* pFCSockManager,
		CFCServerFacade * pFCServerFacade);

public:
	//��һ���ͻ��ɹ��ĺͷ���������������
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

private:
	CFCLiveDoCmd *	m_pFCLiveDoCmd;

};

class CProbeNetCom : public CNetServerBaseServant<CMyServerLogImp>
{
public:
	CProbeNetCom()
	{
		m_pCmdServant = NULL;
		m_pSysMaintenanceServant = NULL;
		m_pLiveStatServant = NULL;

	}

	~CProbeNetCom()
	{	
		close();
	}

public:
	bool init(int port,
		int cmd_servant_id,
		int live_stat_servant_id,
		int sys_maintance_servand_id,
		bool multithread, 
		int nMaxClientNum,
		ACE_Time_Value * recv_timeout,
		ACE_Time_Value * send_timeout,
		ACE_Log_File_Msg * pLogMsg,
		CFCServerFacade * pFCServerFacade,
		CFCSockManager* pFCSockManager);

	void close();

	CCConfigAndCtrlDoCmd * GetConfigAndCtrlDoCmd();

private:
	CCmdServant *		m_pCmdServant;
	CSysMaintenanceServant * m_pSysMaintenanceServant;
	CLiveStatServant *	m_pLiveStatServant;

};
