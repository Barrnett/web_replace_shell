//////////////////////////////////////////////////////////////////////////
//HostEtherCardMgr.h һ����������̫�����Ĺ���

#pragma once

#include "cpf/cpf.h"

class CPF_CLASS CHostEtherCardMgr
{
public:
	typedef struct tagONE_ETHER_CARD_INFO
	{
		int		_iPktMaxLen;				// ���͵�ʱ�����İ���

		//������������linux����һ���ģ�����etho,eth1����Ϣ
		char	_szLinkName[MAX_PATH];		// �豸��Link ���ƣ�����һ��GUI(\DEVICE\{E110C6A6-95D5-42A8-83B2-6040D63794EC})
		char	_szDevDesc[MAX_PATH];	// �豸����ʾ���ƣ�Ӧ�ú�����������߿�����һ��(Intel(R) PRO/1000 PL Network Connection)
		char	_szShowName[MAX_PATH];	// ���豸������(��������)

		BYTE	_szMacAddress[8];	// �豸��MAC��ַ�����಻�ṩ�޸�MAC��ַ�Ĺ���

	}ONE_ETHER_CARD_INFO;

public:
	CHostEtherCardMgr(void);
	~CHostEtherCardMgr(void);

#ifdef OS_WINDOWS
private:
	int QuaryCardInfoFromReg_Win();
#endif

#ifdef OS_LINUX
private:
	int QuaryCardInfoFromReg_Linux();
#endif

public:
	//�������ȡ���������Ļ�����Ϣ������mac��ַ�����֣�������
	//�����ж��ٸ�����
	int Init();

public:
	int GetOneIndexByMAC(const BYTE * mac_addr) const;
	int GetOneIndexByShowName(const char * shoname) const;

public:
	ONE_ETHER_CARD_INFO		m_array_card_info[32];
	int						m_card_count;

};
