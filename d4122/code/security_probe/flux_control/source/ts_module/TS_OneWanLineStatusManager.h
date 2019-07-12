#if !defined(__TS_ONEWANLINE_STATUSMANAGER_HEADER__)
#define __TS_ONEWANLINE_STATUSMANAGER_HEADER__
//////////////////////////////////////////////////////////////////////////
//TS_OneWanLineStatusManager.h

#include "cpf/ostypedef.h"

class COneTypeTestInfo
{
public:
	COneTypeTestInfo()
	{
		onoff = 0;

		test_items = NULL;
		cur_index = 0;
		max_nums = 0;

		m_drop_nums = 0;
		m_delay_ms = 0;

		m_rsp_num = 0;
		m_lost_num = 0;
	}

	~COneTypeTestInfo()
	{
		close();
	}

	void init(int onoff_param,int nums)
	{
		close();

		onoff = onoff_param;

		if( onoff )
		{
			test_items = new TAGOneTestItem[nums];
		}

		cur_index = 0;
		max_nums = nums;

		m_drop_nums = 0;
		m_delay_ms = 0;
		m_rsp_num = 0;
		m_lost_num = 0;
	}

	void close()
	{
		if( test_items )
		{
			delete []test_items;
			test_items = NULL;
		}

		cur_index = 0;
		max_nums = 0;
		onoff = 0;

		m_drop_nums = 0;
		m_delay_ms = 0;
		m_rsp_num = 0;
		m_lost_num = 0;
	}

public:
	typedef struct TAGOneTestItem
	{
	public:
		TAGOneTestItem()
		{
			id = 0;
			send_time = rsp_time = 0;
		}

		inline ACE_UINT32 CalDelay() const
		{
			return ((ACE_UINT32)(rsp_time-send_time));
		}

		ACE_UINT64	id;

		ACE_UINT32 send_time;//�������ݰ���ʱ��,send_time=0��ʾ��û�з���
		ACE_UINT32 rsp_time;//������ʱ�䣬rsp_time=-1��ʾ�Ѿ���ʱ�ˣ�rsp_time=0��ʾ�շ��ͣ���û�л�����������ʾ������ʱ��
	}OneTestItem;

public:
	void OnSendOneRequest(ACE_UINT64 id,ACE_UINT32 send_time)
	{
		if( onoff )
		{
			if( cur_index >= max_nums )
			{
				cur_index = 0;
			}

			//�����ǵĶ����Ƕ���״̬�򶪰�����1���հ�״̬���հ�����1
			if ((ACE_UINT32)-1 == test_items[cur_index].rsp_time)
			{
				if (m_lost_num > 0)
				{
					m_lost_num--;
				}
			}
			else if (0 != test_items[cur_index].rsp_time)
			{
				if (m_rsp_num > 0)
				{
					m_rsp_num--;
				}
			}

			test_items[cur_index].id = id;
			test_items[cur_index].send_time = send_time;
			test_items[cur_index].rsp_time = 0;


			++cur_index;
		}
	}

	void OnGetOneRsp(ACE_UINT64 id,ACE_UINT32 rsp_time)
	{
		if( onoff )
		{
			for(int i = 0; i < max_nums; ++i)
			{
				if ((test_items[i].id == id)&&(test_items[i].rsp_time == 0))
				{
					test_items[i].rsp_time = rsp_time;

					if (m_rsp_num < max_nums)
					{
						m_rsp_num++;
					}

					break;
				}
			}
		}		

		return;
	}

	//���㶪����
	inline int CalDropNums()
	{
		m_drop_nums = 0;

		if( onoff )
		{
			for(int i = 0; i < max_nums; ++i)
			{
				if( test_items[i].send_time != 0 
					&& test_items[i].rsp_time == (ACE_UINT32)-1 )
				{
					++m_drop_nums;
				}
			}
		}

		return m_drop_nums;
	}

	//����ƽ��ʱ��
	inline int CalDelay()
	{
		m_delay_ms = 0;

		int ok_nums = 0;
		ACE_UINT32 delay_ms = 0;

		if( onoff )
		{
			for(int i = 0; i < max_nums; ++i)
			{
				if( test_items[i].send_time != 0 
					&& test_items[i].rsp_time != (ACE_UINT32)-1 
					&& test_items[i].rsp_time != (ACE_UINT32)0 )
				{
					delay_ms += test_items[i].CalDelay();

					++ok_nums;
				}
			}
		}

		if( ok_nums == 0 )
		{
			return 0;
		}

		m_delay_ms = (int)(delay_ms/ok_nums);

		return m_delay_ms;
	}

	inline int CheckTimeout(ACE_UINT32 cur_time,int timeout_ms)
	{		
		if( onoff )
		{
			for(int i = 0; i < max_nums; ++i)
			{
				if( test_items[i].send_time != 0 
					&& test_items[i].rsp_time == 0 )
				{
					if( (ACE_UINT32)(cur_time-test_items[i].send_time) >= (ACE_UINT32)timeout_ms )
					{
						test_items[i].rsp_time = (ACE_UINT32)-1;

						m_lost_num++;
					}
				}
			}
		}
		
		return m_lost_num;
	}

	// add xumx 2016-7-12, ͳ��״̬�Ѿ���Ч�ĸ���
	inline unsigned int GetValidNums(ACE_UINT32 cur_time, unsigned int timeout_ms)
	{
		if( !onoff ){ return(0); }

		unsigned int ValidNums = 0;
		for(int i = 0; i < max_nums; ++i)
		{
			if
			(
				(test_items[i].send_time != 0 )
			 &&  (cur_time - test_items[i].send_time >= timeout_ms)
			)
			{
				ValidNums++;
			}
		}

		return(ValidNums);
	}

public:
	inline int GetDropNums() const
	{
		return m_drop_nums;
	}

	inline int GetDelayMs() const
	{
		return m_delay_ms;
	}

	inline int GetRspNum() const
	{
		return m_rsp_num;
	}

public:
	int					onoff;
	TAGOneTestItem *	test_items;
	int					cur_index;
	int					max_nums;

private:
	int					m_drop_nums;
	int					m_delay_ms;
	int					m_rsp_num;
	int					m_lost_num;
};

#if 0
class COneWanLineStatusManager
{
public:
	COneWanLineStatusManager(void);
	~COneWanLineStatusManager(void);

	enum{WAN_LINE_STATUS_OK = 0};

	enum{INDEX_PING=0,INDEX_DNS=1,INDEX_TCPING=2,MAX_TEST_TYPE=3};

public:
	void init(
		int active_test_try_nums,
		int active_test_interval,		
		int drop_pkt_timeout_msec,
		int offline_judge_pkts,
		int	ping_test_onoff,
		int	dns_test_onoff,
		int	tcping_test_onoff);

	void close();

public:
	void OnSendOneRequest(int test_type,ACE_UINT64 id,ACE_UINT32 send_time);
	void OnGetOneRsp(int test_type,ACE_UINT64 id,ACE_UINT32 rsp_time);

	void CheckTimeout(ACE_UINT32 cur_time);

	inline BOOL IsActive() const
	{
		return m_is_active;
	}

private:
	//���㶪����
	int CalDropNums(int test_type)
	{
		if( test_type < 0 || test_type >= MAX_TEST_TYPE )
		{
			ACE_ASSERT(FALSE);
			return 0;
		}

		return m_test_info[test_type].CalDropNums();
	}

	//����ƽ��ʱ��
	int CalDelay(int test_type)
	{
		if( test_type < 0 || test_type >= MAX_TEST_TYPE )
		{
			ACE_ASSERT(FALSE);
			return 0;
		}

		return m_test_info[test_type].CalDelay();
	}

	BOOL CalIsOK() const;

public:
	void CalWanLineStatus(
		BOOL& bis_active,
		int& ping_err_nums,
		int& dns_err_nums,
		int& tcping_err_nums,
		int& ping_delay_ms,
		int& dns_delay_ms,
		int& tcping_delay_ms);

	BOOL IsOK() const;

public:
	COneTypeTestInfo			m_test_info[MAX_TEST_TYPE];

private:
	int			m_active_test_interval;//̽����,ÿ��̽�����ʱ������ÿ���೤ʱ�䷢��һ�����ݰ����룩  
	int			m_active_test_try_nums;//���Դ���,�����ٸ�����һ�μ�����ڷ��Ͷ��ٸ����ݰ�
	int			m_drop_pkt_timeout_msec;//������ʱ�������жϣ���ʱʱ��೤ʱ��û��Ӧ����Ϊû����Ӧ
	int			m_offline_judge_pkts;//	���˶��ٸ�����Ϊ����

private:
	BOOL		m_is_active;
};
#endif

//////////////////////////////////////////////////////////////////////////
#endif
