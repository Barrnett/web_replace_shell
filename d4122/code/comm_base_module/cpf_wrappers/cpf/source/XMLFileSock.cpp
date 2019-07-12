//////////////////////////////////////////////////////////////////////////
//XMLFileSock.cpp

#include "cpf/XMLFileSock.h"
#include "cpf/TinyXmlEx.h"
#include "cpf/Data_Stream.h"
#include "cpf/path_tools.h"

CPF::CTempFileNameHelper::CTempFileNameHelper()
{
	m_temp_file_name[0] = 0;
}

//在写文件之前，得到临时文件名
const char * CPF::CTempFileNameHelper::GetTempFileName(const char * orig_filename)
{
	sprintf(m_temp_file_name,"%s.%u.temp.urm",orig_filename,GetTickCount());

	return m_temp_file_name;
}
//
BOOL CPF::CTempFileNameHelper::RestoreOrigName(const char * orig_filename)
{
	if( !CopyFile(m_temp_file_name,orig_filename,false) )
	{
		return false;
	}

	ACE_OS::unlink(m_temp_file_name);

	return true;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL CPF::OnCmd_SetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	const char * pdata = CMsgBlockHelper_Stream::ReadPtr(*pMsgBlock,0,0);

	ACE_UINT32 length = CData_Stream_LE::GetUint32(pdata);
	const char * ptext = NULL;

	if( length > 0 )
	{
		ptext = CData_Stream_LE::GetFixString(pdata,length);
	}

	FILE * file = fopen(filename,"wb");

	if( file )
	{
		if(length > 0)
		{
			if( 1 != fwrite(ptext,length-1,1,file) )
			{
				fclose(file);
				return false;
			}
		}

		fclose(file);

		if(length > 0)
		{
			CTinyXmlEx xml_reader;

			if( xml_reader.open_for_readonly(filename) != 0 )
			{
				return false;
			}

			xml_reader.close();
		}
		
		return true;
	}

	return false;
}

BOOL CPF::OnCmd_Safe_SetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	CPF::CTempFileNameHelper tmp_file_name_helper;

	const char * buf_file_name = tmp_file_name_helper.GetTempFileName(filename);

	if( !OnCmd_SetOneXmlFileConfig(buf_file_name,pRemoteAddr,pMsgBlock,pRetMsgBlock) )
	{
		return false;
	}

	return tmp_file_name_helper.RestoreOrigName(filename);
}


BOOL CPF::OnCmd_GetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock)
{
	char * buf = NULL;
	long length = CTinyXmlEx::ReadOneXmlFileForSend(filename,buf);

	if( length == -1 )
	{
		pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,2+4);

		char * pdata = CMsgBlockHelper_Stream::WritePtr(*pRetMsgBlock,0,2+4);

		CData_Stream_LE::PutUint16(pdata,0);
		CData_Stream_LE::PutUint32(pdata,0);

		return true;
	}

	pRetMsgBlock = CMsgBlockManager::Malloc_Rsp(pMsgBlock,2+length);

	if( !pRetMsgBlock )
	{
		delete []buf;

		pRetMsgBlock = CBaseDoCmdTool::MakeUint16ErrorMsgBlock(pMsgBlock,4,RESPONSE_CODE_SUCCESS);
		return true;
	}

	char * pdata = CMsgBlockHelper_Stream::WritePtr(*pRetMsgBlock,0,2+length);

	CData_Stream_LE::PutUint16(pdata,0);
	CData_Stream_LE::PutFixString(pdata,length,buf);

	delete []buf;

	return true;
}

BOOL CPF::GetOneXMLFileFromServer(const char * filename,MSG_BLOCK * pMsgBlock)
{
	const char * pdata = CMsgBlockHelper_Stream::ReadPtr(*pMsgBlock,0,0);

	ACE_UINT16 code = CData_Stream_LE::GetUint16(pdata);

	if( code != 0 )
		return false;

	ACE_UINT32 length = CData_Stream_LE::GetUint32(pdata);
	const char * ptext = NULL;

	if( length > 0 )
	{
		ptext = CData_Stream_LE::GetFixString(pdata,length);
	}

	// 先创建一个目录，在进行文件操作
	std::string path_name = CPF::GetPathNameFromFullName(filename);
	if (! CPF::CreateFullDirecory(path_name.c_str()) )
		return FALSE;

	FILE * file = fopen(filename,"wb");

	if( file )
	{
		if(length > 0)
		{
			if( 1 != fwrite(ptext,length-1,1,file) )
				return false;
		}

		fclose(file);

		return true;
	}

	return false;
}

BOOL CPF::Safe_GetOneXMLFileFromServer(const char * filename,MSG_BLOCK * pMsgBlock)
{
	CPF::CTempFileNameHelper tmp_file_name_helper;

	const char * buf_file_name = tmp_file_name_helper.GetTempFileName(filename);

	if( !GetOneXMLFileFromServer(buf_file_name,pMsgBlock) )
	{
		return false;
	}
	
	return tmp_file_name_helper.RestoreOrigName(filename);
}
