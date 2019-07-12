// 20101123
// 修改为在linux下运行，不在具有dialog的状态，只是一个普通类，不使用OnTimer，完全有外部调用
// 将所有在界面呈现的东西，修改成printf输出和日志输出

// ChannelFluxDlg.cpp : implementation file
//

//#include "stdafx.h"
//#include "TestControl.h"
#include "ChannelFluxDlg.h"
#include "BandPolicyData2.h"

#include "flux_control.h"
#include "WorktimeSetting.h"
// CChannelFluxDlg dialog

//IMPLEMENT_DYNAMIC(CChannelFluxDlg, CPropertyPage)

CChannelFluxDlg::CChannelFluxDlg(ACE_UINT32 vm_id)
	: m_nTimerInterval(2)
	, m_vm_id(vm_id)

CChannelFluxDlg::CChannelFluxDlg()
	: m_nTimerInterval(2)
{
	m_pOldStat = NULL;
}

CChannelFluxDlg::~CChannelFluxDlg()
{
	if( m_pOldStat )
	{
		delete []m_pOldStat;
		m_pOldStat = NULL;
	}
}


BOOL CChannelFluxDlg::OnInitDialog()
{
//	InitListCtrl();

	OnBnClickedButtonRefresh();

	m_dwLastTick = 0;

//	m_nTimer = SetTimer(1,m_nTimerInterval*1000,NULL);

	return TRUE;
}

/*
void CChannelFluxDlg::InitListCtrl()
{
	m_listChannel.SetExtendedStyle(
		(m_listChannel.GetExtendedStyle() 
		| LVS_EX_FULLROWSELECT 
		| LVS_EX_GRIDLINES));

	int index = 0;

	{
		m_listChannel.InsertColumn(index++,"通道名称",LVCFMT_LEFT,110);
		m_listChannel.InsertColumn(index++,"通道ID",LVCFMT_LEFT,60);
		m_listChannel.InsertColumn(index++,"上行带宽(最低/最高)",LVCFMT_RIGHT,150);
		m_listChannel.InsertColumn(index++,"下行带宽(最低/最高)",LVCFMT_RIGHT,150);

		m_listChannel.InsertColumn(index++,"总流量-流控前/流控后",LVCFMT_RIGHT,120);
		m_listChannel.InsertColumn(index++,"流出-流控前/流控后",LVCFMT_RIGHT,120);
		m_listChannel.InsertColumn(index++,"流入-流控前/流控后",LVCFMT_RIGHT,120);
	}


	return;
}
*/

void CChannelFluxDlg::OnTimer(UINT_PTR nIDEvent)
{
	DWORD curTick = GetTickCount();

	for(int j = 0; j < (int)m_vt_channel_id.size(); ++j)
	{
		const unsigned int channel_id = m_vt_channel_id[j].first;

		CHANNEL_PERF_STAT_EX stat;

		CHANNEL_PERF_STAT_EX& old_stat = m_pOldStat[j];

		if( !FLUX_CONTROL_GetChannelFlux(channel_id,stat) )
		{
			stat = old_stat;
		}

		CString strInfo;

		if( m_dwLastTick != 0 )
		{
printf("通道名称=%-30s 通道ID=%d\t", m_vt_channel_id[j].second.c_str(), j);
			int col = 4;

			std::string flux_info1;

			//流控前-总流量
			CPF::GetFluxInfo(stat.nOrigSendBytes+stat.nOrigRecvBytes,
				old_stat.nOrigSendBytes+old_stat.nOrigRecvBytes,
				curTick - m_dwLastTick,
				1,
				&flux_info1);

			std::string flux_info2;

			//流控后-总流量
			CPF::GetFluxInfo(stat.nSendBytes+stat.nRecvBytes,
				old_stat.nSendBytes+old_stat.nRecvBytes,
				curTick - m_dwLastTick,
				1,
				&flux_info2);

			strInfo = flux_info1.c_str();

			strInfo += "/";

			strInfo += flux_info2.c_str();

//			m_listChannel.SetItemText(j,col++,strInfo.GetString());
printf("\%s", strInfo.GetString());

			//流控前-发
			CPF::GetFluxInfo(stat.nOrigSendBytes,
				old_stat.nOrigSendBytes,
				curTick - m_dwLastTick,
				1,
				&flux_info1);

			//流控后-发
			CPF::GetFluxInfo(stat.nSendBytes,
				old_stat.nSendBytes,
				curTick - m_dwLastTick,
				1,
				&flux_info2);

			strInfo = flux_info1.c_str();

			strInfo += "/";

			strInfo += flux_info2.c_str();

//			m_listChannel.SetItemText(j,col++,strInfo.GetString());
printf("\t%s", strInfo.GetString());

			//流控前-收
			CPF::GetFluxInfo(stat.nOrigRecvBytes,
				old_stat.nOrigRecvBytes,
				curTick - m_dwLastTick,
				1,
				&flux_info1);


			//流控后-收
			CPF::GetFluxInfo(stat.nRecvBytes,
				old_stat.nRecvBytes,
				curTick - m_dwLastTick,
				1,
				&flux_info2);

			strInfo = flux_info1.c_str();

			strInfo += "/";

			strInfo += flux_info2.c_str();

//			m_listChannel.SetItemText(j,col++,strInfo.GetString());
printf("\t%s\n", strInfo.GetString());
		}

		old_stat = stat;	
	}

	printf("\n");
	m_dwLastTick = curTick;

//	CPropertyPage::OnTimer(nIDEvent);
}


static CString ShowBandAndRate(unsigned int band,unsigned int total_band)
{
	CString str_info;

	if( total_band != 0 )
	{
		str_info.Format("%d%%",band*100/total_band);

		std::string str_band = CPF::GetFluxInfo(band,1);

		str_info += "(";
		str_info += str_band.c_str();
		str_info += ")";

	}
	else
	{
		str_info = "0";
	}

	return str_info;

}

void CChannelFluxDlg::OnBnClickedButtonRefresh()
{
	CollectChannelInfo();

//	m_listChannel.DeleteAllItems();
	printf("\n");

	for(size_t i = 0; i < m_vt_channel_id.size(); ++i)
	{
		const unsigned int channle_id = m_vt_channel_id[i].first;
		const char * name = m_vt_channel_id[i].second.c_str();

//		int item = m_listChannel.InsertItem(m_listChannel.GetItemCount(),name);
		printf("通道名称=%-30s", name);

		char id_buf[20] = {0};

		sprintf(id_buf,"%u",channle_id);

//		m_listChannel.SetItemText(item,1,id_buf);
		printf("\t通道ID=%s", id_buf);

		const CBucketChannel * pChannel = CBandPolicyData2_Singleton::instance()->m_internet_policy.FindChannel(channle_id);

		if( pChannel )
		{
			unsigned int min_band_up,max_band_up;
			unsigned int min_band_down,max_band_down;

			GetChannelBandInfo(pChannel,min_band_up,max_band_up,min_band_down,max_band_down);

			CString str_min_band_up = ShowBandAndRate(min_band_up,
				CBandPolicyData2_Singleton::instance()->m_internet_policy.GetTotalBand(DIR_I2O));

			CString str_max_band_up = ShowBandAndRate(max_band_up,
				CBandPolicyData2_Singleton::instance()->m_internet_policy.GetTotalBand(DIR_I2O));

			CString strInfo_up = str_min_band_up;

			strInfo_up += "/";

			strInfo_up += str_max_band_up;

//			m_listChannel.SetItemText(item,2,strInfo_up.GetString());
			printf("\t上行带宽(最低/最高)=%s", strInfo_up.GetString());

			CString str_min_band_down = ShowBandAndRate(min_band_down,
				CBandPolicyData2_Singleton::instance()->m_internet_policy.GetTotalBand(DIR_O2I));

			CString str_max_band_down = ShowBandAndRate(max_band_down,
				CBandPolicyData2_Singleton::instance()->m_internet_policy.GetTotalBand(DIR_O2I));

			CString strInfo_down = str_min_band_down;

			strInfo_down += "/";

			strInfo_down += str_max_band_down;

//			m_listChannel.SetItemText(item,3,strInfo_down.GetString());
			printf("\t下行带宽(最低/最高)=%s\n", strInfo_down.GetString());
		}
	}

}

void CChannelFluxDlg::GetChannelBandInfo(const CBucketChannel * pChannel,
						unsigned int& min_band_up,unsigned int& max_band_up,
						unsigned int& min_band_down,unsigned int& max_band_down)
{
	const unsigned int channel_id = pChannel->m_channel_id;

	if( channel_id ==  CHANNEL_ID_ALL )
	{
		min_band_up = max_band_up  = CBandPolicyData2_Singleton::instance()->m_internet_policy.GetTotalBand(DIR_I2O);
		min_band_down = max_band_down = CBandPolicyData2_Singleton::instance()->m_internet_policy.GetTotalBand(DIR_O2I);
	}
	else if( channel_id ==  CHANNEL_ID_GUARD )
	{
		CChannelCollectionData *pChannelCollectionData = NULL;

		pChannelCollectionData = 
			&CBandPolicyData2_Singleton::instance()->m_internet_policy.m_guart_policy;

		min_band_up = pChannelCollectionData->GetMinUsedBand(DIR_I2O);
		min_band_down = pChannelCollectionData->GetMinUsedBand(DIR_O2I);

		max_band_up = pChannelCollectionData->GetMaxUsedBand(DIR_I2O);
		max_band_down = pChannelCollectionData->GetMaxUsedBand(DIR_O2I);
	}
	else if( channel_id ==  CHANNEL_ID_LIMITED
		|| channel_id ==  CHANNEL_ID_OTHER )
	{
		min_band_up = 0;
		min_band_down = 0;

		ACE_Time_Value cur_time = ACE_OS::gettimeofday();

		int time_type = CWorktimeSetting_Singleton::instance()->get_time_type((int)cur_time.sec());

		CTimeStamp32 curts(cur_time);

		max_band_up = CBandPolicyData2_Singleton::instance()->m_internet_policy.GetMinRemainBand(DIR_I2O,time_type,curts);
		max_band_down = CBandPolicyData2_Singleton::instance()->m_internet_policy.GetMinRemainBand(DIR_O2I,time_type,curts);
	}	
	else
	{
		min_band_up =  pChannel->GetMinBand(DIR_I2O);
		min_band_down = pChannel->GetMinBand(DIR_O2I);

		max_band_up = pChannel->GetMaxBand(DIR_I2O);
		max_band_down = pChannel->GetMaxBand(DIR_O2I);
	}

	return;
}

void CChannelFluxDlg::CollectChannelInfo()
{
	m_vt_channel_id.clear();

	if( m_pOldStat )
	{
		delete []m_pOldStat;
		m_pOldStat = NULL;
	}

	unsigned int channel_id = -1;
	std::string channel_name;
	
	channel_id = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_all_channel.m_channel_id;
	channel_name = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_all_channel.m_str_channel_name;
	m_vt_channel_id.push_back(std::make_pair(channel_id,channel_name));

	channel_id = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_all_guart_channle.m_channel_id;
	channel_name = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_all_guart_channle.m_str_channel_name;
	m_vt_channel_id.push_back(std::make_pair(channel_id,channel_name));

	channel_id = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_remain_channel.m_channel_id;
	channel_name = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_remain_channel.m_str_channel_name;
	m_vt_channel_id.push_back(std::make_pair(channel_id,channel_name));

	channel_id = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_other_channel.m_channel_id;
	channel_name = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_other_channel.m_str_channel_name;
	m_vt_channel_id.push_back(std::make_pair(channel_id,channel_name));


	{
		int count = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_guart_policy.GetChannelCount();

		for(int i = 0; i < count; ++i)
		{
			CBucketChannel * pChannel =
				CBandPolicyData2_Singleton::instance()->m_internet_policy.m_guart_policy.GetChannel(i);

			m_vt_channel_id.push_back(std::make_pair(pChannel->m_channel_id,pChannel->m_str_channel_name));
		}
	}

	{
		int count = CBandPolicyData2_Singleton::instance()->m_internet_policy.m_limited_policy.GetChannelCount();

		for(int i = 0; i < count; ++i)
		{
			CBucketChannel * pChannel =
				CBandPolicyData2_Singleton::instance()->m_internet_policy.m_limited_policy.GetChannel(i);

			m_vt_channel_id.push_back(std::make_pair(pChannel->m_channel_id,pChannel->m_str_channel_name));
		}
	}

	m_pOldStat = new CHANNEL_PERF_STAT_EX[m_vt_channel_id.size()];

	return;
}
