
#ifndef _ETHERIO_EXPORT_H_SOMEONE_20061028__
#define _ETHERIO_EXPORT_H_SOMEONE_20061028__

#include "CommonStructDefine_etherio.h"

#ifdef _WIN32
	#ifdef ETHERIO_EXPORTS
	#	ifdef __cplusplus
	#		define ETHERIO_API extern "C"	__declspec(dllexport)
	#	else
	#		define ETHERIO_API				__declspec(dllexport)
	#	endif
	#	define ETHERIO_CLASS				__declspec(dllexport)
	#else
	#	ifdef __cplusplus
	#		define ETHERIO_API extern "C"	__declspec(dllimport)
	#	else
	#		define ETHERIO_API				__declspec(dllimport)
	#	endif
	#	define ETHERIO_CLASS				__declspec(dllimport)
	#endif
#else
	#define ETHERIO_API
#endif




//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					总体部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

enum
{
	EN_ERR_SUCCESS = 0,					// 没有错误
	EN_ERR_SERVICE_NAME = -1,			// 前后调用的服务名字不一致
	EN_ERR_NO_SYSFILE = -2,				// 找不到驱动文件
	EN_ERR_OCCUPY = -3,					// 驱动被其他进程所使用
	EN_ERR_SERVICE_INSTALL = -4,		// 安装服务失败
	EN_ERR_SERVICE_START = -5,			// 启动服务失败
	EN_ERR_SERVICE_STATUS_UNKNOW = -6,	// 服务状态未知
	EN_ERR_DEVICE_IS_BUSY = -7,		// 设备没准备好
	EN_ERR_DRIVER_IS_BUSY = -8,		// 驱动没准备好，DeivceIOControl调用失败
	EN_ERR_OPEN_BY_OTHER = -9,		// 被其他程序所占用
	EN_ERR_CREATE_DRV_FILE = -10,		// 

	EN_ACCESS_DATA = 0,		// 访问数据的方式
	EN_ACCESS_INFO = 1,		// 查询板卡信息的方式
};

//
// 初始化驱动
//		pszFileName - 驱动文件名称
//		pszSrvName - 驱动的服务名称
//		iType - 驱动加载方式：0==数据访问方式；1==信息获取方式
//				方式0为独占，使用后，其他进程再也无法用任何方式在访问驱动
//				方式1为共享，打开后，其他进程可以再次使用1的方式访问驱动
//		bStaticLoad - 驱动的加载方式：0=动态加载，也就是原先使用方式；1=静态加载，不在进行驱动的安装和卸载
//
//		输出：  >0 - 为设备个数
//				=0 - 正常，但无支持的设备
//				<1 - 错误，错误原因见上边的说明
//
ETHERIO_API int EtherIO_Initialize(BOOL bStaticLoad = FALSE);

ETHERIO_API int EtherIO_InitializeEx(
	IN const char* pszFileName, 
	IN const char* pszSrvName,
	IN BOOL bStaticLoad = FALSE);

ETHERIO_API int EtherIO_InitializeEx2(
	IN const char* pszFileName, 
	IN const char* pszSrvName,
	IN int iType,
	IN BOOL bStaticLoad = FALSE);

//
// 关闭驱动程序
// 
ETHERIO_API void EtherIO_UnInitialize();

//
// 遍历设备信息
//
ETHERIO_API int EtherIO_GetDeviceCount();

//
// 返回网卡名称，NULL 表示无效(本地连接)
//
ETHERIO_API const char* EtherIO_GetDeviceName(int iIndex);

//(Intel(R) PRO/1000 PL Network Connection)
ETHERIO_API const char* EtherIO_GetDevDesc(int iIndex);
#ifdef OS_LINUX
ETHERIO_API int EtherIO_GetDeviceKernIndex(int iIndex);
#endif
//
// 取设备MAC地址，输入缓冲区要不小于6个字节
//
ETHERIO_API void EtherIO_GetDeviceMac(int iIndex, OUT UCHAR* pMacAddr);

//根据卡号，获取mac地址的字符串
//如果sep=0,那么输出的是010203040506.text的空间长度至少为13个字节。
//如果间隔字符不为0,比如为':',则输出,01:02:03:04:05:06,其他字符（'-','_'）类推。
//这时text的空间长度至少为18个字节。
ETHERIO_API void EtherIO_GetMacStringByIndex(int iIndex, OUT char * text,
											IN char sep=(':'),IN bool upp=true);

//根据卡号，获取mac地址的字符串，比如："01-02-03-04-05-06,02-03-04-04-05-06"
ETHERIO_API void EtherIO_GetMacStringByMultiIndex(int index_array, OUT char * text,
											IN char sep=(':'),IN bool upp=true);

//得到名字"本地连接|本地连接1"
ETHERIO_API void EtherIO_GetShowNameByMultiIndex(int index_array, OUT char * text);

//
// 通过MAC地址获得板卡的index
//		pszMacAddr - MAC地址字符串，可以是0a0b0c0d0e0f形式，或者0a:0b:0c...，
//			必须为12或者17个字节长，中间以任意字符间隔，不区分大小写
//		return: -1 - input format error, -2 - not find, else - index
//
ETHERIO_API int EtherIO_GetOneIndexByMAC(IN const char* pszMacAddr);

//		pszMacLst - 形式为:"01-02-03-04-05-06,a1-a2-a3-a4-a5-a6".可能多个MAC地址，可能有大小写混合。
//					MAC地址间强制要求带有分隔符, 分隔符可以为'-'或者':'，
//					不同的MAC地址串可以由逗号,空格等分隔,具体分隔符是什么和多少个都无所谓,但不可以是'-'和':'
//		如果没有或者输入不合法，返回0
ETHERIO_API int EtherIO_GetMultiIndexByMAC(IN const char* pszMacLst);

//,如果输入不合法或者没有找到，返回-1
ETHERIO_API int EtherIO_GetOneIndexByShowName(IN const char *pszShowName);

//传入名字"本地连接|本地连接1",如果输入不合法或者没有找到，返回0
ETHERIO_API int EtherIO_GetMultiIndexByShowName(IN const char *pszShowName);

//先调用EtherIO_GetMultiIndexByShowName，如果不成功，再调用EtherIO_GetMultiIndexByMAC。
ETHERIO_API int EtherIO_GetMultiIndexByShowNameOrMAC(IN const char* card_info);


//
// 取设备速率，单位是Mbps，0 或 -1无效
//
ETHERIO_API int EtherIO_GetDeviceLinkSpeed(int iIndex);

//
// 取设备的接收统计
//
ETHERIO_API bool EtherIO_GetDeviceStss(int iIndex, OUT DRIVER_STSS_INFO* pStss);


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					接收部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

typedef void* ETHERIO_RECV_HANDLE;

//
// 创建接收对象，
//		dwDevLst - 以位表示的成员列表，
//		dwBufLen - 指定接收缓冲区大小，以字节为单位
//		bWaitPacketEvent - 轮询时是否采用等待包事件机制，取值为TRUE(阻塞) 或 FALSE(非阻塞) 
//		watermark- 收包缓冲区中包数等于或多于此值时会产生包事件，唤醒收包线程
//		输出为接收对象句柄，0为失败
//
ETHERIO_API ETHERIO_RECV_HANDLE EtherIO_CreateReceiveObj(DWORD dwDevLst, DWORD dwBufLen,BOOL bWaitPacketEvent =TRUE,USHORT watermark = 1);


// 删除接收对象，输入为上边创建的接收对象句柄
//
ETHERIO_API void EtherIO_ReleaseReceiveObj(IN ETHERIO_RECV_HANDLE recvHandle);

//等待接收缓冲区的数据
//返回0表示有数据包，返回1表示超时,-1表示错误，
//参数ms同WaitForSingleObject()中的参数
ETHERIO_API int EtherIO_WaitForRecvPacket(IN ETHERIO_RECV_HANDLE recvHandle,DWORD dwms);
//
// 启动接收，输入为上边创建的接收对象句柄
//
ETHERIO_API bool EtherIO_StartReceive(
	IN ETHERIO_RECV_HANDLE recvHandle,
	IN bool bEnableTimePkt = false,
	IN EN_TIME_MODE enTimeMode = EN_TIME_STAMP_MODE_LOW);

// 
// 停止接收，输入为上边创建的接收对象句柄
//
ETHERIO_API bool EtherIO_StopReceive(IN ETHERIO_RECV_HANDLE recvHandle);

//////////////////////////////////////////////////////////////////////////
//
//  2种取数据的方式，不可以混用
//		EtherIO_GetPacket和EtherIO_SkipPacket成对使用
//		EtherIO_GetNextPacket单独使用
//
//////////////////////////////////////////////////////////////////////////

//
// 取数据
//		recvHandle - 为上边创建的接收对象句柄
//		OUT pPacket - 返回这个数据包的地址
//		OUT stamp - 返回这个数据包的时间辍
//		OUT dwAttrib - 返回这个包的属性
//		OUT dwOriginalLen - 包的原始长度
// 
//	 返回值
//		>0 - 表示有数据包，返回值表示数据包的长度
//		=0 - 当前无包
//		<0 - 描述错误状态，具体值参考<CommonQueueManager.h>文件中PACKET_TYPE_XX的定义
//
ETHERIO_API int EtherIO_GetPacket(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT PUCHAR *pPacket, 
		OUT EIO_TIME *stamp,
		OUT DWORD *card_index, 
		OUT DWORD *dwOriginalLen);


//
// 跳过当前数据包
// 
ETHERIO_API void EtherIO_SkipPacket(IN ETHERIO_RECV_HANDLE recvHandle);

// 
// 取数据，使用这个函数取数据的时候，不需要skip操作
//		参数和返回值含义和EtherIO_GetPacket的一样
//	
ETHERIO_API int EtherIO_GetNextPacket(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT PUCHAR *pPacket, 
		OUT EIO_TIME *stamp,
		OUT DWORD * card_index,
		OUT DWORD *dwOriginalLen);


// 
// 设置定时包的控制
//
/*
ETHERIO_API void EtherIO_EnableTimedControl(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		bool bEnable);
*/


//
// 取接收对象的接收统计
//
ETHERIO_API bool EtherIO_GetRecvStss(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT DRIVER_STSS_INFO* pStss);

//
// 取接收缓冲区的使用字节数
//
//		recvHandle - 为上边创建的接收对象句柄
//		OUT pTotalLen - 返回这个缓冲区的总长度
// 
//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）
ETHERIO_API DWORD EtherIO_GetRecvBufferUsed(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT DWORD* pTotalLen);


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					发送部分                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
typedef void* ETHERIO_SEND_HANDLE;

//typedef enum{EN_SENDOBJ_FIFO=0, EN_SENDOBJ_BLOCK=1} EN_SENDOBJ_MODE;

//
// 创建发送对象，
//		iIndex - 要发送的设备索引，
//		dwBufLen - 指定接收缓冲区大小，以字节为单位
//		输出为发送对象句柄，0为失败
//
// ETHERIO_API ETHERIO_SEND_HANDLE EtherIO_CreateSendObj(
// 		EN_SEND_MODE enSendMode,
// 		int iIndex, 
// 		DWORD dwBufLen);
ETHERIO_API ETHERIO_SEND_HANDLE EtherIO_CreateSendObj(
		SEND_CREATE_PARA*  pSendPara);

//
// 删除发送对象，输入为上边创建的发送对象句柄
//
ETHERIO_API bool EtherIO_ReleaseSendObj(IN ETHERIO_SEND_HANDLE sendHandle);

//
// 启动发送，输入为上边创建的发送对象句柄
//
ETHERIO_API bool EtherIO_StartSend(
	IN ETHERIO_SEND_HANDLE sendHandle, 
	IN EN_TIME_MODE enTimeMode = EN_TIME_STAMP_MODE_LOW);

// 
// 停止发送，输入为上边创建的发送对象句柄
//
ETHERIO_API bool EtherIO_StopSend(IN ETHERIO_SEND_HANDLE sendHandle);

// 
// 取发送的状态
//		sendHandle - 发送对象句柄
//		OUT sendInfo - 返回发送的信息
//
ETHERIO_API bool EtherIO_GetSendInfo(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		OUT GET_SEND_INFO* pSendInfo);

/*
// 
// 设置发送的时间精度
//		sendHandle - 发送对象句柄
//		enTimeMode - 时间精度
//
ETHERIO_API bool EtherIO_SetSendTimeMode(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN EN_TIME_MODE enTimeMode);
*/

//
// 发送数据
//		sendHandle - 发送对象句柄
//		pPacket - 数据包指针
//		dwPktLen - 数据包长度
//		dwReTryTimes - 重试次数(对于单包发送没有意义)
//
ETHERIO_API bool EtherIO_SendPacket(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN PUCHAR pPacket, 
		IN DWORD dwPktLen, 
		IN DWORD dwReTryTimes);

//
// 从发送缓冲区申请一段内存
//		sendHandle - 发送对象句柄
//		dwWriteLength - 申请的长度
//		dwReTryTimes - 重试次数
//		返回缓冲区的指针
//
ETHERIO_API PVOID EtherIO_AllocSendBuffer(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength, 
		IN DWORD dwReTryTimes);

//
// 通知发送缓冲区，使用了的数据长度
//		sendHandle - 发送对象句柄
//		dwWriteLength - 使用了的长度
//		dwReTryTimes - 重试次数
//
//	 注意：使用此函数前，要先调用EtherIO_AllocSendBuffer申请一个缓冲区，
//
ETHERIO_API bool EtherIO_FinishedSendBuffer(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength);



//
// 发送数据(带有时间辍的方式)
//		sendHandle - 发送对象句柄
//		pPacket - 数据包指针
//		dwPktLen - 数据包长度
//		dwReTryTimes - 重试次数
//
ETHERIO_API bool EtherIO_SendPacketWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN PUCHAR pPacket, 
		IN DWORD dwPktLen, 
		IN DWORD dwReTryTimes,//(对于单包发送没有意义)
		IN DWORD dwOffset//(对于单包发送没有意义)
		);

//
// 从发送缓冲区申请一段内存(带有时间辍的方式)
//		sendHandle - 发送对象句柄
//		dwWriteLength - 申请的长度
//		dwReTryTimes - 重试次数
//		返回缓冲区的指针
//
ETHERIO_API PVOID EtherIO_AllocSendBufferWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength, 
		IN DWORD dwReTryTimes,
		IN DWORD dwOffset);

//
// 通知发送缓冲区，使用了的数据长度(带有时间辍的方式)
//		sendHandle - 发送对象句柄
//		dwWriteLength - 使用了的长度
//		dwReTryTimes - 重试次数
//
//	 注意：使用此函数前，要先调用EtherIO_AllocSendBuffer申请一个缓冲区，
//
ETHERIO_API bool EtherIO_FinishedSendBufferWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength);

//
// 设置BLOCK发送方式下的待发送数据
//		sendHandle - 发送对象句柄
//		pSendData - 应用程序开辟并设置的数据缓冲区指针
//
// 返回值从bool 改为 int
//	0 -- 成功
//	-1 -- 数据长度错误
//	-2 -- 当前的发送模式错误
//	-3 -- 与驱动交互错误
ETHERIO_API int EtherIO_BlockSetData(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN BLOCK_SEND_DATA_STAMP* pSendData);


//
// 设置BLOCK发送方式下的发送参数
//		sendHandle - 发送对象句柄
//		pSendData - 应用程序开辟并设置的发送参数指针
//
ETHERIO_API bool EtherIO_BlockSetPara(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN BLOCK_SEND_PARA* pSendPara);


//
// 取发送缓冲区的使用字节数
//
//		sendHandle - 发送对象句柄
//		OUT pTotalLen - 返回这个缓冲区的总长度
// 
//	 返回值
//		=0 - 缓冲区当前已经使用了的长度
//
//	 说明
//		当pTotalLen不为空的时候，这个参数返回缓冲区的总长度（字节）

ETHERIO_API DWORD EtherIO_GetSendBufferUsed(
		IN ETHERIO_RECV_HANDLE sendHandle, 
		OUT DWORD* pTotalLen);


//////////////////////////////////////////////////////////////////////////
//
// 资源释放类，请勿轻易调用
//
//////////////////////////////////////////////////////////////////////////

//
// 释放驱动中所有申请的资源
//
ETHERIO_API void EtherIO_ResourceReleaseAll();
//
// 释放驱动中本进程占用的所有资源
//
ETHERIO_API void EtherIO_ResourceReleaseAllMine();
//
// 释放驱动中非本进程占用的所有资源
//
ETHERIO_API void EtherIO_ResourceReleaseAllOther();



//////////////////////////////////////////////////////////////////////////
//
// 取板卡驱动统计的接口
//
//////////////////////////////////////////////////////////////////////////

#define OID_GEN_XMIT_OK                         0x00020101
#define OID_GEN_RCV_OK                          0x00020102
#define OID_GEN_XMIT_ERROR                      0x00020103
#define OID_GEN_RCV_ERROR                       0x00020104
#define OID_GEN_RCV_NO_BUFFER                   0x00020105

ETHERIO_API bool EtherIO_GetNICStatistic(
		IN int iIndex, 
		IN DWORD dwStatType, 
		OUT ULONGLONG *ddwValue);


/*
不包括报头的帧最大传输长度
return:	-1 -- failed
		else -- right
*/
ETHERIO_API int EtherIO_GetMaxFrameSize(IN int iIndex);

#endif // _ETHERIO_EXPORT_H_SOMEONE_20061028__
