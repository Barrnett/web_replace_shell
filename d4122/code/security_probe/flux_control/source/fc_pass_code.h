//////////////////////////////////////////////////////////////////////////
//fc_pass_code.h

#pragma once

typedef enum{
	FC_PC_PASSED = 0,//ͨ��
	FC_PC_FLUX_CONTROL = 0x0001,//��������
	FC_PC_L7_ACL = 0x0002,//Ա���������ʿ���
	FC_PC_AUTH = 0x0004,//��֤��ͨ��
	FC_PC_IP_MANAGER = 0x0008,//��ʾIP��ַ������ͨ��
	FC_PC_LINK_LIMITED = 0x0010,//�������������ƣ���ͨ��
	FC_PC_HOME_PAGE = 0x0020,//��û�н��п�����ҳ�棬��ͨ��
	FC_PC_NEED_REDIR = 0x0040,//��Ϊ��Ҫ�ض���
	FC_PC_L3_ACL = 0x0080,//��Ϊ�������ǽ
	FC_PC_FORBIT_DNS = 0x0100//��ΪDNS����	

}FC_PASS_CODE;


