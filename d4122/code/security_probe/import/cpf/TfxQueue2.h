//////////////////////////////////////////////////////////////////////////
//TfxQueue2.h

#pragma  once

#include "cpf/cpf.h"
#include "cpf/TfxQueue.h"


template <class T>
class TfxQueue2
{
public:
	TfxQueue2()
	{
	}

	TfxQueue2(const int maxitem, T * buff= NULL)
		:m_data_queue(maxitem,buff),m_status_queue(maxitem)
	{
	}

	int Create(const int maxitem, T * buff= NULL)
	{
		m_data_queue.Create(maxitem,buff);
		m_status_queue.Create(maxitem);
	
		return 0;
	}

	void Destroy()
	{
		m_data_queue.Destroy();
		m_status_queue.Destroy();

		return;
	}

	// 析钩子
	~TfxQueue2()
	{
		Destroy();
	}

	// 推入一个元素,如果元素满了，返回-1
	//如果正确返回0
	int Push(const T & a_data)    
	{
		m_data_queue.Push(a_data);
		m_status_queue.Push(true);
	}

	int PushEmptyData()
	{
		if( 0 == m_status_queue.Push(false) )
		{
			T a_data;

			m_data_queue.Push(a_data);

			return 0;
		}

		return -1;
		
	}

	// 推入一个元素,如果元素满了，则pop一个，然后继续添加
	void Push_force(const T & a_data)
	{
		m_data_queue.Push_force(a_data);
		m_status_queue.Push_force(true);
		
	}

	void PushEmptyData_force()
	{
		T a_data;

		m_data_queue.Push_force(a_data);
		m_status_queue.Push_force(false);

	}

	// 弹出一个元素
	int Pop(T& a_data,BOOL& bValidData)     
	{
		if ( this->IsEmpty() )
			return -1;

		m_data_queue.Pop(a_data);
		m_status_queue.Pop(bValidData);

		return 0;
	}

	T& Pop(BOOL& bValidData)     
	{
		if ( this->IsEmpty() )
		{
			ACE_ASSERT(FALSE);
		}

		m_status_queue.Pop(bValidData);

		return m_data_queue.Pop();
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

	//遍历当前所有的，但是不删除
	T * GetData(int index,BOOL& bValidData) const
	{
		if( index < 0 || index >= GetCurCount() )
			return NULL;

		bValidData = *m_status_queue.GetData(index);

		return m_data_queue.GetData(index);
	}

	//得到第一个数据
	T * GetHead(BOOL& bValidData) const
	{
		return GetData(0,bValidData);
	}

	//得到最后一个数据
	T * GetTail(BOOL& bValidData) const
	{
		return GetData(GetCurCount()-1,bValidData);
	}

	// 清除缓存内容
	int Clear()      
	{
		m_data_queue.Clear();
		m_status_queue.Clear();

		return 0;
	}

	// 获得缓存最大长度
	int GetMaxItem() const   
	{
		return m_data_queue.GetMaxItem();
	}

	// 获得实际数据长度
	int GetCurCount() const     
	{
		return m_data_queue.GetCurCount();
	}

	// 当前缓存是否为空
	BOOL IsEmpty() const    
	{
		return m_data_queue.IsEmpty();
	}

	// 当前缓存是否已满
	BOOL IsFull() const   
	{
		return m_data_queue.IsFull();
	}

public:
	//用于流量计算，得到前后两个的值，可以做差值.index表示第几个差值，从0开始。
	//index=0表示a1-a0,index=1表示a2-a1.
	BOOL GetDiffData(int index,T*& data1,BOOL& bValidData1,T*& data2,BOOL& bValidData2)
	{
		if( index < 0 || index >= GetCurCount() -1 )
			return false;

		data1 = GetData(index,bValidData1);
		data2 = GetData(index+1,bValidData2);

		return true;
	}

	BOOL GetDiffData(int index,T& data1,BOOL& bValidData1,T& data2,BOOL& bValidData2)
	{
		if( index < 0 || index >= GetCurCount() -1 )
			return false;

		data1 = *(GetData(index,bValidData1));
		data2 = *(GetData(index+1,bValidData2));

		return true;
	}

	int GetCurDiffCount() const     
	{
		return m_data_queue.GetCurDiffCount();
	}

	int GetMaxDiffItem() const   
	{
		return m_data_queue.GetMaxDiffItem();
	}

	// 当前缓存是否为空
	BOOL IsDiffEmpty() const    
	{
		return m_data_queue.IsDiffEmpty();
	}

	BOOL IsDiffFull() const   
	{
		return m_data_queue.IsDiffFull();
	}

public:
	CTfxQueue<T>	m_data_queue;
	CTfxQueue<BOOL>	m_status_queue;

};


// 常规定义
typedef  TfxQueue2<BYTE>			CTfx2ByteBuffer;
typedef  TfxQueue2<WORD>			CTfx2WordBuffer;
typedef  TfxQueue2<DWORD>			CTfx2DwordBuffer;
typedef  TfxQueue2<long>			CTfx2LongBuffer;
typedef  TfxQueue2<int>			    CTfx2IntBuffer;
typedef  TfxQueue2<ACE_UINT64>		CTfx2Uint64Buffer;
typedef  TfxQueue2<ACE_UINT32>		CTfx2Uint32Buffer;
typedef  TfxQueue2<STAT_SI_ITEM>	CTfx2_STAT_SI_ITEM_Buffer;


