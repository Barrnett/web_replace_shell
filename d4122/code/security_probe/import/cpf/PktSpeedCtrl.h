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

	//speed���������ٶȣ���λΪbps.,���speed==0����ʾ���ܷ����κ����ݰ�
	//speed==-1��ʾ��Ҫ�����ٶ�
	//uint_nums(1-10^6):���Ƶ����ȣ�����һ���ӵ����ݷֳɼ���������,��ֵԽ�����ݷ���Խƽ��
	virtual BOOL init(ACE_UINT64 speed,unsigned int uint_nums=100)
	{
		m_speed = speed;
		m_uint_nums = uint_nums;

		m_nRealSendBytes = 0;

		return true;
	}

	//����������Ƿ��ܹ����ͣ�������ܷ��ͷ���FALSE������Ҫ��sleep(sleeptime)
	//����ܹ����ͣ�����TRUE��sleeptime��Ч
	inline BOOL IfCanSendPkt(size_t pktlen,OUT ACE_Time_Value* sleeptime)
	{
		BOOL bSend = SubClass_IfCanSendPkt(pktlen,sleeptime);

		if( bSend )
		{
			m_nRealSendBytes += (unsigned int)pktlen;
		}

		return bSend;
	}

	//���¿�ʼ����������һ��ÿ���ӵ���һ�α�����
	//�������ʱ��η����˶��ٸ��ֽ�
	inline ACE_UINT64 Reset()
	{
		SubClass_Reset();

		ACE_UINT64 nRealSendBytes = m_nRealSendBytes;

		m_nRealSendBytes = 0;

		return nRealSendBytes;
	}

	//һֱ�ȴ����Է������ݰ��������������IfCanSendPkt��
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

	//ʵ���ڵ�λʱ���ڷ��Ͷ��ٸ��Լ�,������ʵ�ʷ��ͳ�ȥ��
	unsigned int m_nRealSendBytes;

};

class CPF_CLASS C_lrt_PktSpeedCtrl : public CPktSpeedCtrl
{
public:
	C_lrt_PktSpeedCtrl(void);
	virtual ~C_lrt_PktSpeedCtrl(void);

	//speed���������ٶȣ���λΪbps.,���speed==0����ʾ���ܷ����κ����ݰ�
	//speed==-1��ʾ��Ҫ�����ٶ�
	//uint_nums(1-10^6):���Ƶ����ȣ�����һ���ӵ����ݷֳɼ���������,��ֵԽ�����ݷ���Խƽ��
	virtual BOOL init(ACE_UINT64 speed,unsigned int uint_nums=100);

protected:
	//����������Ƿ��ܹ����ͣ�������ܷ��ͷ���FALSE������Ҫ��sleep(sleeptime)
	//����ܹ����ͣ�����TRUE��sleeptime��Ч
	virtual BOOL SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime);

	virtual void SubClass_Reset();

private:	
	ACE_UINT64		m_FirstTime;//��ǰʱ�䵥λ������һ����ʱ��

	ACE_UINT64		m_nBytesConfig;//�������õ��ٶȺ�ʱ��������ȣ�ÿ��ʱ�������Ӧ�÷��Ͷ��ٸ��ֽ�
	ACE_UINT64		m_nBytesSended;//���ʱ����ڣ��Ѿ������˶��ٸ��ֽ�

	//����ֳ�һ���ӷֳ�1000�ݣ���m_BlockTimeMs=1000*1000/1000=1000
	//����ֳ�һ���ӷֳ�100�ݣ���m_BlockTimeMs=1000*1000/100=10*100
	//m_BlockTimeUs=1000*1000/uint_nums;
	unsigned int	m_BlockTimeUs;//ÿ�����ȵĸ���

	BOOL			m_bFull;//��ǰʱ��ε������Ƿ������
};

#ifdef OS_WINDOWS

class CPF_CLASS C_winhrt_PktSpeedCtrl : public CPktSpeedCtrl
{
public:
	C_winhrt_PktSpeedCtrl(void);
	virtual ~C_winhrt_PktSpeedCtrl(void);

public:
	//speed���������ٶȣ���λΪbps.,���speed==0����ʾ���ܷ����κ����ݰ�
	//speed==-1��ʾ��Ҫ�����ٶ�
	//uint_nums(1-10^6):���Ƶ����ȣ�����һ���ӵ����ݷֳɼ���������,��ֵԽ�����ݷ���Խƽ��
	virtual BOOL init(ACE_UINT64 speed,unsigned int uint_nums=100);

protected:
	//����������Ƿ��ܹ����ͣ�������ܷ��ͷ���FALSE������Ҫ��sleep(sleeptime)
	//����ܹ����ͣ�����TRUE��sleeptime��Ч
	virtual BOOL SubClass_IfCanSendPkt(size_t pktlen,ACE_Time_Value* sleeptime);

	virtual void SubClass_Reset();

private:
	double			m_Frequency_Per_us;

	LARGE_INTEGER	m_Frequency;
	LARGE_INTEGER	m_FirstCounter;//��ǰʱ�䵥λ������һ����ʱ��
	LARGE_INTEGER	m_BlockCounter;//����


	BOOL			m_bFull;//��ǰʱ��ε������Ƿ������
	ACE_UINT64		m_nBytesConfig;//�������õ��ٶȺ�ʱ��������ȣ�ÿ��ʱ�������Ӧ�÷��Ͷ��ٸ��ֽ�
	ACE_UINT64		m_nBytesSended;//���ʱ����ڣ��Ѿ������˶��ٸ��ֽ�

};

#endif//OS_WINDOWS

//////////////////////////////////////////////////////////////////////////

//type:0���������߾��ȿ���,������ܴ����߾��ȣ�����һ���;���
//type:1�;��ȿ���
//type:2�߾��ȿ��ƣ�������ܴ���������NULL
CPF_CLASS CPktSpeedCtrl * CreatePktSpeedCtrl(int type,ACE_UINT64 speed,unsigned unit_nums);


