//////////////////////////////////////////////////////////////////////////
//PrevAndNextWndMgr.h

#pragma once

#include <list>

//(1)����һ���µĽڵ㣬����ǰ�ڵ������롣�²���Ľڵ�Ϊ��ǰ�ڵ�
//(2)������б��е�һ���ڵ㣬������ڵ�ɾ����Ȼ���գ�1���ķ�������
//(3)���ز鿴�ڵ㣬ֻ�䶯��ǰ�ڵ�
//(4)ɾ����ǰ�ڵ㣬��ǰ�ڵ����ó���һ���ڵ�
class CPrevAndNextWndMgr
{
public:
	CPrevAndNextWndMgr(BOOL bLoop);

	~CPrevAndNextWndMgr(void);

	void close();

public:
	void InsertOneNode(CWnd * data)
	{
		InsertOneNodeInPrevAndNext(data);
		InsertOneNodeInTimeOrder(data);
	}

	BOOL ShowOneNode(CWnd * data)
	{
		ShowOneNodeInPrevAndNext(data);
		ShowOneNodeInTimeOrder(data);

		return true;
	}

	BOOL DeleteOneNode(CWnd * data)
	{
		DeleteOneNodeInPrevAndNext(data);
		DeleteOneNodeInTimeOrder(data);

		return true;
	}

	BOOL MoveToNext(CWnd *& data)
	{
		MoveToNextInPrevAndNext(data);

		if( data )
		{
			MoveToNextInTimeOrder(data);

			return true;
		}

		return false;

	}

	BOOL MoveToPrev(CWnd *& data)
	{
		MoveToPrevInPrevAndNext(data);

		if( data )
		{
			MoveToPrevInTimeOrder(data);

			return true;
		}

		return false;

	}

public:
	CWnd * GetOldest();
	CWnd * GetCur();

private:
	BOOL DeleteOneNodeInPrevAndNext(CWnd * data);
	BOOL DeleteOneNodeInTimeOrder(CWnd * data);

	void InsertOneNodeInPrevAndNext(CWnd * data);
	void InsertOneNodeInTimeOrder(CWnd * data);

	BOOL ShowOneNodeInPrevAndNext(CWnd * data);
	BOOL ShowOneNodeInTimeOrder(CWnd * data);

	BOOL MoveToNextInPrevAndNext(CWnd *& data);
	BOOL MoveToPrevInPrevAndNext(CWnd *& data);
	
	BOOL MoveToNextInTimeOrder(CWnd * data);
	BOOL MoveToPrevInTimeOrder(CWnd * data);

private:
	std::list<CWnd *>::iterator	m_cur_it;
	std::list<CWnd *>			m_list_prevnext;

	BOOL						m_bLoop;

	
private:
	std::list<CWnd *>			m_list_timeorder;//����ʱ����Ⱥ��������

};
