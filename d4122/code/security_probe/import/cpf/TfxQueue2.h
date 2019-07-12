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

	// ������
	~TfxQueue2()
	{
		Destroy();
	}

	// ����һ��Ԫ��,���Ԫ�����ˣ�����-1
	//�����ȷ����0
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

	// ����һ��Ԫ��,���Ԫ�����ˣ���popһ����Ȼ��������
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

	// ����һ��Ԫ��
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

	// ����һ��Ԫ��
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

	//������ǰ���еģ����ǲ�ɾ��
	T * GetData(int index,BOOL& bValidData) const
	{
		if( index < 0 || index >= GetCurCount() )
			return NULL;

		bValidData = *m_status_queue.GetData(index);

		return m_data_queue.GetData(index);
	}

	//�õ���һ������
	T * GetHead(BOOL& bValidData) const
	{
		return GetData(0,bValidData);
	}

	//�õ����һ������
	T * GetTail(BOOL& bValidData) const
	{
		return GetData(GetCurCount()-1,bValidData);
	}

	// �����������
	int Clear()      
	{
		m_data_queue.Clear();
		m_status_queue.Clear();

		return 0;
	}

	// ��û�����󳤶�
	int GetMaxItem() const   
	{
		return m_data_queue.GetMaxItem();
	}

	// ���ʵ�����ݳ���
	int GetCurCount() const     
	{
		return m_data_queue.GetCurCount();
	}

	// ��ǰ�����Ƿ�Ϊ��
	BOOL IsEmpty() const    
	{
		return m_data_queue.IsEmpty();
	}

	// ��ǰ�����Ƿ�����
	BOOL IsFull() const   
	{
		return m_data_queue.IsFull();
	}

public:
	//�����������㣬�õ�ǰ��������ֵ����������ֵ.index��ʾ�ڼ�����ֵ����0��ʼ��
	//index=0��ʾa1-a0,index=1��ʾa2-a1.
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

	// ��ǰ�����Ƿ�Ϊ��
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


// ���涨��
typedef  TfxQueue2<BYTE>			CTfx2ByteBuffer;
typedef  TfxQueue2<WORD>			CTfx2WordBuffer;
typedef  TfxQueue2<DWORD>			CTfx2DwordBuffer;
typedef  TfxQueue2<long>			CTfx2LongBuffer;
typedef  TfxQueue2<int>			    CTfx2IntBuffer;
typedef  TfxQueue2<ACE_UINT64>		CTfx2Uint64Buffer;
typedef  TfxQueue2<ACE_UINT32>		CTfx2Uint32Buffer;
typedef  TfxQueue2<STAT_SI_ITEM>	CTfx2_STAT_SI_ITEM_Buffer;


