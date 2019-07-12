#ifndef  TCPDATAPOOL_H
#define  TCPDATAPOOL_H
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "tcpip_mgr_common.h"
#include "cpf/Octets.h"

//���ݻ���ṹ
struct MEMPOOL{
	int			nBuflen;//�洢���ݵĳ���,
	int 		nDatalen;//tcp���ݳ��ȣ������ж��Ƿ񶪰������ظ���
	ACE_UINT32	seqnum;//tcp�����
	ACE_UINT32	reserved;//ռλ��Ϊ�˺���8�ֽڶ���

	char 		pBuffer[1];//����ָ��

};

#include "cpf/DataArray.h"

class TCPIP_MGR_COMMON_CLASS tcpdatapool  
{

public:
	tcpdatapool();
	virtual ~tcpdatapool();
public:
	//�ͷ���Դ����ʼ��
	void Close();
	//��������
	void Add(char *data,size_t datalen,size_t tcpdatalen,ACE_UINT32 seqnum);
	//�����Ƿ�Ϊ��
	BOOL IsEmpty() const{return m_datalen==0;}
	//���ݳ���
	size_t  GetDataLen() const {return m_datalen;}

	int		GetSize() const {  return m_dataArray.GetSize();	}

    inline MEMPOOL *GetAt (int pos) const
	{
#ifdef _DEBUG
		int size = GetSize();
		ACE_ASSERT(pos>=0&&pos<size);
#endif
		return  (MEMPOOL *)(m_dataArray[pos]);
	}

	//TCP������
	void Handle(CDataArray &array, int nNum = 10);

	static int Icmp(const void *p1,const void *p2);

	//------------------------------------------------------------------------
	// Function:	Merge()
	// Purpose:	
	// Parameters:	pcbOut	-		[out]ָ�򱣴淵�����ݵ�buffer�ĵ�ַ
	//				ncbBuf	-		[in]The size of the input buffer
	//				nNum	-		[in] �ش������ݰ��ĸ�����Ĭ��ֵΪ10��0��ʾ����
	//									�����ٰ���Ҫ���¶����ݰ����򣬴���10�������ݰ�����
	//									С��10Ҫ�����ݰ���������
	// Returns:		>=0		-		д������ݵ��ֽ���Ŀ.
	//				-1		-		ERROR
	// Note:		
	// Author:
	//------------------------------------------------------------------------
	int Merge(char *pcbOut, int ncbBuf,int nNum = 10);

	int Merge(CCPFOctets& myoct,int nNum = 10);

private:
	//���ݳ���
	size_t m_datalen;
	//ָ������
	CDataArray  m_dataArray;

};

inline	int tcpdatapool::Icmp(const void *p1,const void *p2)
{
	//MEMPOOL *v1,*v2;
	//v1 = (MEMPOOL *)(*(int*)p1);
	//v2 = (MEMPOOL *)(*(int*)p2);
	MEMPOOL * v1 = *(MEMPOOL **)p1;
	MEMPOOL * v2 = *(MEMPOOL **)p2;

	if(v1->seqnum==v2->seqnum)
		return 0;

	if (SEQ_GT(v1->seqnum ,v2->seqnum) )
		return 1;

	return -1;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif // TCPDATAPOOL_H
