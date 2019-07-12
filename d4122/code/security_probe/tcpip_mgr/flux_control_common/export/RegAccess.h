#ifndef _REG_ACCESS_H_LIANGQUAN_
#define _REG_ACCESS_H_LIANGQUAN_

#include "flux_control_common.h"

#include <vector>
#include <string>


typedef struct __ST_DEVICE_INFO__
{
	__ST_DEVICE_INFO__()
	{
		iRegCount = 0;
		iEnable = 0;
		iValue = -1;
		iReserve = -1;
	}
	std::string	pszConnectName;
	std::string	pszDesc;
	std::string	pszGuid;
	std::string	pszRegPosi[16];
	int			iRegCount;

	int			iEnable;			// 是否有效
	int			iValue;			// 用外部调用者使用
	int			iReserve;		// 
}ST_DEVICE_INFO;


FLUX_CONTROL_COMMON_API bool REG_SearchAllDevice(std::vector<ST_DEVICE_INFO>& vec_out);


#endif // _REG_ACCESS_H_LIANGQUAN_