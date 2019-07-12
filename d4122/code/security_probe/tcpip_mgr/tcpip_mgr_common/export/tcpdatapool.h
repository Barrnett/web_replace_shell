#ifndef  TCPDATAPOOL_H
#define  TCPDATAPOOL_H
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"
#include "tcpip_mgr_common.h"
#include "cpf/Octets.h"

//数据缓冲结构
struct MEMPOOL{
	int			nBuflen;//存储数据的长度,
	int 		nDatalen;//tcp数据长度，用于判断是否丢包或者重复包
	ACE_UINT32	seqnum;//tcp包序号
	ACE_UINT32	reserved;//占位，为了后面8字节对齐

	char 		pBuffer[1];//数据指针

};

#include "cpf/DataArray.h"

class TCPIP_MGR_COMMON_CLASS tcpdatapool  
{

public:
	tcpdatapool();
	virtual ~tcpdatapool();
public:
	//释放资源并初始化
	void Close();
	//加入数据
	void Add(char *data,size_t datalen,size_t tcpdatalen,ACE_UINT32 seqnum);
	//链表是否为空
	BOOL IsEmpty() const{return m_datalen==0;}
	//数据长度
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

	//TCP包排序
	void Handle(CDataArray &array, int nNum = 10);

	static int Icmp(const void *p1,const void *p2);

	//------------------------------------------------------------------------
	// Function:	Merge()
	// Purpose:	
	// Parameters:	pcbOut	-		[out]指向保存返回内容的buffer的地址
	//				ncbBuf	-		[in]The size of the input buffer
	//				nNum	-		[in] 重传的数据包的个数，默认值为10；0表示不管
	//									丢多少包都要重新对数据包排序，大于10不对数据包排序
	//									小于10要对数据包进行排序
	// Returns:		>=0		-		写入的数据的字节数目.
	//				-1		-		ERROR
	// Note:		
	// Author:
	//------------------------------------------------------------------------
	int Merge(char *pcbOut, int ncbBuf,int nNum = 10);

	int Merge(CCPFOctets& myoct,int nNum = 10);

private:
	//数据长度
	size_t m_datalen;
	//指针数组
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
