//////////////////////////////////////////////////////////////////////////
//os_syscall.h

#pragma once

#include "cpf/cpf.h"

namespace CPF
{
	CPF_CLASS
		//EWX_POWEROFF=0x00000008
		//EWX_REBOOT=0x00000002
		BOOL MySystemShutdown(UINT uFlags);

	//�õ�cpu�ĸ���
	CPF_CLASS
		int GetCpuCount();

	// ��������̰󶨵�ָ��CPU��
	// CPU��������CPU_MAX=CPF::GetCpuCount()������λ���л�ġ�
	//	���cpuID == -1, ��ʾ�󶨵�����CPU��
	CPF_CLASS
	BOOL BindProcessCPU(int cpuID);

	CPF_CLASS
	BOOL BindThreadCPU(int cpuID);


}
