/*=========================================================
*	File name	：	tcpdatapool.cpp
*	Authored by	：	xuxinyao
*	Date		：	2003-2-11 16:54:34
*	Description	：	主要用来tcp数据包排序
*
*	Modify  	：	
*=========================================================*/

#include "tcpdatapool.h"
#include "cpf/memmgr.h"
#include "tcpip_mgr_common.h"
#include "tcpip_mgr_common_init.h"


tcpdatapool::tcpdatapool():m_dataArray(0,260)
{
	m_datalen = 0;
}

tcpdatapool::~tcpdatapool()
{
	Close();

}

void tcpdatapool::Close()
{
	int size = m_dataArray.GetSize();
	for(int i =0;i<size;i++)
	{
		MEMPOOL *p = (MEMPOOL *)(m_dataArray[i]);
		CPF_MEM_FREE(p);
	}
	m_dataArray.Clear();
	m_datalen = 0;

}

 void tcpdatapool::Add(char *data,size_t datalen,size_t tcpdatalen,ACE_UINT32 seqnum)
{
	if(datalen<=0)
		return;

	MEMPOOL *p =(MEMPOOL*)CPF_MEM_ALLOC(sizeof(MEMPOOL)+datalen+1);
	if(p == NULL)
	{
		ACE_ASSERT(false);
		return;
	}

	p->nBuflen = datalen;
	memcpy(p->pBuffer,data,datalen);
	p->pBuffer[datalen] = '\0';
	p->seqnum = seqnum;
	p->nDatalen = tcpdatalen;

	m_dataArray.Add((UINT_PTR)(p));
	m_datalen += datalen;

}

/*==========================================================
* Function	    : tcpdatapool::Handle
* Description	: 
* Return	    : void 
* Parament		: CDataArray &array
* Parament		: int nNum			如果被排序的总包数小于nNum，支持排序系数小于0.8
* Comments		: 
*=========================================================*/
/*
 void tcpdatapool::Handle(CDataArray &array, int nNum)
 {
	 //ASSERT(pArray);
	 array.Clear();
	 int size = m_dataArray.GetSize();
	 if(size<=0)
		 return;


	 qsort(m_dataArray.GetData(),size,sizeof(UINT_PTR),Icmp);

	 MEMPOOL *p1 = NULL;
	 MEMPOOL *p2 = NULL;
	 MEMPOOL *p3 = NULL;

	 size--;

	 p1 = GetAt(size);
	 ACE_UINT32 curseqnum = p1->seqnum;

	 while((size-1)>=0)
	 {
		 p2 = GetAt(size-1);
		 if( SEQ_LEQ(p2->seqnum+p2->nDatalen,p1->seqnum+p1->nDatalen) )
			 break;
		 p1 = p2;
		 size--;
	 }

	 curseqnum =  p1->seqnum;
	 array.Add(UINT_PTR(p1));
	 int j = 0;
	 int index = 0;

	 for(int i = size-1;i>=0;i--)
	 {
		 p2 = GetAt(i);
		 if(p2->seqnum!=curseqnum)
		 {
			 if(p2->seqnum + p2->nDatalen==curseqnum+p1->nDatalen)
			 {
				 j++;
				 continue;
			 }

			 if(p2->seqnum + p2->nDatalen != curseqnum)
			 {
				 while((j--)>0)
				 {
					 p1 = GetAt(i+j+1);
					 if(p1->seqnum == (p2->seqnum + p2->nDatalen))
					 {
						 array.SetAt(index,UINT_PTR(p1));
						 curseqnum = p1->seqnum;
						 i++;
						 break;
					 }

				 }

				 if(SEQ_GT(p2->seqnum + p2->nDatalen , curseqnum) )
				 {

					 if(i>0)
					 {
						 p3 = GetAt(i-1);
						 if(p3->seqnum+p3->nDatalen==p2->seqnum)
						 {
							 p2->nBuflen -= (p2->seqnum + p2->nDatalen-curseqnum);
							 if(p2->nBuflen<0)
								 p2->nBuflen = 0;
							 p2->pBuffer[p2->nBuflen] = '\0';
							 p1 = p2;
							 index = array.Add(UINT_PTR(p1));
							 curseqnum = p1->seqnum;
						 }
					 }
					 //DUMPTEXT("%s\r\n",p2->pcbData);//, pEntry->dwID);
				 }


			 }

			 else
			 {
				 p1 = p2;
				 index = array.Add(UINT_PTR(p1));
				 curseqnum = p1->seqnum;
			 }
			 j = 0;
		 }
	 }

	 //主要防止数据不完整或别的原因造成数据重组不完整时恢复原数据
	 int size1 = m_dataArray.GetSize();
	 int size2 = array.GetSize();
	 float temp = float(size2)/size1;
	 if(temp<0.8 && size1>nNum && nNum != 0)
	 {
		 array.Clear();
		 for( int i = size1-1;i>=0;i--)
		 {
			 p1 = GetAt(i);
			 array.Add(UINT_PTR(p1));
		 }
	 }

	 //if()
 }
*/

void tcpdatapool::Handle(CDataArray &array, int nNum)
{
	//ASSERT(pArray);
	array.Clear();
	int size = m_dataArray.GetSize();
	if(size<=0)
		return;

	if( size > 1 )
	{
		//先按照seq进行排序,保证前面包的seq<=后面包的seq.
		qsort(m_dataArray.GetData(),size,sizeof(UINT_PTR),Icmp);
	}

	//p1为当前节点
	MEMPOOL * p1 = GetAt(size-1);

	ACE_UINT32 curseqnum = p1->seqnum;

	array.Add(UINT_PTR(p1));

	for(int i = size-2;i>=0;--i)
	{
		MEMPOOL * p2 = GetAt(i);

		if(p2->seqnum==curseqnum)
		{//重复包
			continue;
		}

		int cmp = SEQ_CMP(p2->seqnum + p2->nDatalen,curseqnum);

		if(cmp == 0)
		{//完全符合顺序

			p1 = p2;
			array.Add(UINT_PTR(p1));
			curseqnum = p1->seqnum;
		}
		else if( cmp < 0 )
		{//出现丢包

		}
		else
		{//出现交叉覆盖

			p2->nBuflen -= (p2->seqnum + p2->nDatalen-curseqnum);

			if( p2->nBuflen < 0 )
				p2->nBuflen = 0;

			p1 = p2;
			array.Add(UINT_PTR(p1));
			curseqnum = p1->seqnum;
		}
	}

	//主要防止数据不完整或别的原因造成数据重组不完整时恢复原数据
	int size1 = m_dataArray.GetSize();
	int size2 = array.GetSize();
	float temp = float(size2)/size1;
	if(temp<0.8 && size1>nNum && nNum != 0)
	{
		array.Clear();
		for( int i = size1-1;i>=0;--i)
		{
			p1 = GetAt(i);
			array.Add(UINT_PTR(p1));
		}
	}

	//if()
}

//------------------------------------------------------------------------
// Function:	Merge()
// Purpose:	
// Parameters:	pcbOut	-		[out]指向保存返回内容的buffer的地址
//				ncbBuf	-		[in]The size of the input buffer
// Returns:		>=0		-		写入的数据的字节数目.
//				-1		-		ERROR
// Note:		
// Author:
//------------------------------------------------------------------------
int tcpdatapool::Merge(char *pcbOut, int ncbBuf,int nNum)
{
	if (NULL == pcbOut || 0 >= ncbBuf)
	{
		ACE_ASSERT(FALSE);
		return -1;
	}

	if ( IsEmpty())
	{
		return 0;	// There is no data
	}

	CDataArray  out_dataArray(m_dataArray.GetSize());

	Handle(out_dataArray,nNum);

	int nPos = 0;

	for (int i = out_dataArray.GetSize() - 1; i >= 0 ; --i)
	{
		MEMPOOL * pEntry = (MEMPOOL *)out_dataArray.GetAt(i);

		if ((nPos + pEntry->nBuflen) > ncbBuf)
			break ;

		memcpy(pcbOut + nPos, pEntry->pBuffer, pEntry->nBuflen);

		nPos += pEntry->nBuflen;		
	}

	if( nPos == ncbBuf )
	{
		*(pcbOut + nPos - 1) = '\0';
	}
	else
	{
		*(pcbOut + nPos) = '\0';
	}

	return nPos;
}

int tcpdatapool::Merge(CCPFOctets& myoct,int nNum)
{
	myoct.Free();

	if ( IsEmpty())
	{
		return 0;	// There is no data
	}

	CDataArray  out_dataArray(m_dataArray.GetSize());

	Handle(out_dataArray,nNum);

	for (int i = out_dataArray.GetSize() - 1; i >= 0 ; --i)
	{
		MEMPOOL * pEntry = (MEMPOOL *)out_dataArray.GetAt(i);

		myoct.Append((BYTE *) pEntry->pBuffer, (UINT)pEntry->nBuflen);
	}

	myoct.Append((char)'\0');

	return myoct.GetDataLength();
}
