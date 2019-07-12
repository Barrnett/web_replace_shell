/*******************************************************************************
  CapSave.h

	�� �� ��:	�����
	����ʱ��:	2002/8/20 
	����������  �����ļ�ͷ��ض��塣


		�ļ��ṹ��

�����������������������������Щ����Щ����Щ����Щ����������������Щ��������Щ����Щ�����������
���ļ�ͷ��CAPFILE_HEAD�ṹ����֡1 ��... ��֡n ��֡������־0xFFFF������ֵ1 ��... �� ����ֵm  ��
�����������������������������ة����ة����ة����ة����������������ة��������ة����ة�����������
			
		�����ṹ��

		ÿ��nIndexStep��֡����һ������ֵ������֡��ţ�nIndexStep*��ǰ�����±꣬
		����ֵΪ��֡���ļ��е�λ�á�����ֵΪ0ʱ�Ƚ����⣬����֡���ļ��е�ʵ��λ�ã�
		��֡Ӧ�����ļ�ͷ��


	�޸ļ�¼��
	�޸�ʱ��	    �޸���		�޸�����

*********************************************************************************/


#ifndef _CAPFILEHEAD_H_
#define _CAPFILEHEAD_H_
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"
#include "cpf/TimeStamp32.h"


#if defined(OS_WINDOWS)
#define _MAX_ULONGLONG  0xffffffffffffffff
#define _MIN_ULONGLONG  0x0
#else
#define _MAX_ULONGLONG  0xffffffffffffffffull
#define _MIN_ULONGLONG  0x0ull
#endif
#define _MAX_DWORD      0xffffffff
#define _MIN_DWORD      0x0
#define _MAX_WORD	0xffff
#define _MIN_WORD	0x0
#define _MAX_SHORT	0x7fff
#define _MIN_SHORT	(-0x7fff)
#define _MAX_LONG       0x7fffffff
#define _MIN_LONG       (-0x7fffffff)
#define _MAX_INT        0x7fffffff
#define _MIN_INT        (-0x7fffffff)

#if defined(OS_LINUX)
typedef void *				HANDLE;
typedef int               	FILE_ID;
#define ZCTT_INVALID_HANDLE	(-1)
#define CLOSEFILE(x)		close(x)
#define CLOSE_MAP(x)		
#elif defined(OS_WINDOWS)

typedef HANDLE             	FILE_ID;
#define ZCTT_INVALID_HANDLE	INVALID_HANDLE_VALUE
#define CLOSEFILE(x)		CloseHandle(x)
#define CLOSE_MAP(x)		CloseHandle(x)	
typedef LONGLONG            off64_t;
typedef LONGLONG            loff_t;
#endif//#if defined(OS_LINUX)

#define FILENAME_LEN1AND2	100
#define FILENAME_LEN3		50

#define CAPFILE_ID1		"ZCXC CPT 1"
#define CAPFILE_ID2		"ZCXC CPT 2"
#define CAPFILE_ID3		"ZCXC CPT 3"
#define CAPFILE_ID4		"ZCXC CPT 4"

#define MAX_RAW_PACKETLEN		4096    //���ԭʼ������

#pragma pack(1)

typedef struct
{
	BYTE 			btHeaderLength;//HEADER_INFO
	BYTE			btVersion;
	WORD			wdProto;
	BYTE 			btHardwareType;
	BYTE 			btBureauNo;
	BYTE 			btFrontMachineNo;
	BYTE 			btCardNo;
	BYTE 			btInterfaceNo;

	struct
	{
		BYTE Ex:1;
		BYTE Tx:1;
		BYTE Rx:1;
		BYTE Re:1;
		BYTE L:1;
		BYTE Ts:1;
		BYTE Sp1:1;
		BYTE Sp2:1;
	} btFlag;

	DWORD 			dwError;
	TIMESTAMP32 	stamp;

	void* 			pHeaderSpec;//ҲҪ����

	
}HEADER_INFO;

#pragma pack()

typedef struct	tagPACKET_CONTEXT
{
	tagPACKET_CONTEXT()
	{
		dwLengthPacket = 0;
		pPacket = NULL;
		dwOffsetBit = 0;
		dwAddiBitLen = 0;
	}

	DWORD			dwLengthPacket;	//���ݰ����ֽڳ���
	BYTE*  			pPacket;			//���ݰ����׵�ַ
	HEADER_INFO*	pHeaderInfo;		//���ݰ�ͷ����Ϣ

	DWORD			dwOffsetBit;//Ϊ��֧��3G��λ���룬�ϲ�Э������ݵĿ�ʼλ����pPacket��dwOffsetBit(λ)
	DWORD			dwAddiBitLen;//Ϊ��֧��3G��λ���룬�ϲ�Э������ݳ�����(dwLengthPacket*8+dwAddiBitLen)(λ��


}PACKET_CONTEXT;


#pragma pack(1)


typedef struct 
{
	char	ID[10];						// �����ļ���ʽ��ʶ CAPFILE_ID1
	ACE_UINT32	timeBegin;					// ��ʼ��׽��ʱ��
	ACE_UINT32	timeEnd;					// ֹͣ��׽��ʱ��
	BOOL	bSerialFiles;				// �Ƿ���һϵ�е��ļ��е�һ��
	DWORD	dwPacketCount;				// ��¼�İ��ĸ���
	int		nNo;						// �ǵڼ����ļ�,��0��ʼ
	char	PrevFileName[FILENAME_LEN3]; // ǰһ���ļ���
	char	NextFileName[FILENAME_LEN3]; // ��һ���ļ���
	DWORD	dwIndexStep;				// ���ٸ�֡����һ������
	DWORD	dwIndexCount;				// ����ֵ����
	DWORD	dwIndexPosInFile;			// �ļ��д�������Ŀ�ʼλ��
	BOOL	bFileCreating;				// �ļ����ڴ����У���һ��д������ʹ����
	DWORD	dwAddtionDataCount;			// �������ݸ���
	DWORD	dwAddtionDataPos;			// �������ݿ�ʼλ��
	char	Reserved[100];				// ����,������0���
	WORD	dwHeadCheckSum;				// ͷ��У���,ʹ�ú���CPTFileHeadComputeCheckSum
} CAPFILE_HEAD;		// �����ļ�ͷ��ʽ��Size=256�ֽ�

#pragma pack()

#define CAPFILE_FRAME_END_FLAG	0xFFFF			// ��������־
#define CPTFILE_FRAME_END_STRING "CPT PKT END"	// ��������,CPT �汾 3 �Ժ�ʹ��,����CAPFILE_FRAME_END_FLAG��
#define CPTFILE_FRAME_END_STRING_LEN	12		// ���������ĳ���,CPT �汾 3 �Ժ�ʹ��

// ÿ��д�������Сʣ��ռ䣨���С��������Ϊ�ļ�����
// ���а��������˷�4��CPTFILE_FRAME_END_STRING�ĳ��ȺͰ�������־2��
#define CPTFILE_MIN_FREE	(CPTFILE_FRAME_END_STRING_LEN+8)	

// CPT�ļ��е���󸽼������� 100 Mb��д�����CPT�ļ���ʱ��
// �������4G-CPTFILE_MAX_ADDTION_DATA��Ҳ���ļ�д����
// �Ա��Ժ���븽�����ݺ󣬸��ļ�Ҳ���� 4 GB ����
// Ҳ����˵��û�и������ݵ�CPT�ļ����Ϊ 4GB - CPTFILE_MAX_ADDTION_DATA
// �и������ݵ�һ��CPT�ļ����ᳬ�� 4GB����¼����4GB�����ݿ���ʹ��
// ������¼���CPT�ļ��ķ����� 
#define CPTFILE_MAX_ADDTION_DATA	(100*1024*1024) 

#define DEFAULT_INDEX_STEP	1000		// ȱʡ���ٸ�֡����һ������



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#endif//_CAPFILEHEAD_H_

