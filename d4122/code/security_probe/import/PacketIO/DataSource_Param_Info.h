//////////////////////////////////////////////////////////////////////////
//DataSource_Param_Info.h

#pragma once

#include "PacketIO/PacketIO.h"
#include "PacketIO/WinpCapDataSource.h"

typedef struct tagEtherio_RecvParam_Info
{
	tagEtherio_RecvParam_Info()
	{
		m_bEtherioEnableTimerPacket = TRUE;
		m_nMemSize = 20;
		m_nTimeStampType = 6;
		m_bErrorRecover = true;
		m_drv_load_type = 0;
	}

	tagEtherio_RecvParam_Info& operator = (const tagEtherio_RecvParam_Info& other )
	{
		if( &other != this )
		{
			m_strSysFileName = other.m_strSysFileName;
			m_strDriverName = other.m_strDriverName;
			m_strMacAddr = other.m_strMacAddr;
			m_bEtherioEnableTimerPacket = other.m_bEtherioEnableTimerPacket;
			m_nMemSize = other.m_nMemSize;
			m_nTimeStampType = other.m_nTimeStampType;
			m_bErrorRecover = other.m_bErrorRecover;
			m_drv_load_type = other.m_drv_load_type;
		}

		return *this;
	}

	std::string			m_strSysFileName;	
	std::string			m_strDriverName;
	std::string			m_strMacAddr;

	BOOL				m_bEtherioEnableTimerPacket;
	unsigned int		m_nMemSize;
	int					m_nTimeStampType;
	BOOL				m_bErrorRecover;
	int					m_drv_load_type;

}Etherio_RecvParam_Info;

typedef struct tagMem_RecvParam_Info
{
	tagMem_RecvParam_Info()
	{
		m_datatype = 0;

		m_minlen = m_maxlen = 64;
		m_wProto = 2;

		m_pModelData = NULL;
		m_nModelDataLen = 0;
	}

	~tagMem_RecvParam_Info()
	{
		if( m_pModelData )
		{
			delete []m_pModelData;
			m_pModelData = NULL;
		}
	}

	void clear()
	{
		if( m_pModelData )
		{
			delete []m_pModelData;
			m_pModelData = NULL;
		}

		m_nModelDataLen = 0;
	}

	tagMem_RecvParam_Info& operator = (const tagMem_RecvParam_Info& other )
	{
		if( &other != this )
		{
			m_datatype = other.m_datatype;

			if( m_pModelData )
			{
				delete []m_pModelData;
				m_pModelData = NULL;
			}

			m_nModelDataLen = other.m_nModelDataLen;

			if( other.m_pModelData )
			{
				m_pModelData = new unsigned char[m_nModelDataLen];

				memcpy(m_pModelData,other.m_pModelData,m_nModelDataLen);
			}

			m_minlen = other.m_minlen;
			m_maxlen = other.m_maxlen;
			m_wProto = other.m_wProto;
		}



		return *this;
	}

public:

	unsigned int		m_minlen;
	unsigned int		m_maxlen;
	WORD				m_wProto;

	
	//m_datatype=0���������
	//m_datatype=1, �༭����
	int					m_datatype;

	unsigned char *		m_pModelData;
	int					m_nModelDataLen;


}Mem_RecvParam_Info;

typedef struct tagMem_EditData_Info
{
	tagMem_EditData_Info()
	{
		vlan_id = -1;
		trans_type = 2;

		memset(src_mac,0x00,sizeof(src_mac));
		memset(dst_mac,0x00,sizeof(dst_mac));

		hostorder_src_ip = 0;
		hostorder_dst_ip = 0;

		tos = 0;
		ttl = 255;

		hostorder_src_port = 80;
		hostorder_dst_port = 80;
	}

	tagMem_EditData_Info& operator=(const tagMem_EditData_Info& other)
	{
		if( this != &other )
		{
			memcpy(this,&other,sizeof(*this));
		}

		return *this;
	}

	BYTE src_mac[6];
	BYTE dst_mac[6];

	int	 vlan_id;//���Ϊ-1��ʾ��Ч����ʹ��vlan

	ACE_UINT32 hostorder_src_ip;
	ACE_UINT32 hostorder_dst_ip;

	int	tos;
	int ttl;

	int	trans_type;//0=tcp,1=udp,2=icmp

	ACE_UINT16 hostorder_src_port;
	ACE_UINT16 hostorder_dst_port;

}Mem_EditData_Info;

typedef struct tagPlayback_Param_Info
{
	tagPlayback_Param_Info()
	{
		m_nloopnums = 1;
		m_skip_error_file = false;

		m_re_calculate_time = 1;
		m_nAdjustType = 0;
	}

	tagPlayback_Param_Info& operator = (const tagPlayback_Param_Info& other )
	{
		if( &other != this )
		{
			m_nloopnums = other.m_nloopnums;

			m_vtFileName = other.m_vtFileName;
			m_skip_error_file = other.m_skip_error_file;

			m_re_calculate_time = other.m_re_calculate_time;
			m_nAdjustType = other.m_nAdjustType;
			m_strAdjustBeginTime = other.m_strAdjustBeginTime;
		}

		return *this;
	}

	int GetAdjustBeginTime(CTimeStamp32& adjusttime) const
	{		
		if( m_strAdjustBeginTime.empty() )
		{
			adjusttime = CTimeStamp32::zero;
		}
		else
		{//��ʱ��֧�֣�ʹ�õ�ǰʱ��������
			adjusttime = CTimeStamp32::zero;
		}

		return m_nAdjustType;
	}

	size_t					m_nloopnums;//loopnumsѭ���Ĵ���,0��ʾ����ѭ����������ʾѭ������
	CStdStringArray			m_vtFileName;

	int						m_skip_error_file;//�Ƿ��������ִ�����ļ�


	//re_calculate_time=1��ʾ����ļ���ʱ���������������֤�ڶ����ļ�����ʱ��϶���ǰһ���ļ���
	//re_calculate_time=0��ʾʹ��ÿ���ļ��������ʱ��,�����Ϳ��ܳ��ֺ������ʱ��С��ǰ�����ʱ�䡣
	//���m_nAdjustType==1����������Ч����ȱʡΪ1
	int						m_re_calculate_time;

	//;����ʱ�䣺Ϊ0��ʾ��������
	//  Ϊ1��ʾʹ��ϵͳ��ǰʱ��������������ÿ������ʱ��
	//	;Ϊ2��ʾʹ�þ���ʱ��m_strAdjustBeginTime��������Ŀǰ��֧�֣�
	//	;���ļ������ݵ�ʱ���µ�ÿ������ʱ�� = m_strAdjustBeginTime+(����ʱ��-��һ������ʱ�䣩
	//	;ȱʡΪ0
	int						m_nAdjustType;


	//	;adjust_begin_timeʱ���ʽ������������: 2007:12:31-23:32:59
	//;adjust_begin_timeΪ�գ����ʾʹ�õ�ǰʱ����ƫ��,ȱʡΪ��
	//	;tm_hour: (0�C23)
	//	;tm_mday: (1�C31) 
	//	;tm_min: (0�C59)
	//	;tm_mon: (1�C12; January = 1)
	//	;tm_sec:(0�C59)
	//	;tm_year:2001�ĸ�ʽ 

	std::string				m_strAdjustBeginTime;

}Playback_Param_Info;

struct Ethereal_Playback_Param
{
public:
	Ethereal_Playback_Param& operator = (const Ethereal_Playback_Param& other )
	{
		if( &other != this )
		{
			m_playback_info = other.m_playback_info;
		}

		return *this;
	}

public:
	Playback_Param_Info	m_playback_info;

};

struct Cpt_Playback_Param
{
public:
	Cpt_Playback_Param()
	{
		m_nReadNums = 0;
		m_nBufferSizeMB = 8;
	}

public:
	Cpt_Playback_Param& operator = (const Cpt_Playback_Param& other )
	{
		if( &other != this )
		{
			m_nReadNums = other.m_nReadNums;
			m_nBufferSizeMB = other.m_nBufferSizeMB;
			m_playback_info = other.m_playback_info;
		}

		return *this;
	}


public:
	unsigned int	m_nReadNums;//���������ļ���˵�������ļ������Ϊ0��ʾ�������ļ�
	unsigned int	m_nBufferSizeMB;

	Playback_Param_Info	m_playback_info;
};

struct ShareMem_DataSource_Param
{
	ShareMem_DataSource_Param()
	{
		m_mem_size_mb = 0;
	}

	ShareMem_DataSource_Param& operator = (const ShareMem_DataSource_Param& other )
	{
		if( &other != this )
		{
			m_strShareName = other.m_strShareName;
			m_mem_size_mb = other.m_mem_size_mb;
		}

		return *this;
	}

public:
	std::string		m_strShareName;
	unsigned int	m_mem_size_mb;
};

class PACKETIO_CLASS CDataSource_Param_Info
{
public:
	CDataSource_Param_Info()
	{
		m_drvType = DRIVER_TYPE_NONE;

		m_nThreadBufferSize = 0;
		m_bEnableTimerPacket = true;

		m_nbps = (ACE_UINT64)-1;
		m_unit_nums = 100;

		m_bCopyPkt = 0;
		m_nDiscartRate = 0;
	}

	~CDataSource_Param_Info(void)
	{

	}

public:

	DRIVER_TYPE GetDrvType() const
	{
		return m_drvType;
	}

	void SetDrvType(DRIVER_TYPE drvtype)
	{
		m_drvType = drvtype;
	}

	void SetSpeedCtrl(ACE_UINT64 nbps,unsigned int unit_nums)
	{	m_nbps = nbps;	m_unit_nums = unit_nums;}

	void GetSpeedCtrl(ACE_UINT64& nbps,unsigned int& unit_nums) const
	{	nbps = m_nbps;	unit_nums = m_unit_nums;}

	BOOL IsEnableTimerPacket() const
	{
		return m_bEnableTimerPacket;
	}

	void SetEnableTimerPacket(BOOL bEnableTimerPacket)
	{
		m_bEnableTimerPacket = bEnableTimerPacket;
	}

	unsigned int GetThreadBufferSize() const
	{
		return m_nThreadBufferSize;
	}

	void SetThreadBufferSize(unsigned int nThreadBufferSize)
	{
		m_nThreadBufferSize = nThreadBufferSize;
	}

	unsigned int GetDiscardRate() const
	{
		return m_nDiscartRate;
	}

	void SetDiscardRate(unsigned int discard_rete)
	{
		m_nDiscartRate = discard_rete;
	}

	BOOL IsCopyPacket() const
	{
		return m_bCopyPkt;
	}

	void SetCopyPacket(BOOL bCopyPkt)
	{
		m_bCopyPkt = bCopyPkt;
	}

	Cpt_Playback_Param & GetCptPlaybackParam()
	{
		return m_cpt_playback_param;
	}

	Playback_Param_Info & GetCommonPlaybackParam();

	Ethereal_Playback_Param& GetEtheralPlaybackParam()
	{
		return m_ethreal_playback_param;
	}


#ifdef OS_WINDOWS
	WINPCAP_RecvDRV_PARAM& GetWinpcapDrvParam()
	{
		return m_winpcap_drv_param;
	}

	Etherio_RecvParam_Info& GetEtherioParamInfo()
	{
		return m_etherioParamInfo;

	}
#endif

	Mem_RecvParam_Info& GetMemDatasourceParam()
	{
		return m_memParamInfo;
	}

	Mem_EditData_Info& GetMemEditDataInfo()
	{
		return m_memEditDataInfo;
	}

	ShareMem_DataSource_Param& GetShareMemDatasourceParam()
	{
		return m_ShareMemParamInfo;
	}

public:
	static void ConstrouctPacket(Mem_RecvParam_Info& mem_info,
		Mem_EditData_Info& mem_editdata_info);

private:
	DRIVER_TYPE				m_drvType;
	unsigned int			m_nThreadBufferSize;//ʹ���̵߳Ĺ����ڴ�,��СMB
	BOOL					m_bEnableTimerPacket;

	BOOL					m_bCopyPkt;//�Ƿ�Ϊ�����ݰ�ȫ����������
	unsigned int			m_nDiscartRate;//����������ı���������m_nDiscartRate=20����ʾ�������1/20�����ݰ�

	Cpt_Playback_Param		m_cpt_playback_param;

	Ethereal_Playback_Param	m_ethreal_playback_param;

#ifdef OS_WINDOWS
	WINPCAP_RecvDRV_PARAM	m_winpcap_drv_param;
#endif

	//for etherio
	Etherio_RecvParam_Info		m_etherioParamInfo;

	//for mem_datasource
	Mem_RecvParam_Info			m_memParamInfo;

	Mem_EditData_Info			m_memEditDataInfo;

	ShareMem_DataSource_Param	m_ShareMemParamInfo;


	//�������ݵ��ٶȿ���
	//m_npbs=-1��ʾ�������ٶȡ�0��ʾ�������κ�����
	ACE_UINT64				m_nbps;
	//uint(1-10^6):���Ƶ����ȣ�����һ���ӵ����ݷֳɼ���������,��ֵԽ�����ݷ���Խƽ��,ȱʡ100
	unsigned int			m_unit_nums;

	friend class CDataSourceParamReader;
	friend class C_Ini_DataSourceParamReader;
	friend class C_Xml_DataSourceParamReader;
	friend class CDataSourceMgr;
};
