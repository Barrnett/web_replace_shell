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
		//已经出现连接关闭了，准备重新连接
	//返回true,表示要进行重新连接，
	//返回false,表示这次不要重新连接，
	//注意函数在这个模块的定时器线程中调用
	virtual BOOL OnBeforeReConnect();

	//重新连接之后的回调。注意函数在这个模块的定时器线程中调用
	virtual void OnAfterReconnect(BOOL bReconnectOK);

public:
	virtual int SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);

	//user interface
	virtual int SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);

public:
	int SendSyncRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout);


};

