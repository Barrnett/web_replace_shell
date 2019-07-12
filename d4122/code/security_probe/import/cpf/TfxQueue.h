//////////////////////////////////////////////////////////////////////////
//TfxQueue.h

#include "cpf/cpf.h"

#ifndef  __TFX__QUEUE__H__
#define  __TFX__QUEUE__H__ 

#pragma  once


template <class T>
class CTfxQueue
{
public:
	CTfxQueue()
	{
		this->m_buff_len = 0;
		this->m_nFront = 0;
		this->m_nTail = 0;
		this->m_max_len = 0 ;

		this->m_bFreeMem = false;

		this->m_buff = NULL;

	}

	CTfxQueue(const int maxitem, T * buff= NULL)
	{
		this->m_buff_len = 0;
		this->m_nFront = 0;
		this->m_nTail = 0;
		this->m_max_len = 0 ;

		this->m_buff = NULL;
		this->m_bFreeMem = false;

		Create(maxitem, NULL);
	}

	// 析钩子
	~CTfxQueue()
	{
		Destroy();
	}

	int Create(const int maxitem, T * buff= NULL)
	{
		int iret = 0;

		m_max_len = maxitem;
		
		if ( m_max_len < 1 ) 
			m_max_len = 100;

		if (buff == NULL ) // 由内部申请空间
		{
			m_bFreeMem = TRUE;

			this->m_buff = new T[m_max_len];

			if (this->m_buff == NULL) return -1;
		}
		else // 缓存空间由外部传入, 此时在类析构时不负责删除缓存
		{

			m_bFreeMem = FALSE;
			this->m_buff = buff;
		}

		return 0;

	}

	void Destroy()
	{
		if (m_bFreeMem && m_buff ) 
		{
			delete [] m_buff ;
		}

		m_buff = NULL;
		m_bFreeMem = false;

		Clear();

		return;
	}


	const T* GetBuffer() const
	{
		return m_buff;
	}

	T* GetBuffer()
	{
		return m_buff;
	}
		
public:

	// 推入一个元素,如果元素满了，返回-1
	//如果正确返回0
	int Push(const T & a_data)    
	{
		if ( this->IsFull() ) return -1;

		m_buff[m_nTail] = a_data;
		m_nTail = ( m_nTail + 1) % m_max_len;

		m_buff_len ++;

		return 0;
	}

	// 推入一个元素,如果元素满了，则pop一个，然后继续添加
	void Push_force(const T & a_data)
	{
		if ( this->IsFull() )
		{
			Pop();
		}

		Push(a_data);
	}

	// 弹出一个元素
	int Pop(T& a_data)     
	{
		if ( this->IsEmpty() )
			return -1;

		a_data = m_buff[m_nFront];
		m_nFront = ( m_nFront + 1) % m_max_len;

		m_buff_len --;

		return 0;
	}

	T& Pop()     
	{
		if ( this->IsEmpty() )
		{
			ACE_ASSERT(FALSE);
		}

		T& data = m_buff[m_nFront];
		m_nFront = ( m_nFront + 1) % m_max_len;

		m_buff_len --;

		return data;
	}

	// 推入一组元素
	int PushBuff(const T *  p_data, int len) 
	{
		int li_pushed_len = 0;

		for ( int i = 0 ; i < len ; i ++)
		{
			int ret2 = Push(p_data[i]);

			if ( ret2 != 0) break;

			li_pushed_len ++;
		}

		return li_pushed_len;
	}

	int PushBuff_force(const T *  p_data, int len) 
	{
		int li_pushed_len = 0;

		for ( int i = 0 ; i < len ; i ++)
		{
			Push_force(p_data[i]);

			li_pushed_len ++;
		}

		return li_pushed_len;
	}

	// 弹出一组元素
	int PopBuff(T *  p_data, int len) 
	{
		int li_poped_len = mymin(len,GetCurCount());

		for ( int i = 0 ; i < li_poped_len ; i ++)
		{
			Pop(*(p_data + li_poped_len) );
		}

		return li_poped_len;
	}

	//遍历当前所有的，但是不删除
	T * GetData(int index) const
	{
		if( index < 0 || index >= GetCurCount() )
			return NULL;

		return &m_buff[(m_nFront + index)%m_max_len];
	}

	//得到第一个数据
	T * GetHead() const
	{
		return GetData(0);
	}

	//得到最后一个数据
	T * GetTail() const
	{
		return GetData(GetCurCount()-1);
	}

	// 清除缓存内容
	int Clear()      
	{
		this->m_buff_len = 0;
		this->m_nFront = 0;
		this->m_nTail = 0;

		return 0;
	}

	// 获得缓存最大长度
	int GetMaxItem() const   
	{
		return (this->m_max_len);
	}

	// 获得实际数据长度
	int GetCurCount() const     
	{
		return this->m_buff_len;
	}

	// 当前缓存是否为空
	BOOL IsEmpty() const    
	{
		return (this->m_buff_len == 0 );
	}

	// 当前缓存是否已满
	BOOL IsFull() const   
	{
		return (this->m_buff_len >= this->m_max_len );
	}

	void RemoveAt(int index)
	{
		for(int i = index+1; i < GetCurCount(); ++i)
		{
			*GetData(i-1) = *GetData(i);
		}

		m_nTail = ( m_nTail - 1 + m_max_len) % m_max_len;

		m_buff_len --;
	}

public:
	//用于流量计算，得到前后两个的值，可以做差值.index表示第几个差值，从0开始。
	//index=0表示a1-a0,index=1表示a2-a1.
	BOOL GetDiffData(int index,T*& data1,T*& data2)
	{
		if( index < 0 || index >= GetCurCount() -1 )
			return false;

		data1 = GetData(index);
		data2 = GetData(index+1);

		return true;
	}

	BOOL GetDiffData(int index,T& data1,T& data2)
	{
		if( index < 0 || index >= GetCurCount() -1 )
			return false;

		data1 = *(GetData(index));
		data2 = *(GetData(index+1));

		return true;
	}

	int GetCurDiffCount() const     
	{
		if( GetCurCount() <= 0 )
			return 0;

		return this->m_buff_len-1;
	}

	int GetMaxDiffItem() const   
	{
		return (GetMaxItem()-1);
	}

	// 当前缓存是否为空
	BOOL IsDiffEmpty() const    
	{
		return (GetCurDiffCount() == 0 );
	}

	BOOL IsDiffFull() const   
	{
		return IsFull();
	}

private:


	T  *m_buff;

	int  m_nFront;    // 队首
	int  m_nTail;    // 对尾巴
	int  m_buff_len;    // 缓存长度
	int  m_max_len;    // 最长长度

	BOOL    m_bFreeMem;    // 是否由内部是否内存


};


#include "cpf/flux_tools.h"

// 常规定义
typedef  CTfxQueue<BYTE>			CTfxByteBuffer;
typedef  CTfxQueue<WORD>			CTfxWordBuffer;
typedef  CTfxQueue<DWORD>			CTfxDwordBuffer;
typedef  CTfxQueue<long>			CTfxLongBuffer;
typedef  CTfxQueue<int>				CTfxIntBuffer;
typedef  CTfxQueue<ACE_UINT64>		CTfxUint64Buffer;
typedef  CTfxQueue<ACE_UINT32>		CTfxUint32Buffer;
typedef  CTfxQueue<STAT_SI_ITEM>	CTfx_STAT_SI_ITEM_Buffer;

//对于类型T，如果支持T1-T2可以使用下面的类来获取差值，这样使用更方便一些
template <class T>
class CTfxQueueEx : public CTfxQueue<T>
{
public:
	T GetDiffDataEx(int index)
	{
		T* data1 = NULL;
		T* data2 = NULL;

		if( !CTfxQueue<T>::GetDiffData(index,data1,data2) )
		{
			ACE_ASSERT(FALSE);
		}

		return (*data2 - *data1);
	}

};


typedef  CTfxQueueEx<BYTE>				CTfxExByteBuffer;
typedef  CTfxQueueEx<WORD>				CTfxExWordBuffer;
typedef  CTfxQueueEx<DWORD>				CTfxExDwordBuffer;
typedef  CTfxQueueEx<long>				CTfxExLongBuffer;
typedef  CTfxQueueEx<int>				CTfxExIntBuffer;
typedef  CTfxQueueEx<ACE_UINT64>		CTfxExUint64Buffer;
typedef  CTfxQueueEx<ACE_UINT32>		CTfxExUint32Buffer;
typedef  CTfxQueueEx<STAT_SI_ITEM>		CTfxEx_STAT_SI_ITEM_Buffer;

//////////////////////////////////////////////////////////////////////////
#endif//__TFX__QUEUE__H__

