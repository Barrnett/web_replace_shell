//////////////////////////////////////////////////////////////////////////
//PrevAndNextWndMgr.cpp

#include "StdAfx.h"
#include "PrevAndNextWndMgr.h"

CPrevAndNextWndMgr::CPrevAndNextWndMgr(BOOL bLoop)
{
	m_bLoop = bLoop;
	m_cur_it = m_list_prevnext.end();
}

CPrevAndNextWndMgr::~CPrevAndNextWndMgr(void)
{
	close();
}

void CPrevAndNextWndMgr::close()
{
	m_list_prevnext.clear();

	m_list_timeorder.clear();

	m_cur_it = m_list_prevnext.end();

	return;
}

void CPrevAndNextWndMgr::InsertOneNodeInPrevAndNext(CWnd * data)
{
	if( m_cur_it == m_list_prevnext.end() )
	{
		m_list_prevnext.push_back(data);

		m_cur_it = m_list_prevnext.end();

		--m_cur_it;
	}
	else
	{
		std::list<CWnd *>::iterator it = m_cur_it;
		++it;

		m_list_prevnext.insert(it,data);

		++m_cur_it;
	}
}

void CPrevAndNextWndMgr::InsertOneNodeInTimeOrder(CWnd * data)
{
	m_list_timeorder.push_back(data);

	return;
}

BOOL CPrevAndNextWndMgr::ShowOneNodeInPrevAndNext(CWnd * data)
{
	std::list<CWnd *>::iterator it;

	for(it = m_list_prevnext.begin(); it != m_list_prevnext.end(); ++it)
	{
		if(*it != data)
			continue;

		if( it == m_cur_it )
		{
			return true;
		}
		else
		{
			m_list_prevnext.erase(it);

			InsertOneNodeInPrevAndNext(data);

			return true;
		}		
	}

	return false;
}

BOOL CPrevAndNextWndMgr::ShowOneNodeInTimeOrder(CWnd * data)
{
	DeleteOneNodeInTimeOrder(data);
	
	InsertOneNodeInTimeOrder(data);

	return true;
	
}

BOOL CPrevAndNextWndMgr::DeleteOneNodeInPrevAndNext(CWnd * data)
{
	std::list<CWnd *>::iterator it;

	for(it = m_list_prevnext.begin(); it != m_list_prevnext.end(); ++it)
	{
		if(*it != data)
			continue;

		if( it == m_cur_it )
		{
			if( m_cur_it == m_list_prevnext.begin() )
			{
				m_cur_it = m_list_prevnext.end();
			}
			else
			{
				--m_cur_it;
			}

			m_list_prevnext.erase(it);
		}
		else
		{
			m_list_prevnext.erase(it);
		}

		return true;
	}

	return false;
}

BOOL CPrevAndNextWndMgr::DeleteOneNodeInTimeOrder(CWnd * data)
{
	std::list<CWnd *>::iterator it;

	for(it = m_list_timeorder.begin(); it != m_list_timeorder.end(); ++it)
	{
		if(*it != data)
			continue;

		m_list_timeorder.erase(it);

		return true;
	}

	return false;
}

BOOL CPrevAndNextWndMgr::MoveToNextInPrevAndNext(CWnd *& data)
{
	if( m_cur_it == m_list_prevnext.end() )
		return false;

	std::list<CWnd *>::iterator last = m_list_prevnext.end();
	--last;

	if( m_cur_it != last )
	{
		++m_cur_it;
	}
	else
	{
		if( m_bLoop )
		{
			m_cur_it = m_list_prevnext.begin();
		}
	}

	data = *m_cur_it;

	return true;
}

BOOL CPrevAndNextWndMgr::MoveToPrevInPrevAndNext(CWnd *& data)
{
	if( m_cur_it == m_list_prevnext.end() )
		return false;

	if( m_cur_it != m_list_prevnext.begin() )
	{
		--m_cur_it;
	}
	else
	{
		if( m_bLoop )
		{
			m_cur_it = m_list_prevnext.end();
			--m_cur_it;
		}
	}

	data = *m_cur_it;

	return true;
}

BOOL CPrevAndNextWndMgr::MoveToNextInTimeOrder(CWnd * data)
{
	if( data )
	{
		ShowOneNodeInTimeOrder(data);
	}

	return true;
}

BOOL CPrevAndNextWndMgr::MoveToPrevInTimeOrder(CWnd * data)
{
	if( data )
	{
		ShowOneNodeInTimeOrder(data);
	}

	return true;
}

CWnd * CPrevAndNextWndMgr::GetOldest()
{
	if( m_list_timeorder.empty() )
		return NULL;

	return (*m_list_timeorder.begin());
}

CWnd * CPrevAndNextWndMgr::GetCur()
{
	if( m_list_prevnext.empty() )
		return NULL;

	return *m_cur_it;
}