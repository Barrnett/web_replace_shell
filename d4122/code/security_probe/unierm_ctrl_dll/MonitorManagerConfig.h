//////////////////////////////////////////////////////////////////////////
//MonitorManagerConfig.h

#ifndef _MONITOR_MANAGER_CONFIG_H
#define _MONITOR_MANAGER_CONFIG_H

#include <vector>
#include "MonitorManager.h"
#include "cpf/Common_Func_Macor.h"
#include "cpf/TinyXmlEx.h"

typedef struct tagPROBESERVICE_CONFIG_PARAM 
{
	tagPROBESERVICE_CONFIG_PARAM()
	{
//		nSupportRestoreModule = 0x00;
		bLogState = false;
//		bTestCom = false;
	}

	tagPROBESERVICE_CONFIG_PARAM(const tagPROBESERVICE_CONFIG_PARAM & other)
	{
		*this = other ;
	}
	tagPROBESERVICE_CONFIG_PARAM& operator = (const tagPROBESERVICE_CONFIG_PARAM & other)
	{
		if (this != &other)
		{
			//nSupportRestoreModule = other.nSupportRestoreModule;
	
			bLogState = other.bLogState;
			//bTestCom = other.bTestCom;

			//probe_param = other.probe_param;
			//db_param = other.db_param;	
			//disk_param = other.disk_param;
			
		}

		return *this;
	}

	//enum{RESTORE_PROBE = 0X01,RESTORE_DB = 0X02,RESTORE_MGRPLAT = 0X04};

	//ACE_UINT32	nSupportRestoreModule;

	BOOL		bLogState;

	//是否进行通信测试，在进行通信测试的时候，
	//所有的命令需要很长时间的命令都没有实际执行，比如恢复到出厂配置，清理统计数据等
	//BOOL		bTestCom;

	typedef struct tag_test_com_param
	{
		int wait_time;

		tag_test_com_param()
		{
			wait_time = 0;
		}

		tag_test_com_param& operator = (const tag_test_com_param & other)
		{
			if (this != &other)
			{
				wait_time = other.wait_time;
			}

			return *this;
		}

	}TEST_COM_PARAM;

	typedef struct tag_probe_param
	{
		tag_probe_param()
		{
			szProbeConfigDeliver[0] = 0 ;
			szProbeConfigReceive[0] = 0 ;

			szProbe_output_path[0] = 0;
			szProbe_data_path[0] = 0;
			szProbe_buffer_path[0] = 0;

			szProbeConfigOrgBak[0] = 0;
		}

		tag_probe_param& operator = (const tag_probe_param & other)
		{
			if (this != &other)
			{
				ACE_OS::strcpy(szProbeConfigDeliver,other.szProbeConfigDeliver);
				ACE_OS::strcpy(szProbeConfigReceive,other.szProbeConfigReceive);

				ACE_OS::strcpy(szProbe_output_path,other.szProbe_output_path);
				ACE_OS::strcpy(szProbe_data_path,other.szProbe_data_path);
				ACE_OS::strcpy(szProbe_buffer_path,other.szProbe_buffer_path);

				ACE_OS::strcpy(szProbeConfigOrgBak,other.szProbeConfigOrgBak);

				vtCtrlProgramName = other.vtCtrlProgramName;
			}

			return *this;
		}
	
		std::vector<std::string>	vtCtrlProgramName;

		char szProbe_output_path[MAX_PATH];
		char szProbe_data_path[MAX_PATH];
		char szProbe_buffer_path[MAX_PATH];

		char szProbeConfigDeliver[MAX_PATH];	//配置下发的位置，需要填写全路径名称，缺省位置在程序运行目录下的.\config
		char szProbeConfigReceive[MAX_PATH];	//将配置文件从<ConfigDeliver>位置移动到该节点所在的位置，缺省位置在程序运行目录下的.\config

		char szProbeConfigOrgBak[MAX_PATH];		//出厂的配置备份目录，当接到要求恢复出厂信息的时候，将这个目录拷贝覆盖到szConfigReceive下

	}PROBE_PARAM;

	typedef struct tag_db_param
	{
		tag_db_param()
		{
			szDBProgramPath[0] = 0;
			szDBDataPath[0] = 0;

			szDBName[0] = 0;

			szUserName[0] = 0;
			szPasswd[0] = 0;
			char_set[0] = 0;

			key_buffer_size = 64;
			event_scheduler = true;
			table_cache = 256;
			sort_buffer_size = 4;
			read_buffer_size = 1;
			port = 3306;

			wait_connect_time = 0;
		}

		tag_db_param& operator = (const tag_db_param & other)
		{
			if (this != &other)
			{
				ACE_OS::strcpy(szDBProgramPath,other.szDBProgramPath);
				ACE_OS::strcpy(szDBDataPath,other.szDBDataPath);

				ACE_OS::strcpy(szDBName,other.szDBName);

				ACE_OS::strcpy(szUserName,other.szUserName);
				ACE_OS::strcpy(szPasswd,other.szPasswd);
				ACE_OS::strcpy(char_set,other.char_set);

				key_buffer_size = other.key_buffer_size;
				table_cache = other.table_cache;
				sort_buffer_size = other.sort_buffer_size;
				read_buffer_size = other.read_buffer_size;
				port = other.port;

				wait_connect_time = other.wait_connect_time;
			}

			return *this;
		}

		//--default-character-set=GBK 
		//--key_buffer_size=64M 
		//--table_cache=256 
		//--sort_buffer_size=4M 
		//--read_buffer_size=1M 
		//--event_scheduler=on 
		//--port=3306 
		//--datadir=E:/DBDATA/

		//当nSupportRestoreModule&RESTORE_DB是有效
		char szDBProgramPath[MAX_PATH];//db的程序路径
		char szDBDataPath[MAX_PATH];//db的数据路径

		char szDBName[64];

		char szUserName[256];
		char szPasswd[128];
		char char_set[64];	
		BOOL event_scheduler;
		int key_buffer_size;
		int table_cache;
		int	 sort_buffer_size;
		int	read_buffer_size;
		int port;

		int	wait_connect_time;

	}DB_PARAM;

	typedef struct  tag_DISK_ALARM_PARAM
	{
		tag_DISK_ALARM_PARAM()
		{
			for(size_t i = 0; i < GET_NUMS_OF_ARRAY(aUsePercentLimit_level); ++i)
			{
				aUsePercentLimit_level[i] = -1;
			}

			aUsePercentLimit_level[0] = 65;
			aUsePercentLimit_level[1] = 70;
	
			aUsePercentLimit_level[2] = 75;
			aUsePercentLimit_level[3] = 80;

			aUsePercentLimit_level[4] = 85;
			aUsePercentLimit_level[5] = 90;
		}

		tag_DISK_ALARM_PARAM& operator = (const tag_DISK_ALARM_PARAM & other)
		{
			if (this != &other)
			{
				memcpy(aUsePercentLimit_level,
					other.aUsePercentLimit_level,
					sizeof(aUsePercentLimit_level));
			}

			return *this;
		}

		int aUsePercentLimit_level[5*2];			
		
	}DISK_ALARM_PARAM;

public:
	//PROBE_PARAM		probe_param;
	//DB_PARAM		db_param;
	//TEST_COM_PARAM	test_com_param;

	//DISK_ALARM_PARAM	disk_param;


}PROBESERVICE_CONFIG_PARAM;

class CMonitorManagerConfig
{
public:
	CMonitorManagerConfig();
	~CMonitorManagerConfig();

	int open(const char * xml_filename);
	void close();

	void save();

public:
	int GetMonitorTasks(std::vector<MONITOR_TASK> & aMonitorTask);

	int ReadProbeServiceConfigParam(PROBESERVICE_CONFIG_PARAM& config_param);

public:
	int SetMonitorTaskState(const char * taskname,int state);

private:
	CTinyXmlEx	m_xml_tool;
};



#endif
