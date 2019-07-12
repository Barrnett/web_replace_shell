//////////////////////////////////////////////////////////////////////////
//MyConnectToServerMgr.h

#pragma once

#include "cpf/ConnectToServerMgr.h"
#include "config_center.h"

class CFCServerFacade;

class CMyConnectToServerMgr : public CConnectToServerMgr
{
public:
	CMyConnectToServerMgr(void);
	virtual ~CMyConnectToServerMgr(void);

public:
	virtual int SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);

	//user interface
	virtual int SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);

public:
	void SetFCServerFacade(CFCServerFacade * pFCServerFacade);

protected:
	CFCServerFacade * m_pFCServerFacade;
};


class CFCConnectToServerMgr : public CMyConnectToServerMgr
{
public:
	CFCConnectToServerMgr();
	virtual ~CFCConnectToServerMgr();

protected:
	BOOL IsSockReady();

public:
		//�Ѿ��������ӹر��ˣ�׼����������
	//����true,��ʾҪ�����������ӣ�
	//����false,��ʾ��β�Ҫ�������ӣ�
	//ע�⺯�������ģ��Ķ�ʱ���߳��е���
	virtual BOOL OnBeforeReConnect();

	//��������֮��Ļص���ע�⺯�������ģ��Ķ�ʱ���߳��е���
	virtual void OnAfterReconnect(BOOL bReconnectOK);

public:
	virtual int SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);

	//user interface
	virtual int SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);

public:
	int SendSyncRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);


};

