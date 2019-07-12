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

	//得到cpu的个数
	CPF_CLASS
		int GetCpuCount();

	// 将自身进程绑定到指定CPU上
	// CPU的总数是CPU_MAX=CPF::GetCpuCount()，按照位进行或的。
	//	如果cpuID == -1, 表示绑定到所有CPU上
	CPF_CLASS
	BOOL BindProcessCPU(int cpuID);

	CPF_CLASS
	BOOL BindThreadCPU(int cpuID);


}
