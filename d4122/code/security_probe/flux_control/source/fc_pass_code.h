//////////////////////////////////////////////////////////////////////////
//fc_pass_code.h

#pragma once

typedef enum{
	FC_PC_PASSED = 0,//通过
	FC_PC_FLUX_CONTROL = 0x0001,//流量控制
	FC_PC_L7_ACL = 0x0002,//员工上网访问控制
	FC_PC_AUTH = 0x0004,//认证不通过
	FC_PC_IP_MANAGER = 0x0008,//表示IP地址管理，不通过
	FC_PC_LINK_LIMITED = 0x0010,//并发连接数限制，不通过
	FC_PC_HOME_PAGE = 0x0020,//还没有进行看公告页面，不通过
	FC_PC_NEED_REDIR = 0x0040,//因为需要重定向
	FC_PC_L3_ACL = 0x0080,//因为三层防火墙
	FC_PC_FORBIT_DNS = 0x0100//因为DNS拦截	

}FC_PASS_CODE;


