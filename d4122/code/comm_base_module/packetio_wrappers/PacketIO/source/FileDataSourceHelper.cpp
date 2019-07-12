//////////////////////////////////////////////////////////////////////////
//FileDataSourceHelper.cpp

#include "PacketIO/FileDataSourceHelper.h"
#include "PacketIO/EtherrealDataSource.h"
#include "PacketIO/DataSourceHelper.h"

CFileDataSourceHelper::CFileDataSourceHelper(void)
{
	m_pFileRecvDataSource = NULL;
}

CFileDataSourceHelper::~CFileDataSourceHelper(void)
{
}



BOOL CFileDataSourceHelper::init(const char * filename,DRIVER_TYPE type)
{	
	ACE_ASSERT(DRIVER_TYPE_FILE_ETHEREAL==type||DRIVER_TYPE_FILE_ZCCPT==type);

	m_pFileRecvDataSource = (IFileRecvDataSource *)
		CDataSourceHelper::CreateRecvDataSource(type,false,false);

	if( !m_pFileRecvDataSource )
		return false;

	m_pFileRecvDataSource->init(filename);

	return true;
}

void CFileDataSourceHelper::fini()
{
	m_pFileRecvDataSource->fini();

	CDataSourceHelper::DestroyRecvDataSource(m_pFileRecvDataSource);
	m_pFileRecvDataSource = NULL;

}

int CFileDataSourceHelper::OpenDataSource()
{
	return m_pFileRecvDataSource->OpenDataSource();
}

void CFileDataSourceHelper::CloseDataSource()
{
	if( m_pFileRecvDataSource )
		return m_pFileRecvDataSource->CloseDataSource();
}