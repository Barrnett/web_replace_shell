//////////////////////////////////////////////////////////////////////////
//RecvDataSource.cpp

#include "PacketIO/RecvDataSource.h"
#include "cpf/other_tools.h"

BOOL IRecvDataSource::IsPlaybackDataSource() const
{
	DRIVER_TYPE drv_type = GetDrvType(); 

	switch( drv_type )
	{
	case DRIVER_TYPE_MULTI_FILE:
	case DRIVER_TYPE_FILE_ETHEREAL:
	case DRIVER_TYPE_FILE_ZCCPT:
		return true;

	default:
		return false;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
//IExtDataSource
//////////////////////////////////////////////////////////////////////////

int IExtDataSource::OpenDataSource()
{
	if( m_pRealDataSource )
	{
		if( m_pRealDataSource->OpenDataSource() == 0 )
		{
			SeekToBegin();
		}
	}

	return -1;
}

void IExtDataSource::CloseDataSource()
{
	if( m_pRealDataSource )
	{
		m_pRealDataSource->CloseDataSource();
	}

}

//开始接收数据，注意不是打开设备，可能有些子类没有具体实现。
BOOL IExtDataSource::StartToRecv()
{
	if( m_pRealDataSource )
	{
		return m_pRealDataSource->StartToRecv();
	}

	return false;
}

//停止接收数据，注意不是关闭设备，可能有些子类没有具体实现。
void IExtDataSource::StopRecving()
{
	if( m_pRealDataSource )
	{
		return m_pRealDataSource->StopRecving();
	}

	return;
}

void IExtDataSource::GetDroppedPkts(ACE_UINT64& pkts,ACE_UINT64& bytes)
{
	if( m_pRealDataSource )
	{
		m_pRealDataSource->GetDroppedPkts(pkts,bytes);
	}
	else
	{
		pkts = -1;
		bytes = -1;
	}

	return;
}

void IExtDataSource::GetTotalPkts(ACE_UINT64& pkts,ACE_UINT64& bytes,unsigned int& nloops)
{
	if( m_pRealDataSource )
	{
		m_pRealDataSource->GetTotalPkts(pkts,bytes,nloops);
	}
	else
	{
		nloops = 1;
		pkts = -1;
		bytes = -1;
	}

	return;
}

//将读数据位置指向开始的地方。
BOOL IExtDataSource::SeekToBegin()
{
	return m_pRealDataSource->SeekToBegin();
}




//////////////////////////////////////////////////////////////////////////
//ISingleFileRecvDataSource
//////////////////////////////////////////////////////////////////////////
BOOL ISingleFileRecvDataSource::init(const char * filename,BOOL bcur_systime)
{
	m_strFileName = filename;
	m_bcur_systime = bcur_systime;
	return true;
}

void ISingleFileRecvDataSource::fini()
{
}

bool ISingleFileRecvDataSource::GetDataSourceName(std::string& name)
{
	std::string otherinfo = GetFileOtherInfo(m_strFileName.c_str(),NULL);

	name = m_strFileName.c_str() + otherinfo;

	return true;
}

std::string ISingleFileRecvDataSource::GetFileOtherInfo(const char * fullname,
														const Time_Stamp * pcur_time)
{
	std::string name;

#ifdef OS_WINDOWS

		WIN32_FILE_ATTRIBUTE_DATA file_attr;

		BOOL bOK = GetFileAttributesEx(fullname,GetFileExInfoStandard,&file_attr);

		if( bOK )
		{
			ACE_UINT64 filesize = (((ACE_UINT64)file_attr.nFileSizeHigh<<32) | file_attr.nFileSizeLow);

			char buf[1024];

			if( !pcur_time || pcur_time->sec == 0 )
			{
				CTimeStamp32 ts;

				ts.m_ts.from_ace_timevalue(ACE_Time_Value(file_attr.ftCreationTime));

				sprintf(buf,"(%uMB,%s)",
					(unsigned int)(filesize>>20), CPF::FormatTime(6,ts.GetSEC()) );
			}
			else
			{
				sprintf(buf,"(%uMB,%s)",
					(unsigned int)(filesize>>20), CPF::FormatTime(6,pcur_time->sec) );

			}

			name = buf;
		}
#else
		//do nothing now
		//...

#endif




	return name;

}



