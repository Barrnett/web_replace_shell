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

	// ������
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

		if (buff == NULL ) // ���ڲ�����ռ�
		{
			m_bFreeMem = TRUE;

			this->m_buff = new T[m_max_len];

			if (this->m_buff == NULL) return -1;
		}
		else // ����ռ����ⲿ����, ��ʱ��������ʱ������ɾ������
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

	// ����һ��Ԫ��,���Ԫ�����ˣ�����-1
	//�����ȷ����0
	int Push(const T & a_data)    
	{
		if ( this->IsFull() ) return -1;

		m_buff[m_nTail] = a_data;
		m_nTail = ( m_nTail + 1) % m_max_len;

		m_buff_len ++;

		return 0;
	}

	// ����һ��Ԫ��,���Ԫ�����ˣ���popһ����Ȼ��������
	void Push_force(const T & a_data)
	{
		if ( this->IsFull() )
		{
			Pop();
		}

		Push(a_data);
	}

	// ����һ��Ԫ��
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

	// ����һ��Ԫ��
	int PopBuff(T *  p_data, int len) 
	{
		int li_poped_len = mymin(len,GetCurCount());

		for ( int i = 0 ; i < li_poped_len ; i ++)
		{
			Pop(*(p_data + li_poped_len) );
		}

		return li_poped_len;
	}

	//������ǰ���еģ����ǲ�ɾ��
	T * GetData(int index) const
	{
		if( index < 0 || index >= GetCurCount() )
			return NULL;

		return &m_buff[(m_nFront + index)%m_max_len];
	}

	//�õ���һ������
	T * GetHead() const
	{
		return GetData(0);
	}

	//�õ����һ������
	T * GetTail() const
	{
		return GetData(GetCurCount()-1);
	}

	// �����������
	int Clear()      
	{
		this->m_buff_len = 0;
		this->m_nFront = 0;
		this->m_nTail = 0;

		return 0;
	}

	// ��û�����󳤶�
	int GetMaxItem() const   
	{
		return (this->m_max_len);
	}

	// ���ʵ�����ݳ���
	int GetCurCount() const     
	{
		return this->m_buff_len;
	}

	// ��ǰ�����Ƿ�Ϊ��
	BOOL IsEmpty() const    
	{
		return (this->m_buff_len == 0 );
	}

	// ��ǰ�����Ƿ�����
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
	//�����������㣬�õ�ǰ��������ֵ����������ֵ.index��ʾ�ڼ�����ֵ����0��ʼ��
	//index=0��ʾa1-a0,index=1��ʾa2-a1.
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

	// ��ǰ�����Ƿ�Ϊ��
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

	int  m_nFront;    // ����
	int  m_nTail;    // ��β��
	int  m_buff_len;    // ���泤��
	int  m_max_len;    // �����

	BOOL    m_bFreeMem;    // �Ƿ����ڲ��Ƿ��ڴ�


};


#include "cpf/flux_tools.h"

// ���涨��
typedef  CTfxQueue<BYTE>			CTfxByteBuffer;
typedef  CTfxQueue<WORD>			CTfxWordBuffer;
typedef  CTfxQueue<DWORD>			CTfxDwordBuffer;
typedef  CTfxQueue<long>			CTfxLongBuffer;
typedef  CTfxQueue<int>				CTfxIntBuffer;
typedef  CTfxQueue<ACE_UINT64>		CTfxUint64Buffer;
typedef  CTfxQueue<ACE_UINT32>		CTfxUint32Buffer;
typedef  CTfxQueue<STAT_SI_ITEM>	CTfx_STAT_SI_ITEM_Buffer;

//��������T�����֧��T1-T2����ʹ�������������ȡ��ֵ������ʹ�ø�����һЩ
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

