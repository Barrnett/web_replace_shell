//////////////////////////////////////////////////////////////////////////
//XMLFileSock.h

#pragma once

#include "cpf/ostypedef.h"
#include "cpf/ComImp.h"

namespace CPF
{
	class CPF_CLASS CTempFileNameHelper
	{
	public:
		CTempFileNameHelper();

	public:
		//在写文件之前，得到临时文件名
		const char * GetTempFileName(const char * orig_filename);

		//在写入成功后，将临时文件修改成正式文件名
		BOOL RestoreOrigName(const char * orig_filename);

	private:
		char m_temp_file_name[MAX_PATH];

	};

	//服务器接到命令后的处理
	CPF_CLASS BOOL OnCmd_SetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	
	//安全的写文件，程序接到xml文件的命令，先写入一个临时文件，如果临时文件写成功，再覆盖到正式文件filename
	CPF_CLASS BOOL OnCmd_Safe_SetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

	CPF_CLASS BOOL OnCmd_GetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

	//客户端在发送命令完成后，得到的数据，准备写文件
	CPF_CLASS BOOL GetOneXMLFileFromServer(const char * filename,MSG_BLOCK * pMsgBlock);

	//安全的得到文件，得到文件后，，先写入一个临时文件，如果临时文件写成功，再覆盖到正式文件filename
	CPF_CLASS BOOL Safe_GetOneXMLFileFromServer(const char * filename,MSG_BLOCK * pMsgBlock);

}
