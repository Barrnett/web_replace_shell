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
		//��д�ļ�֮ǰ���õ���ʱ�ļ���
		const char * GetTempFileName(const char * orig_filename);

		//��д��ɹ��󣬽���ʱ�ļ��޸ĳ���ʽ�ļ���
		BOOL RestoreOrigName(const char * orig_filename);

	private:
		char m_temp_file_name[MAX_PATH];

	};

	//�������ӵ������Ĵ���
	CPF_CLASS BOOL OnCmd_SetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);
	
	//��ȫ��д�ļ�������ӵ�xml�ļ��������д��һ����ʱ�ļ��������ʱ�ļ�д�ɹ����ٸ��ǵ���ʽ�ļ�filename
	CPF_CLASS BOOL OnCmd_Safe_SetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

	CPF_CLASS BOOL OnCmd_GetOneXmlFileConfig(const char * filename,ACE_INET_Addr * pRemoteAddr,MSG_BLOCK * pMsgBlock,MSG_BLOCK * & pRetMsgBlock);

	//�ͻ����ڷ���������ɺ󣬵õ������ݣ�׼��д�ļ�
	CPF_CLASS BOOL GetOneXMLFileFromServer(const char * filename,MSG_BLOCK * pMsgBlock);

	//��ȫ�ĵõ��ļ����õ��ļ��󣬣���д��һ����ʱ�ļ��������ʱ�ļ�д�ɹ����ٸ��ǵ���ʽ�ļ�filename
	CPF_CLASS BOOL Safe_GetOneXMLFileFromServer(const char * filename,MSG_BLOCK * pMsgBlock);

}
