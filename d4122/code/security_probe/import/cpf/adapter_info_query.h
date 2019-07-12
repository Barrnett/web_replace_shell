//////////////////////////////////////////////////////////////////////////
// adapter_info_query.h 

#include "cpf/ostypedef.h"
int adapter_query_dev_name(char* pszBufOut, int iBufLen);

int adapter_query_mac(const char* pszInterface, char* pMac);
int adapter_query_mac_str(const char* pszInterface, char* pszOutMac, char chSec=':');

int adapter_query_ip(const char* pszInterface, unsigned int& ipAddr);
int adapter_query_ip_str(const char* pszInterface, char* pszOutIP);

int adapter_query_mask(const char* pszInterface, unsigned int& maskAddr);
int adapter_query_gateway(const char* pszInterface, unsigned int& iGateWay);

int adapter_query_mtu(const char* pszInterface, unsigned int& mtu);

