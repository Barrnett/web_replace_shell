#if !defined(AFX_TBLOCKMANAGER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_)
#define AFX_TBLOCKMANAGER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_

#ifdef __cplusplus

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////         /CTBlockManager/              ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//这个类是为了用户自己开辟空间和管理使用。
//这个类通过块(block)的操作，来辅助用户管理内存。
//允许用户进行增加元素和删除元素。
//但是这个类没有为增加元素和删除元素提供同步。
//如果一个线程在增加元素，另外一个线程在删除元素（或者增加元素），整个程序将会出错。
//用户应该自己提供同步机制

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

	/*分配一个块，返回一个标识符*/
	inline T* AllocBlock()
	{	return CTHashFinder<T>::AllocBlock(0,TRUE);	}
	inline T* AllocBlock(int& index)
	{	return CTHashFinder<T>::AllocBlock(0,TRUE,index);	}
	
	//如果Create函数时，如果bSupportLink＝FALSE,只能调用这两个函数来遍历
	//EOL(index)表示结束
	inline INT_PTR GetFirstBlock() const
	{	return CTHashFinder<T>::GetFindHeadBlock(0);		}
	inline T* GetNextBlock(INT_PTR & index ) const
	{	return	CTHashFinder<T>::GetFindNextBlock(index);	}


	//如果在Create函数时，bSupportLink＝TRUE,可以调用GetFirstBlock，GetNextBlock这两个函数来遍历

	//还可以调用父类的GetHeadBlock，GetNextBlockFromHead	从最旧的到最新的
	//还可以调用父类的GetTailBlock GetNextBlockFromTail 从最新的到最旧的;
	
	
protected:
	CTBlockManager(const CTBlockManager<T>& bgm);
	CTBlockManager<T>& operator = (const CTBlockManager<T>& bgm);
	
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////*******************CTBlockManager*******************////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __cplusplus

#endif // !defined(AFX_TBLOCKMANAGER_H__2F43062B_5DB9_4A06_A250_7F84A7A70CAA__INCLUDED_)


