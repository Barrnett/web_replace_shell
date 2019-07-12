#if !defined(AFX_TBLOCKMANAGER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_)
#define AFX_TBLOCKMANAGER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_

#ifdef __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////         /CTBlockManager/              ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//�������Ϊ���û��Լ����ٿռ�͹���ʹ�á�
//�����ͨ����(block)�Ĳ������������û������ڴ档
//�����û���������Ԫ�غ�ɾ��Ԫ�ء�
//���������û��Ϊ����Ԫ�غ�ɾ��Ԫ���ṩͬ����
//���һ���߳�������Ԫ�أ�����һ���߳���ɾ��Ԫ�أ���������Ԫ�أ����������򽫻����
//�û�Ӧ���Լ��ṩͬ������

#include "THashFinder.h"

template<class T>
class CTBlockManager : public CTHashFinder<T>
{
public:
	CTBlockManager()
	{return;}
	
	~CTBlockManager()
	{	return; }
	
public:
	inline BOOL Create(int blocknums,bool bSupportLink = FALSE,T* pData=NULL)
	{	return CTHashFinder<T>::Create(blocknums,1,bSupportLink,pData);		}

	/*����һ���飬����һ����ʶ��*/
	inline T* AllocBlock()
	{	return CTHashFinder<T>::AllocBlock(0,TRUE);	}
	inline T* AllocBlock(int& index)
	{	return CTHashFinder<T>::AllocBlock(0,TRUE,index);	}
	
	//���Create����ʱ�����bSupportLink��FALSE,ֻ�ܵ�������������������
	//EOL(index)��ʾ����
	inline INT_PTR GetFirstBlock() const
	{	return CTHashFinder<T>::GetFindHeadBlock(0);		}
	inline T* GetNextBlock(INT_PTR & index ) const
	{	return	CTHashFinder<T>::GetFindNextBlock(index);	}


	//�����Create����ʱ��bSupportLink��TRUE,���Ե���GetFirstBlock��GetNextBlock����������������

	//�����Ե��ø����GetHeadBlock��GetNextBlockFromHead	����ɵĵ����µ�
	//�����Ե��ø����GetTailBlock GetNextBlockFromTail �����µĵ���ɵ�;
	
	
protected:
	CTBlockManager(const CTBlockManager<T>& bgm);
	CTBlockManager<T>& operator = (const CTBlockManager<T>& bgm);
	
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////*******************CTBlockManager*******************////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __cplusplus

#endif // !defined(AFX_TBLOCKMANAGER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_)


