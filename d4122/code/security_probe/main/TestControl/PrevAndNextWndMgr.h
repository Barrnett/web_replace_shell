//////////////////////////////////////////////////////////////////////////
//PrevAndNextWndMgr.h

#pragma once

#include <list>

//(1)插入一个新的节点，往当前节点后面插入。新插入的节点为当前节点
//(2)点击了列表中的一个节点，将点击节点删除，然后按照（1）的方法插入
//(3)来回查看节点，只变动当前节点
//(4)删除当前节点，则当前节点设置成上一个节点
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
	std::list<CWnd *>			m_list_timeorder;//按照时间的先后进行排序

};
