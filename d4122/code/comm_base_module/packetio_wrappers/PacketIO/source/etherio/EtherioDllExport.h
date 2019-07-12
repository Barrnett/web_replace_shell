
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
//					���岿��                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

enum
{
	EN_ERR_SUCCESS = 0,					// û�д���
	EN_ERR_SERVICE_NAME = -1,			// ǰ����õķ������ֲ�һ��
	EN_ERR_NO_SYSFILE = -2,				// �Ҳ��������ļ�
	EN_ERR_OCCUPY = -3,					// ����������������ʹ��
	EN_ERR_SERVICE_INSTALL = -4,		// ��װ����ʧ��
	EN_ERR_SERVICE_START = -5,			// ��������ʧ��
	EN_ERR_SERVICE_STATUS_UNKNOW = -6,	// ����״̬δ֪
	EN_ERR_DEVICE_IS_BUSY = -7,		// �豸û׼����
	EN_ERR_DRIVER_IS_BUSY = -8,		// ����û׼���ã�DeivceIOControl����ʧ��
	EN_ERR_OPEN_BY_OTHER = -9,		// ������������ռ��
	EN_ERR_CREATE_DRV_FILE = -10,		// 

	EN_ACCESS_DATA = 0,		// �������ݵķ�ʽ
	EN_ACCESS_INFO = 1,		// ��ѯ�忨��Ϣ�ķ�ʽ
};

//
// ��ʼ������
//		pszFileName - �����ļ�����
//		pszSrvName - �����ķ�������
//		iType - �������ط�ʽ��0==���ݷ��ʷ�ʽ��1==��Ϣ��ȡ��ʽ
//				��ʽ0Ϊ��ռ��ʹ�ú�����������Ҳ�޷����κη�ʽ�ڷ�������
//				��ʽ1Ϊ�����򿪺��������̿����ٴ�ʹ��1�ķ�ʽ��������
//		bStaticLoad - �����ļ��ط�ʽ��0=��̬���أ�Ҳ����ԭ��ʹ�÷�ʽ��1=��̬���أ����ڽ��������İ�װ��ж��
//
//		�����  >0 - Ϊ�豸����
//				=0 - ����������֧�ֵ��豸
//				<1 - ���󣬴���ԭ����ϱߵ�˵��
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
// �ر���������
// 
ETHERIO_API void EtherIO_UnInitialize();

//
// �����豸��Ϣ
//
ETHERIO_API int EtherIO_GetDeviceCount();

//
// �����������ƣ�NULL ��ʾ��Ч(��������)
//
ETHERIO_API const char* EtherIO_GetDeviceName(int iIndex);

//(Intel(R) PRO/1000 PL Network Connection)
ETHERIO_API const char* EtherIO_GetDevDesc(int iIndex);
#ifdef OS_LINUX
ETHERIO_API int EtherIO_GetDeviceKernIndex(int iIndex);
#endif
//
// ȡ�豸MAC��ַ�����뻺����Ҫ��С��6���ֽ�
//
ETHERIO_API void EtherIO_GetDeviceMac(int iIndex, OUT UCHAR* pMacAddr);

//���ݿ��ţ���ȡmac��ַ���ַ���
//���sep=0,��ô�������010203040506.text�Ŀռ䳤������Ϊ13���ֽڡ�
//�������ַ���Ϊ0,����Ϊ':',�����,01:02:03:04:05:06,�����ַ���'-','_'�����ơ�
//��ʱtext�Ŀռ䳤������Ϊ18���ֽڡ�
ETHERIO_API void EtherIO_GetMacStringByIndex(int iIndex, OUT char * text,
											IN char sep=(':'),IN bool upp=true);

//���ݿ��ţ���ȡmac��ַ���ַ��������磺"01-02-03-04-05-06,02-03-04-04-05-06"
ETHERIO_API void EtherIO_GetMacStringByMultiIndex(int index_array, OUT char * text,
											IN char sep=(':'),IN bool upp=true);

//�õ�����"��������|��������1"
ETHERIO_API void EtherIO_GetShowNameByMultiIndex(int index_array, OUT char * text);

//
// ͨ��MAC��ַ��ð忨��index
//		pszMacAddr - MAC��ַ�ַ�����������0a0b0c0d0e0f��ʽ������0a:0b:0c...��
//			����Ϊ12����17���ֽڳ����м��������ַ�����������ִ�Сд
//		return: -1 - input format error, -2 - not find, else - index
//
ETHERIO_API int EtherIO_GetOneIndexByMAC(IN const char* pszMacAddr);

//		pszMacLst - ��ʽΪ:"01-02-03-04-05-06,a1-a2-a3-a4-a5-a6".���ܶ��MAC��ַ�������д�Сд��ϡ�
//					MAC��ַ��ǿ��Ҫ����зָ���, �ָ�������Ϊ'-'����':'��
//					��ͬ��MAC��ַ�������ɶ���,�ո�ȷָ�,����ָ�����ʲô�Ͷ��ٸ�������ν,����������'-'��':'
//		���û�л������벻�Ϸ�������0
ETHERIO_API int EtherIO_GetMultiIndexByMAC(IN const char* pszMacLst);

//,������벻�Ϸ�����û���ҵ�������-1
ETHERIO_API int EtherIO_GetOneIndexByShowName(IN const char *pszShowName);

//��������"��������|��������1",������벻�Ϸ�����û���ҵ�������0
ETHERIO_API int EtherIO_GetMultiIndexByShowName(IN const char *pszShowName);

//�ȵ���EtherIO_GetMultiIndexByShowName��������ɹ����ٵ���EtherIO_GetMultiIndexByMAC��
ETHERIO_API int EtherIO_GetMultiIndexByShowNameOrMAC(IN const char* card_info);


//
// ȡ�豸���ʣ���λ��Mbps��0 �� -1��Ч
//
ETHERIO_API int EtherIO_GetDeviceLinkSpeed(int iIndex);

//
// ȡ�豸�Ľ���ͳ��
//
ETHERIO_API bool EtherIO_GetDeviceStss(int iIndex, OUT DRIVER_STSS_INFO* pStss);


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					���ղ���                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

typedef void* ETHERIO_RECV_HANDLE;

//
// �������ն���
//		dwDevLst - ��λ��ʾ�ĳ�Ա�б�
//		dwBufLen - ָ�����ջ�������С�����ֽ�Ϊ��λ
//		bWaitPacketEvent - ��ѯʱ�Ƿ���õȴ����¼����ƣ�ȡֵΪTRUE(����) �� FALSE(������) 
//		watermark- �հ��������а������ڻ���ڴ�ֵʱ��������¼��������հ��߳�
//		���Ϊ���ն�������0Ϊʧ��
//
ETHERIO_API ETHERIO_RECV_HANDLE EtherIO_CreateReceiveObj(DWORD dwDevLst, DWORD dwBufLen,BOOL bWaitPacketEvent =TRUE,USHORT watermark = 1);


// ɾ�����ն�������Ϊ�ϱߴ����Ľ��ն�����
//
ETHERIO_API void EtherIO_ReleaseReceiveObj(IN ETHERIO_RECV_HANDLE recvHandle);

//�ȴ����ջ�����������
//����0��ʾ�����ݰ�������1��ʾ��ʱ,-1��ʾ����
//����msͬWaitForSingleObject()�еĲ���
ETHERIO_API int EtherIO_WaitForRecvPacket(IN ETHERIO_RECV_HANDLE recvHandle,DWORD dwms);
//
// �������գ�����Ϊ�ϱߴ����Ľ��ն�����
//
ETHERIO_API bool EtherIO_StartReceive(
	IN ETHERIO_RECV_HANDLE recvHandle,
	IN bool bEnableTimePkt = false,
	IN EN_TIME_MODE enTimeMode = EN_TIME_STAMP_MODE_LOW);

// 
// ֹͣ���գ�����Ϊ�ϱߴ����Ľ��ն�����
//
ETHERIO_API bool EtherIO_StopReceive(IN ETHERIO_RECV_HANDLE recvHandle);

//////////////////////////////////////////////////////////////////////////
//
//  2��ȡ���ݵķ�ʽ�������Ի���
//		EtherIO_GetPacket��EtherIO_SkipPacket�ɶ�ʹ��
//		EtherIO_GetNextPacket����ʹ��
//
//////////////////////////////////////////////////////////////////////////

//
// ȡ����
//		recvHandle - Ϊ�ϱߴ����Ľ��ն�����
//		OUT pPacket - ����������ݰ��ĵ�ַ
//		OUT stamp - ����������ݰ���ʱ���
//		OUT dwAttrib - ���������������
//		OUT dwOriginalLen - ����ԭʼ����
// 
//	 ����ֵ
//		>0 - ��ʾ�����ݰ�������ֵ��ʾ���ݰ��ĳ���
//		=0 - ��ǰ�ް�
//		<0 - ��������״̬������ֵ�ο�<CommonQueueManager.h>�ļ���PACKET_TYPE_XX�Ķ���
//
ETHERIO_API int EtherIO_GetPacket(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT PUCHAR *pPacket, 
		OUT EIO_TIME *stamp,
		OUT DWORD *card_index, 
		OUT DWORD *dwOriginalLen);


//
// ������ǰ���ݰ�
// 
ETHERIO_API void EtherIO_SkipPacket(IN ETHERIO_RECV_HANDLE recvHandle);

// 
// ȡ���ݣ�ʹ���������ȡ���ݵ�ʱ�򣬲���Ҫskip����
//		�����ͷ���ֵ�����EtherIO_GetPacket��һ��
//	
ETHERIO_API int EtherIO_GetNextPacket(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT PUCHAR *pPacket, 
		OUT EIO_TIME *stamp,
		OUT DWORD * card_index,
		OUT DWORD *dwOriginalLen);


// 
// ���ö�ʱ���Ŀ���
//
/*
ETHERIO_API void EtherIO_EnableTimedControl(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		bool bEnable);
*/


//
// ȡ���ն���Ľ���ͳ��
//
ETHERIO_API bool EtherIO_GetRecvStss(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT DRIVER_STSS_INFO* pStss);

//
// ȡ���ջ�������ʹ���ֽ���
//
//		recvHandle - Ϊ�ϱߴ����Ľ��ն�����
//		OUT pTotalLen - ����������������ܳ���
// 
//	 ����ֵ
//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
//
//	 ˵��
//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�
ETHERIO_API DWORD EtherIO_GetRecvBufferUsed(
		IN ETHERIO_RECV_HANDLE recvHandle, 
		OUT DWORD* pTotalLen);


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//					���Ͳ���                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
typedef void* ETHERIO_SEND_HANDLE;

//typedef enum{EN_SENDOBJ_FIFO=0, EN_SENDOBJ_BLOCK=1} EN_SENDOBJ_MODE;

//
// �������Ͷ���
//		iIndex - Ҫ���͵��豸������
//		dwBufLen - ָ�����ջ�������С�����ֽ�Ϊ��λ
//		���Ϊ���Ͷ�������0Ϊʧ��
//
// ETHERIO_API ETHERIO_SEND_HANDLE EtherIO_CreateSendObj(
// 		EN_SEND_MODE enSendMode,
// 		int iIndex, 
// 		DWORD dwBufLen);
ETHERIO_API ETHERIO_SEND_HANDLE EtherIO_CreateSendObj(
		SEND_CREATE_PARA*  pSendPara);

//
// ɾ�����Ͷ�������Ϊ�ϱߴ����ķ��Ͷ�����
//
ETHERIO_API bool EtherIO_ReleaseSendObj(IN ETHERIO_SEND_HANDLE sendHandle);

//
// �������ͣ�����Ϊ�ϱߴ����ķ��Ͷ�����
//
ETHERIO_API bool EtherIO_StartSend(
	IN ETHERIO_SEND_HANDLE sendHandle, 
	IN EN_TIME_MODE enTimeMode = EN_TIME_STAMP_MODE_LOW);

// 
// ֹͣ���ͣ�����Ϊ�ϱߴ����ķ��Ͷ�����
//
ETHERIO_API bool EtherIO_StopSend(IN ETHERIO_SEND_HANDLE sendHandle);

// 
// ȡ���͵�״̬
//		sendHandle - ���Ͷ�����
//		OUT sendInfo - ���ط��͵���Ϣ
//
ETHERIO_API bool EtherIO_GetSendInfo(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		OUT GET_SEND_INFO* pSendInfo);

/*
// 
// ���÷��͵�ʱ�侫��
//		sendHandle - ���Ͷ�����
//		enTimeMode - ʱ�侫��
//
ETHERIO_API bool EtherIO_SetSendTimeMode(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN EN_TIME_MODE enTimeMode);
*/

//
// ��������
//		sendHandle - ���Ͷ�����
//		pPacket - ���ݰ�ָ��
//		dwPktLen - ���ݰ�����
//		dwReTryTimes - ���Դ���(���ڵ�������û������)
//
ETHERIO_API bool EtherIO_SendPacket(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN PUCHAR pPacket, 
		IN DWORD dwPktLen, 
		IN DWORD dwReTryTimes);

//
// �ӷ��ͻ���������һ���ڴ�
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ����ĳ���
//		dwReTryTimes - ���Դ���
//		���ػ�������ָ��
//
ETHERIO_API PVOID EtherIO_AllocSendBuffer(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength, 
		IN DWORD dwReTryTimes);

//
// ֪ͨ���ͻ�������ʹ���˵����ݳ���
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ʹ���˵ĳ���
//		dwReTryTimes - ���Դ���
//
//	 ע�⣺ʹ�ô˺���ǰ��Ҫ�ȵ���EtherIO_AllocSendBuffer����һ����������
//
ETHERIO_API bool EtherIO_FinishedSendBuffer(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength);



//
// ��������(����ʱ��꡵ķ�ʽ)
//		sendHandle - ���Ͷ�����
//		pPacket - ���ݰ�ָ��
//		dwPktLen - ���ݰ�����
//		dwReTryTimes - ���Դ���
//
ETHERIO_API bool EtherIO_SendPacketWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN PUCHAR pPacket, 
		IN DWORD dwPktLen, 
		IN DWORD dwReTryTimes,//(���ڵ�������û������)
		IN DWORD dwOffset//(���ڵ�������û������)
		);

//
// �ӷ��ͻ���������һ���ڴ�(����ʱ��꡵ķ�ʽ)
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ����ĳ���
//		dwReTryTimes - ���Դ���
//		���ػ�������ָ��
//
ETHERIO_API PVOID EtherIO_AllocSendBufferWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength, 
		IN DWORD dwReTryTimes,
		IN DWORD dwOffset);

//
// ֪ͨ���ͻ�������ʹ���˵����ݳ���(����ʱ��꡵ķ�ʽ)
//		sendHandle - ���Ͷ�����
//		dwWriteLength - ʹ���˵ĳ���
//		dwReTryTimes - ���Դ���
//
//	 ע�⣺ʹ�ô˺���ǰ��Ҫ�ȵ���EtherIO_AllocSendBuffer����һ����������
//
ETHERIO_API bool EtherIO_FinishedSendBufferWithTimestamp(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN DWORD dwWriteLength);

//
// ����BLOCK���ͷ�ʽ�µĴ���������
//		sendHandle - ���Ͷ�����
//		pSendData - Ӧ�ó��򿪱ٲ����õ����ݻ�����ָ��
//
// ����ֵ��bool ��Ϊ int
//	0 -- �ɹ�
//	-1 -- ���ݳ��ȴ���
//	-2 -- ��ǰ�ķ���ģʽ����
//	-3 -- ��������������
ETHERIO_API int EtherIO_BlockSetData(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN BLOCK_SEND_DATA_STAMP* pSendData);


//
// ����BLOCK���ͷ�ʽ�µķ��Ͳ���
//		sendHandle - ���Ͷ�����
//		pSendData - Ӧ�ó��򿪱ٲ����õķ��Ͳ���ָ��
//
ETHERIO_API bool EtherIO_BlockSetPara(
		IN ETHERIO_SEND_HANDLE sendHandle, 
		IN BLOCK_SEND_PARA* pSendPara);


//
// ȡ���ͻ�������ʹ���ֽ���
//
//		sendHandle - ���Ͷ�����
//		OUT pTotalLen - ����������������ܳ���
// 
//	 ����ֵ
//		=0 - ��������ǰ�Ѿ�ʹ���˵ĳ���
//
//	 ˵��
//		��pTotalLen��Ϊ�յ�ʱ������������ػ��������ܳ��ȣ��ֽڣ�

ETHERIO_API DWORD EtherIO_GetSendBufferUsed(
		IN ETHERIO_RECV_HANDLE sendHandle, 
		OUT DWORD* pTotalLen);


//////////////////////////////////////////////////////////////////////////
//
// ��Դ�ͷ��࣬�������׵���
//
//////////////////////////////////////////////////////////////////////////

//
// �ͷ������������������Դ
//
ETHERIO_API void EtherIO_ResourceReleaseAll();
//
// �ͷ������б�����ռ�õ�������Դ
//
ETHERIO_API void EtherIO_ResourceReleaseAllMine();
//
// �ͷ������зǱ�����ռ�õ�������Դ
//
ETHERIO_API void EtherIO_ResourceReleaseAllOther();



//////////////////////////////////////////////////////////////////////////
//
// ȡ�忨����ͳ�ƵĽӿ�
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
��������ͷ��֡����䳤��
return:	-1 -- failed
		else -- right
*/
ETHERIO_API int EtherIO_GetMaxFrameSize(IN int iIndex);

#endif // _ETHERIO_EXPORT_H_SOMEONE_20061028__
