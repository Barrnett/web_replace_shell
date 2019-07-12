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

	
	//m_datatype=0，随机数据
	//m_datatype=1, 编辑数据
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

	int	 vlan_id;//如果为-1表示无效，不使用vlan

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
		{//暂时不支持，使用当前时间来调整
			adjusttime = CTimeStamp32::zero;
		}

		return m_nAdjustType;
	}

	size_t					m_nloopnums;//loopnums循环的次数,0表示无限循环，其他表示循环次数
	CStdStringArray			m_vtFileName;

	int						m_skip_error_file;//是否跳过出现错误的文件


	//re_calculate_time=1表示多个文件的时间进行排序。这样保证第二个文件包的时间肯定比前一个文件大。
	//re_calculate_time=0表示使用每个文件包里面的时间,这样就可能出现后面包的时间小于前面包的时间。
	//如果m_nAdjustType==1，这个标记无效。，缺省为1
	int						m_re_calculate_time;

	//;调整时间：为0表示不调整，
	//  为1表示使用系统当前时间来调整来代替每个包的时间
	//	;为2表示使用具体时间m_strAdjustBeginTime来调整（目前不支持）
	//	;读文件的数据的时候，新的每个包的时间 = m_strAdjustBeginTime+(包的时间-第一个包的时间）
	//	;缺省为0
	int						m_nAdjustType;


	//	;adjust_begin_time时间格式必须是完整的: 2007:12:31-23:32:59
	//;adjust_begin_time为空，则表示使用当前时间来偏移,缺省为空
	//	;tm_hour: (0–23)
	//	;tm_mday: (1–31) 
	//	;tm_min: (0–59)
	//	;tm_mon: (1–12; January = 1)
	//	;tm_sec:(0–59)
	//	;tm_year:2001的格式 

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
	unsigned int	m_nReadNums;//对于连续文件来说读几个文件，如果为0表示读所有文件
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
	unsigned int			m_nThreadBufferSize;//使用线程的共享内存,大小MB
	BOOL					m_bEnableTimerPacket;

	BOOL					m_bCopyPkt;//是否为了数据安全，拷贝数据
	unsigned int			m_nDiscartRate;//随机丢弃包的比例，比如m_nDiscartRate=20，表示随机丢弃1/20的数据包

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


	//接收数据的速度控制
	//m_npbs=-1表示不控制速度。0表示不发送任何数据
	ACE_UINT64				m_nbps;
	//uint(1-10^6):控制的粒度，即将一秒钟的数据分成几份来发送,数值越大，数据发送越平缓,缺省100
	unsigned int			m_unit_nums;

	friend class CDataSourceParamReader;
	friend class C_Ini_DataSourceParamReader;
	friend class C_Xml_DataSourceParamReader;
	friend class CDataSourceMgr;
};
