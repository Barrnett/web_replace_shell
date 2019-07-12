#ifndef _COM_IMP_H
#define _COM_IMP_H
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "ace/Reactor.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Connector.h"
#include "ace/Synch.h"
#include "ace/Task.h"
#include "ace/Basic_Types.h"
#include "cpf/cpf.h"
#include "cpf/memmgr.h"
#include "cpf/Data_Stream.h"

class ACE_Log_Msg;

enum ERROR_CODE{
	COM_SUCCESS=0,
	COM_NOT_ATTACH=-1,
	COM_MAX_REQUEST=-2,
	COM_TIMEOUT=-3,
	COM_INVALID_RESPONSE=-4,
	COM_EOF=-5,
	COM_SEND_ERR=-6,

	COM_MAX_PROXY=-7,
	COM_MAX_SERVANT=-8,
	COM_SERVANT_ATTACH_TWICE=-9,
	COM_CONNECT_ERR=-10,
	COM_INVALID_STREAM=-11,
	COM_EXCEED_INDEX=-12,
	COM_MEM=-13,
	COM_SERVANT_SETID_TWICE=-14,
	COM_BUILD_SERVER=-15,
	COM_CLOSE_HANDLE=-16,
	COM_INVALID_VALUE=-17,

	COM_RECV_ERR=-18,

	COM_OTHER=-100
};


#pragma pack(push)
#pragma pack(4)
//the servant user should only deal with nOperation
//the proxy user should only deal with nOperation and nResponseCode
typedef struct {
	int nPduLen; //the total len of pdu ,including this field
	int nDestId;
	int nSrcId;
	short nPduType;
	short nRequestSeq;//when pdutype is PDU_TYPE_REQUEST and PDU_TYPE_REPLY,this field is valid
	short nOperation;//when pdutype is PDU_TYPE_REQUEST and PDU_TYPE_REPLY,this field is valid
	short nResponseCode; //when pdutype is PDU_TYPE_RESPONSE, this field is valid
	//data
}PDU_HDR;

enum PDU_TYPE{
	PDU_TYPE_REQUEST=0,
	PDU_TYPE_RESPONSE,
	PDU_TYPE_NOTIFY
};

enum RESPONSE_CODE{
	RESPONSE_CODE_SUCCESS=0,
	RESPONSE_CODE_NO_DEST,
	RESPONSE_CODE_NO_OPERATION,
	RESPONSE_CODE_BAD_FORMAT,
	RESPONSE_CODE_BAD_LOGICAL,
	RESPONSE_CODE_OTHER
};

typedef struct _Block{
	PDU_HDR * pPduHdr;
	char * pBuffer;
	int    nBufferSize;
	int    nDataLen;
}MSG_BLOCK;

#pragma pack(pop)

class CPF_CLASS CMsgBlockManager{
public:
	enum{
		DEFAULT_BUFFER_SIZE=2048,
		MAX_BUFFER_SIZE=1024*1024*8
	};
	static MSG_BLOCK * Malloc(int nBufferSize=DEFAULT_BUFFER_SIZE);

	//根据请求的块，来生成应当块
	static MSG_BLOCK * Malloc_Rsp(MSG_BLOCK * pReqMsgBlock
		,int nBufferSize=DEFAULT_BUFFER_SIZE
		,ACE_UINT16 nResponseCode=RESPONSE_CODE_SUCCESS);

	static int ReSize(MSG_BLOCK * pMsgBlock,int nBufferSize);
	static int AddSize(MSG_BLOCK * pMsgBlock,int nAddBufferSize);
	static int Free(MSG_BLOCK * pMsgBlock);

public:
	static void Clear(MSG_BLOCK& msgBlock)
	{	msgBlock.nDataLen=0;	}

	static const char * ReadPtr(MSG_BLOCK& msgBlock,int nOffset,int nSize);

	static char * WritePtr(MSG_BLOCK& msgBlock,int nOffset,int nSize);

	static const char * ReadPtr(MSG_BLOCK& msgBlock,const char * cur_pos,int nSize)
	{
		return ReadPtr(msgBlock,(int)(cur_pos - msgBlock.pBuffer),nSize);
	}

	static char * WritePtr(MSG_BLOCK& msgBlock,char * cur_pos,int nSize)
	{
		return WritePtr(msgBlock,(int)(cur_pos - msgBlock.pBuffer),nSize);
	}

	//以nOffset的位置获取写数据的指针,用户要保证数据不越界
	static char * WritePtr(MSG_BLOCK& msgBlock,int nOffset)
	{
		return WritePtr(msgBlock,nOffset,msgBlock.nBufferSize-nOffset);
	}

	//从0的位置获取写数据的指针,用户要保证数据不越界
	static char * WritePtr(MSG_BLOCK& msgBlock)
	{
		return WritePtr(msgBlock,0);
	}

};


typedef CMsgBlockManager CMsgBlockHelper_Stream;


enum{
	DEFAULT_REQUEST_TIMEOUT=1, //second
	MAX_SERVANT_NUM=255,
	MAX_PROXY_NUM=255,
};

class CConnection_ForClient_Imp;
class CConnection_ForServer_Imp;
class CServerImp;

typedef struct
{
	int nMsgType;
	enum{
		MSG_TYPE_DATA,
		MSG_TYPE_STOP,
		MSG_TYPE_CLOSE,
		MSG_TYPE_CONNECTED_SUCCESS,
		MSG_TYPE_CONNECTED_FAILED
	};
	MSG_BLOCK *					pMsgBlock;
	CConnection_ForServer_Imp * pConnection;
}MSG_DATA;


class CPF_CLASS CServantImp : public ACE_Task<ACE_MT_SYNCH> {
private:
	int m_nServantId;

public:
	CServantImp(bool multithread=1,int nServantId=-1) 
		: m_nServantId(nServantId)
		, m_bmultithread(multithread)
		, m_pInnerLogInstance(NULL)
	{
	}

	virtual ~CServantImp(void){}

	//SetServantId函数必须在被attach以及Open之前调用
	int SetServantId(int nServantId);
	//设置是否是多线程,SetServantMultithread函数必须在被attach以及Open之前调用
	int SetServantMultithread(bool multithread=1)
	{	m_bmultithread = multithread;	return 0;}
	void SetLogInstance(ACE_Log_Msg *pLogInstance)
	{	m_pInnerLogInstance = pLogInstance;	}

	int GetServantId(void) { return m_nServantId;}

	virtual int Open(void);
	virtual int Close(void);

	int svc(void);

	//called by framework, the user should overload these twe function

	//有一个客户成功的和服务器建立连接了
	virtual void on_connect_success(ACE_INET_Addr * pRemoteAddr){}
	//有一个客户和服务器建立连接失败
	//连接失败,失败原因reason=1表示没有建立连接(accept失败)，2表示客户端过多，其他没有定义
	virtual void on_connect_failed(int reason,ACE_INET_Addr * pRemoteAddr){}
	
	//有一个客户关闭了和服务器的连接
	virtual void OnClose(ACE_INET_Addr * pRemoteAddr){}
	virtual void OnNotify(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock){}
	virtual int OnRequest(ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pRetMsgBlock) //should return RESPONSE_CODE
	{
		return RESPONSE_CODE_NO_OPERATION;
	}


protected:
	virtual int	put_msg_data(MSG_DATA& MsgData, ACE_Time_Value *timeout = 0);

	bool	m_bmultithread;

	int handle_msg_data(MSG_DATA& MsgData);

	int	handle_msg_block(ACE_Message_Block *block);

private:
	 ACE_Log_Msg *	m_pInnerLogInstance;

private:
	friend class CConnection_ForServer_Imp;
	friend class CServerImp;


};



class CPF_CLASS CProxyImp{
private:
	ACE_Thread_Mutex  m_requestMutex;
	int m_nProxyId;
	CConnection_ForClient_Imp * m_pConnectionImp;
	short m_nRequestSeq;
	ACE_Condition<ACE_Thread_Mutex> m_condition;
	MSG_BLOCK * m_pMsgBlock;
	
	bool m_bRequest;

	//call back by CConnectionImp
	int OnResponse(MSG_BLOCK * pMsgBlock); 
	void SetConnectionImp(CConnection_ForClient_Imp * pConnectionImp);
	void SetProxyId(int nProxyId) { m_nProxyId=nProxyId; }

public:
	CProxyImp();
	virtual ~CProxyImp();

	bool IsServerConnected() const;

	virtual int Open(void);
	virtual int Close(void);


	//server关闭了
	virtual void OnServerClose(){}

	//called by framework
	int GetProxyId(void) const { return m_nProxyId; }

	int SendRequest(int servantid,short operation,const void * pdata,size_t ndatalen,MSG_BLOCK * &pResponseMsgBlock, int nTimeout= DEFAULT_REQUEST_TIMEOUT); //called by user to send Request,if success the caller should free the replyMemBlock;
	int SendNotify(int servantid,short operation,const void * pdata,size_t ndatalen);

	//user interface
	int SendRequest(int servantid,MSG_BLOCK * pMsgBlock,MSG_BLOCK * &pResponseMsgBlock, int nTimeout= DEFAULT_REQUEST_TIMEOUT); //called by user to send Request,if success the caller should free the replyMemBlock;
	int SendNotify(int servantid,MSG_BLOCK * pMsgBlock);

	friend class CConnection_ForClient_Imp;
};

class CServerImp;

//user shouldn't use this class
class CPF_CLASS CConnectionImp : public ACE_Event_Handler{

protected:
	MSG_BLOCK * m_pMsgBlock;
	int			m_nRead;//当前读了多少个字节
	char		m_len_buf[4];//获取数据长度字段的缓冲区，4是数据长度字段本身的长度。

protected:
	void zero_recv_msgblock()
	{
		m_nRead = 0;
		m_pMsgBlock = NULL;
	}

public:
	/// Return the local endpoint address in the referenced <ACE_Addr>.
	/// Returns 0 if successful, else -1.
	int get_local_addr (ACE_INET_Addr &) const;

	/**
	* Return the address of the remotely connected peer (if there is
	* one), in the referenced <ACE_Addr>. Returns 0 if successful, else
	* -1.
	*/
	int get_remote_addr (ACE_INET_Addr &) const;

	ACE_SOCK_Stream& get_stream()
	{
		return m_stream;
	}

protected:
	ACE_Recursive_Thread_Mutex  m_mutex_client;

	ACE_SOCK_Stream m_stream;
	ACE_INET_Addr   m_remoteAddr;

protected:
	int RecvData(int read_nums);

public:
	~CConnectionImp();
	CConnectionImp();

	int SendPdu(MSG_BLOCK * pMsgBlock,const ACE_Time_Value *timeout);

	//called by framework
	ACE_SOCK_Stream & GetStream(void) { return m_stream;}
	ACE_INET_Addr *   GetRemoteAddr(void) {return & m_remoteAddr;}
	ACE_HANDLE get_handle() const { return m_stream.get_handle();}
};

//user shouldn't use this class
class CPF_CLASS CConnection_ForServer_Imp : public CConnectionImp{

	CLASS_CPF_MEM_TRACE(CConnection_ForServer_Imp);

private:
	CServantImp *	m_pServantImp[MAX_SERVANT_NUM];

	CServerImp *	m_pServerImp;

public:
	~CConnection_ForServer_Imp();

	CConnection_ForServer_Imp(CServerImp * pServerImp,
		const ACE_Time_Value * recv_timeout,
		const ACE_Time_Value * send_timeout);

	int CloseByServer();

	int AttachServant(CServantImp * pServantImp);
	int DetachServant(CServantImp * pServantImp);

	//called by framework
	virtual int handle_input(ACE_HANDLE);
	virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask);

public:
	inline ACE_Time_Value * get_send_timeout() const
	{
		return m_send_timeout;
	}

	inline ACE_Time_Value * get_recv_timeout() const
	{
		return m_recv_timeout;
	}

private:
	ACE_Time_Value * m_send_timeout;
	ACE_Time_Value * m_recv_timeout;
};

typedef CConnection_ForServer_Imp *	LPConnection_ForServer_Imp;

class CPF_CLASS CConnection_ForClient_Imp : public CConnectionImp
{
	CLASS_CPF_MEM_TRACE(CConnection_ForClient_Imp);

private:
	CProxyImp *		m_pProxyImp[MAX_PROXY_NUM];

public:
	CConnection_ForClient_Imp();
	~CConnection_ForClient_Imp();

public:
	inline ACE_Log_Msg * GetLogInstance()
	{
		return m_pInnerLogInstance;
	}

	inline void SetLogInstance(ACE_Log_Msg * pLogInstance)
	{
		m_pInnerLogInstance = pLogInstance;
	}

public:

	//user interface
	int CloseByUser(void);

	int AttachProxy(CProxyImp * pProxyImp);
	int DetachProxy(CProxyImp * pProxyImp);

	/** @param timeout     Pointer to an @c ACE_Time_Value object with amount
		*                    of time to wait to connect. If the pointer is 0
		*                    then the call blocks until the connection attempt
		*                    is complete, whether it succeeds or fails.  If
		*                    *timeout == {0, 0} then the connection is done
		*                    using nonblocking mode.  In this case, if the
		*                    connection can't be made immediately, this method
		*                    returns -1 and errno == EWOULDBLOCK.
		*                    If *timeout > {0, 0} then this is the maximum amount
		*                    of time to wait before timing out; if the specified
		*                    amount of time passes before the connection is made,
		*                    this method returns -1 and errno == ETIME. Note
		*                    the difference between this case and when a blocking
		*                    connect is attmpted that TCP times out - in the latter
		*                    case, errno will be ETIMEDOUT.*/
	int ConnectServer(const char * pszServerName,int hostorder_port,ACE_Reactor * pReactor,const ACE_Time_Value *timeout = 0);
	int ConnectServer(ACE_UINT32 netorder_ip,int hostorder_port,ACE_Reactor * pReactor,const ACE_Time_Value *timeout = 0);


	//called by framework
	virtual int handle_input(ACE_HANDLE);
	virtual int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask);

private:
	ACE_Log_Msg *	m_pInnerLogInstance;

};

class CPF_CLASS CServerImp : public ACE_Event_Handler{
private:
	ACE_SOCK_Acceptor m_acceptor;

	ACE_Recursive_Thread_Mutex  m_mutex_server;

	CServantImp *	m_pServantImp[MAX_SERVANT_NUM];

protected:
	CConnection_ForServer_Imp ** m_pConnectionImp;

public:
	CServerImp(int nMaxClientNum = 10);
	~CServerImp();
	
	//user interface
	int BuildServer(int port,ACE_Reactor * pReactor,
		const ACE_Time_Value * recv_timeout,
		const ACE_Time_Value * send_timeout
		);

	int Close(void);
	int AttachServant(CServantImp * pServantImp);
	int DetachServant(CServantImp * pServantImp);
	int DetachAllServant();

	//called by framework
	ACE_HANDLE get_handle() const { return m_acceptor.get_handle();}
	int handle_input(ACE_HANDLE);
	int handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask);

	virtual int ConnectionClosed(CConnection_ForServer_Imp * pConnectionImp);

protected:
	//连接失败,失败原因reason=1表示没有建立连接(accept失败)，2表示客户端过多，其他没有定义
	//缺省实现是通知所有servant某个人连接失败
	virtual void on_connect_failed(int reason,CConnection_ForServer_Imp * pConnectionImp);

	//连接成功，缺省实现是通知所有servant某个人连接成功
	virtual void on_connect_success(CConnection_ForServer_Imp * pConnectionImp);

private:
	int	m_nMaxClientNums;

	ACE_Time_Value * m_send_timeout;
	ACE_Time_Value * m_recv_timeout;
};

class CPF_CLASS CBaseDoCmdTool
{
public:
	static void log_request(
		ACE_Log_Msg * pLogInstance,
		ACE_INET_Addr * pRemoteAddr,
		const char * msg_info);

	static MSG_BLOCK * MakeUint16ErrorMsgBlock(
		MSG_BLOCK * pMsgBlock,
		ACE_UINT16 nerror,
		ACE_UINT16 nResponseCode=RESPONSE_CODE_SUCCESS);

	static MSG_BLOCK * MakeUint32ErrorMsgBlock(
		MSG_BLOCK * pMsgBlock,
		ACE_UINT32 nerror,
		ACE_UINT16 nResponseCode=RESPONSE_CODE_SUCCESS);

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif

