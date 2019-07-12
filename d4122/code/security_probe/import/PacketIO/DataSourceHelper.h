//////////////////////////////////////////////////////////////////////////
//DataSourceHelper.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/WriteDataStream.h"
#include "PacketIO/RecvDataSource.h"

class PACKETIO_CLASS CDataSourceHelper
{
public:
	CDataSourceHelper(void);
	~CDataSourceHelper(void);

	static IRecvDataSource * CreateOrignRecvDataSource(DRIVER_TYPE type);

	static IRecvDataSource * CreateRecvDataSource(DRIVER_TYPE type,
		ACE_UINT64 speed,unsigned int uint_nums,
		int adjusttype,const CTimeStamp32 * pAdjustTimeStamp,
		bool bcopypkt,
		unsigned int discard_rate,
		bool bvpk,
		size_t nloopnums,
		unsigned int threadbuffersize);

	static IRecvDataSource * CreateRecvDataSource(IRecvDataSource * pRealDataSource,
		ACE_UINT64 speed,unsigned int uint_nums,
		int adjusttype,const CTimeStamp32 * pAdjustTimeStamp,
		bool bcopypkt,
		unsigned int discard_rate,
		bool bvpk,
		size_t nloopnums,
		unsigned int threadbuffersize,
		int delete_real_source);

	static void DestroyRecvDataSource(IRecvDataSource * pDataSource);

	static IWriteDataStream * CreateWriteDataStream(DRIVER_TYPE type);
	static void DestroyWriteDataStream(IWriteDataStream * pWriteDataStream);
};
