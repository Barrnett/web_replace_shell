#ifndef ADAPTEROBJ_H
#define ADAPTEROBJ_H

#include "cpf/ostypedef.h"

#ifdef OS_WINDOWS

//#include "packetdef.h"
#include "Packet32.h"
#include <iphlpapi.h>


class adapterobj //支持多线程 
{
public:
	enum
	{
		Max_Num_Adapter = 10
	};

	typedef enum
	{
		INPUTDATATYPE_NET = 0,
		INPUTDATATYPE_TAP = 1
	}INPUT_TYPE;

	adapterobj(INPUT_TYPE type);
	~adapterobj();

public:
	void Close();
	BOOL Open(int open);
	BOOL StartCapture();
	void StopCapture();
	DWORD GetObjID() {return m_objID;}

	int  EnumAdapterInfo(BOOL bGet = TRUE);
	BOOL SetHwFilter(ULONG Filter);
	BOOL SetDriverBuff(int dim);
	void SetReadBuff(int dim);
	BOOL SetMinToCopy(int nNum);
	BOOL SendRSTPacket(const char *Buffer,int Buflen);
	BOOL SetReadTimeout(int timeout=1000);

	BOOL ResetAdapter();
	
	char *GetAdapterInfo(int open);
	
	ADAPTER *Adapter(){return m_lpAdapter;}

	//PACKET*  PacketAlloc();
	//void	 PacketFree(PACKET *blk);

	BOOL ReceivePacket(LPPACKET lpPacket,BOOLEAN Sync=TRUE);

	BOOL GetStats(bpf_stat *stat);

	BOOL IsOpen() { return m_bOpen; }
	void BeOpen() { m_bOpen = TRUE; }
	void NoOpen() { m_bOpen = FALSE; }

	int	 GetOpen(){ return m_OpenAdapterNo; }

	char  m_AdapterName[Max_Num_Adapter][1024];

	static int GetAdaptersName_NT(char AdapterName[][1024]);

private:
	WORD checksum(USHORT *buffer, int size);
	
private:	
	LPADAPTER m_lpAdapter;
	int   m_ReadBuffSize;
	BOOL  m_bOpen;
	int	  m_OpenAdapterNo;
	INPUT_TYPE	m_inputtype;
protected:
	DWORD  m_objID;
	

};

#endif//OS_WINDOWS

#endif // ADAPTEROBJ_H


