//////////////////////////////////////////////////////////////////////////
//PktSpeedCtrl.h

#include "ace/Time_Value.h"
#include "cpf/cpf.h"

#pragma once

class CPF_CLASS CPktSpeedCtrl
{
public:
	CPktSpeedCtrl(void)
	{
		m_speed = -1;
		m_uint_nums = 100;
		m_nRealSendBytes = 0;
	}

	virtual ~CPktSpeedCtrl(void)
	{

	}

	//speed：发包的速度，单位为bps.,如果speed==0，表示不能发送任何数据包
	//speed==-1表示不要控制速度
	//uint_nums(1-10^6):控制的粒度，即将一秒钟的数据分成几份来发送,数值越大，数据发送越平缓
	virtual BOOL init(ACE_UINT64 speed,unsigned int uint_nums=100)
	{
		m_speed = speed;
		m_uint_nums = uint_nums;

		m_nRealSendBytes = 0;

		return true;
	}

	//看这个数据是否能够发送，如果不能发送返回FALSE，并且要求sleep(sleeptime)
	//如果能够发送，返回TRUE。sleeptime无效
	inline BOOL IfCanSendPkt(size_t pktlen,OUT ACE_Time_Value* sleeptime)
	{
		BOOL bSend = SubClass_IfCanSendPkt(pktlen,sleeptime);

		if( bSend )
		{
			m_nRealSendBytes += (unsigned int)pktlen;
		}

		return bSend;
	}

	//重新开始计算流量，一般每秒钟调用一次本函数
	//返回这个时间段发送了多少个字节
	inline ACE_UINT64 Reset()
	{
		SubClass_Reset();

		ACE_UINT64 nRealSendBytes = m_nRealSendBytes;

		m_nRealSendBytes = 0;

		return nRealSendBytes;
	}

	//一直等待可以发送数据包。这里面调用了IfCanSendPkt。
	inline void WaitTimeToNextPkt(size_t pktlen)
	{
		ACE_Time_Value waittime;

		while( 1 )
		{
			if( IfCanSendPkt(pktlen,&waittime) )
			{
				return;
			}
			else
			{
				ACE_OS::sleep(waittime);
			}
		}	

		return;
	}

public:
	inline ACE_UINT64 GetSpeed() const
	{
		return m_speed;
	}

	inline unsigned int GetUnitNums() const
	{
		return m_uint_nums;
	}

protected:
	virtual void SubClass_Reset() = 0;
	virtual BOOL SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime) = 0;

protected:
	ACE_UINT64 m_speed;
	unsigned int m_uint_nums;

	//实际在单位时间内发送多少个自己,子类在实际发送出去后
	unsigned int m_nRealSendBytes;

};

class CPF_CLASS C_lrt_PktSpeedCtrl : public CPktSpeedCtrl
{
public:
	C_lrt_PktSpeedCtrl(void);
	virtual ~C_lrt_PktSpeedCtrl(void);

	//speed：发包的速度，单位为bps.,如果speed==0，表示不能发送任何数据包
	//speed==-1表示不要控制速度
	//uint_nums(1-10^6):控制的粒度，即将一秒钟的数据分成几份来发送,数值越大，数据发送越平缓
	virtual BOOL init(ACE_UINT64 speed,unsigned int uint_nums=100);

protected:
	//看这个数据是否能够发送，如果不能发送返回FALSE，并且要求sleep(sleeptime)
	//如果能够发送，返回TRUE。sleeptime无效
	virtual BOOL SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime);

	virtual void SubClass_Reset();

private:	
	ACE_UINT64		m_FirstTime;//当前时间单位发出第一包的时间

	ACE_UINT64		m_nBytesConfig;//根据配置的速度和时间控制粒度，每个时间段里面应该发送多少个字节
	ACE_UINT64		m_nBytesSended;//这个时间段内，已经发送了多少个字节

	//比如分成一秒钟分成1000份，则m_BlockTimeMs=1000*1000/1000=1000
	//比如分成一秒钟分成100份，则m_BlockTimeMs=1000*1000/100=10*100
	//m_BlockTimeUs=1000*1000/uint_nums;
	unsigned int	m_BlockTimeUs;//每秒粒度的个数

	BOOL			m_bFull;//当前时间段的数据是否发送完毕
};

#ifdef OS_WINDOWS

class CPF_CLASS C_winhrt_PktSpeedCtrl : public CPktSpeedCtrl
{
public:
	C_winhrt_PktSpeedCtrl(void);
	virtual ~C_winhrt_PktSpeedCtrl(void);

public:
	//speed：发包的速度，单位为bps.,如果speed==0，表示不能发送任何数据包
	//speed==-1表示不要控制速度
	//uint_nums(1-10^6):控制的粒度，即将一秒钟的数据分成几份来发送,数值越大，数据发送越平缓
	virtual BOOL init(ACE_UINT64 speed,unsigned int uint_nums=100);

protected:
	//看这个数据是否能够发送，如果不能发送返回FALSE，并且要求sleep(sleeptime)
	//如果能够发送，返回TRUE。sleeptime无效
	virtual BOOL SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime);

	virtual void SubClass_Reset();

private:
	double			m_Frequency_Per_us;

	LARGE_INTEGER	m_Frequency;
	LARGE_INTEGER	m_FirstCounter;//当前时间单位发出第一包的时间
	LARGE_INTEGER	m_BlockCounter;//粒度


	BOOL			m_bFull;//当前时间段的数据是否发送完毕
	ACE_UINT64		m_nBytesConfig;//根据配置的速度和时间控制粒度，每个时间段里面应该发送多少个字节
	ACE_UINT64		m_nBytesSended;//这个时间段内，已经发送了多少个字节

};

#endif//OS_WINDOWS

//////////////////////////////////////////////////////////////////////////

//type:0尽量创建高精度控制,如果不能创建高精度，创建一个低精度
//type:1低精度控制
//type:2高精度控制，如果不能创建，返回NULL
CPF_CLASS CPktSpeedCtrl * CreatePktSpeedCtrl(int type,ACE_UINT64 speed,unsigned unit_nums);


