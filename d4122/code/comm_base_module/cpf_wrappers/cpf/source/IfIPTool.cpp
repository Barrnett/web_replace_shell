//////////////////////////////////////////////////////////////////////////
//IfIPTool.cpp

#include "cpf/IfIPTool.h"
#include "cpf/addr_tools.h"

#ifdef OS_WINDOWS
#include "shellapi.h"
#endif


#ifdef OS_WINDOWS

//nothing

#else//OS_LINUX

#include "net/if.h"
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#ifdef OS_LINUX_UBUNTU

#define file_linux_PAHT "/etc/network/interfaces"

int search(const char *str,int length);
int GenerationDelIPinfo(const char * if_name,std::string *gateway);
int GenerationGetVirtrueNumber(std::string *changedname,const char * if_name,const char *IPadress);

#else//OS_LINUX_CENTOS

#define file_linux_TEMP "/tmp/ifcfg_tmp"
#define file_linux_PATH "/etc/sysconfig/network-scripts/ifcfg-"

#endif//OS_LINUX_UBUNTU

#define file_linux_DNS_PAHT "/etc/resolv.conf"

#endif//OS_WINDOWS

/*

Centos
/etc/sysconfig/network 设置主机名和网络配置
/etc/sysconfig/network-scripts/ifcfg-eth0针对特定的网卡进行设置
/etc/resolv.conf设置DNS
/etc/hosts设置指定的域名解析地址


CentOS 修改DNS

修改对应网卡的DNS的配置文件
# vi /etc/resolv.conf 
修改以下内容

nameserver 8.8.8.8 #google域名服务器
nameserver 8.8.4.4 #google域名服务器
*/




////////////////////////////////////////////////////////////////////////////////////////////////////=

#ifdef OS_WINDOWS
void mySystem(const char * cmd_param)
{
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = "netsh";//调用的程序名
	ShExecInfo.lpParameters =cmd_param;//调用程序的命令行参数
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;//窗口状态为隐藏
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);////等到该进程结束
	return;
} 


int CPF::SetIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	char buf_ip[32];
	strcpy(buf_ip,CPF::hip_to_string(ipaddr));

	char buf_mask[32];
	strcpy(buf_mask,CPF::hip_to_string(mask));

	if( gw == -1 || gw == 0  )
	{//需要删除网关或者网关保持不变

		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip set address name=\"%s\" source=static %s %s",if_name,buf_ip,buf_mask);
		mySystem(buf_cmd);
	}
	else
	{
		char buf_gateway[32];
		strcpy(buf_gateway,CPF::hip_to_string(gw));

		char buf_cmd[256];
		sprintf(buf_cmd,"interface ip set address name=\"%s\" source=static %s %s %s %d",if_name,buf_ip,buf_mask,buf_gateway,gwmetric);
		mySystem(buf_cmd);
	}

	if( gw == 0 )
	{//删除网关

		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip delete address %s gateway=all",if_name);

		mySystem(buf_cmd);

	}

	return 0;
}

int CPF::SetIfIPByCmd_Dhcp(const char * if_name)
{
	char buf_cmd[256];

	sprintf(buf_cmd,"interface ip set address name=\"%s\" source=dhcp",if_name);

	mySystem(buf_cmd);

	return 0;
}


int CPF::AddIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	char buf_ip[32];
	strcpy(buf_ip,CPF::hip_to_string(ipaddr));

	char buf_mask[32];
	strcpy(buf_mask,CPF::hip_to_string(mask));

	if( gw == -1 || gw == 0  )
	{//表示不设置gw

		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip add address name=\"%s\" %s %s",if_name,buf_ip,buf_mask);
		mySystem(buf_cmd);
	}
	else
	{
		char buf_gateway[32];
		strcpy(buf_gateway,CPF::hip_to_string(gw));

		char buf_cmd[256];
		sprintf(buf_cmd,"interface ip add address name=\"%s\" %s %s %s %d",if_name,buf_ip,buf_mask,buf_gateway,gwmetric);
		mySystem(buf_cmd);
	}

	return 0;

}


int CPF::DelIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 gw)
{
	if( ipaddr != 0 )
	{
		char buf_ip[32];
		strcpy(buf_ip,CPF::hip_to_string(ipaddr));

		char buf_cmd[256];
		sprintf(buf_cmd,"interface ip delete address name=\"%s\" %s %s",if_name,buf_ip);

		mySystem(buf_cmd);

	}

	if( gw == -1 )
	{
		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip delete address name=\"%s\" gateway=all",if_name);

		mySystem(buf_cmd);
	}
	else if( gw != 0 )
	{
		char buf_gw[32];
		strcpy(buf_gw,CPF::hip_to_string(gw));

		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip delete address name=\"%s\" gateway=%s",if_name,buf_gw);

		mySystem(buf_cmd);
	}

	return 0;	
}


int CPF::SetIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dns1_ipaddr,ACE_UINT32 dns2_ipaddr)
{
	char str_dns1_ipaddr[32];
	strcpy(str_dns1_ipaddr,CPF::hip_to_string(dns1_ipaddr));

	char buf_cmd[256];
	sprintf(buf_cmd,"interface ip set dns name=\"%s\" static %s primary",if_name,str_dns1_ipaddr);

	mySystem(buf_cmd);

	return 0;
}


int CPF::SetIfDNSByCmd_Dhcp(const char * if_name)
{
	char buf_cmd[256];

	sprintf(buf_cmd,"interface ip set dns name=\"%s\" source=dhcp",if_name);

	mySystem(buf_cmd);

	return 0;
}

//删除dns地址，dhsipaddr==-1表示删除所有dns，其他表示删除指定dns	
CPF_CLASS
int CPF::DelIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dhsipaddr)
{
	if( dhsipaddr == -1 )
	{
		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip delete dns name=\"%s\" all",if_name);

		mySystem(buf_cmd);
	}
	else if( dhsipaddr != 0 )
	{
		char buf_dns[32];
		strcpy(buf_dns,CPF::hip_to_string(dhsipaddr));

		char buf_cmd[256];

		sprintf(buf_cmd,"interface ip delete dns name=\"%s\" %s",if_name,buf_dns);

		mySystem(buf_cmd);
	}

	return 0;	
}

/////////////////////////////////////////////////////////////////////////////////////////
#elif OS_LINUX_UBUNTU

void tool_restart_network()
{
	system("/etc/init.d/networking restart");
}


int CPF::SetIfDNSByCmd_Dhcp(const char * if_name)
{
	return 0;
}
int  CPF::DelIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dhsipaddr)
{
	char strBuff[512];
	FILE *fpTemp = NULL;
	char* pTemp = NULL;
	if(dhsipaddr == -1)
	{

		sprintf(strBuff, "sed -i '/.*$/d' %s",file_linux_DNS_PAHT);
		if((fpTemp = popen(strBuff,"r")) == NULL)
		{
			perror("erro");
		}
		pclose(fpTemp);
	}

	else if (dhsipaddr != 0)
	{
		sprintf(strBuff, "sed -i '/%s/d' %s",CPF::hip_to_string(dhsipaddr),file_linux_DNS_PAHT);
		if((fpTemp = popen(strBuff,"r")) == NULL)
		{
			perror("erro");
		}
		pclose(fpTemp);

	}

	tool_restart_network();

	return 0;

}
int CPF::SetIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dns1_ipaddr,ACE_UINT32 dns2_ipaddr)
{
	char strBuff[512];
	FILE *fpTemp = NULL;
	char* pTemp = NULL;

	if(dns1_ipaddr != 0)
	{
		sprintf(strBuff, "echo 'nameserver %s' >> %s",CPF::hip_to_string(dns1_ipaddr),file_linux_DNS_PAHT);

		if((fpTemp = popen(strBuff,"r")) == NULL)
		{
			perror("erro");
		}
		pclose(fpTemp);
	}

	if(dns2_ipaddr != 0)
	{
		sprintf(strBuff, "echo 'nameserver %s' >> %s",CPF::hip_to_string(dns2_ipaddr),file_linux_DNS_PAHT);

		if((fpTemp = popen(strBuff,"r")) == NULL)
		{
			perror("erro");
		}
		pclose(fpTemp);

	}
	
	tool_restart_network();

	return 0;

}
int CPF:: DelIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 gw)
{
	std::string gateway_old;
	std::string changedname;

	if (ipaddr != 0)
	{
		char buf_ip[32];
		strcpy(buf_ip,CPF::hip_to_string(ipaddr));

		if( -1 == GenerationGetVirtrueNumber(&changedname,if_name,buf_ip) )
		{
			return -1;
		}

		const char *str_name;
		str_name = changedname.c_str();

		if(strlen(str_name) != 0)
		{
			if( -1 != GenerationDelIPinfo(str_name,&gateway_old) )
			{
				FILE *fpTemp = NULL;
				char str_cmd[512];
				sprintf(str_cmd,"mv /temp %s", file_linux_PAHT);
				char* pTemp = NULL;
				if((fpTemp = popen(str_cmd,"r")) == NULL)
				{
					perror("erro");
				}
				pclose(fpTemp);
				fpTemp = NULL;
			}
		}

		tool_restart_network();
	}
	return 0;
}
int CPF::AddIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	std::string changedname;

	char buf_ip[32];
	strcpy(buf_ip,CPF::hip_to_string(ipaddr));
	char buf_mask[32];
	strcpy(buf_mask,CPF::hip_to_string(mask));

	if( -1 == GenerationGetVirtrueNumber(&changedname,if_name,buf_ip) )
	{
		return;
	}

	int length = strlen(if_name);
	char name_compare[512];

	char name_max[512];

	sprintf(name_max,"%s:10",if_name);

	std::string gateway_old;

	if(changedname.length()!=0)
	{
		sprintf(name_compare,"%s",changedname.c_str());
		if( -1 == GenerationDelIPinfo(name_compare,&gateway_old) )
		{
			return -1;
		}
	}
	else
	{
		for(int i=0;i<10;i++)
		{
			sprintf(name_compare,"%s:%d",if_name,i);
			if(search(name_compare,length) == 0)
			{
				if( -1 == GenerationDelIPinfo(name_max,&gateway_old) )
				{
					return -1;
				}
			}
		}
	}

	FILE *fpout = NULL;
	fpout = fopen("/temp","at");
	fprintf(fpout,"\n");
	fprintf(fpout,"auto %s\n",name_compare);
	fprintf(fpout,"iface %s inet static\n",name_compare);
	fprintf(fpout,"\taddress %s\n",buf_ip);
	fprintf(fpout,"\tnetmask %s\n",buf_mask);
	if (gw == -1)
	{
		fprintf(fpout,"\t%s\n",gateway_old.c_str());
	}
	else if (gw == 0)
	{
	}
	else
	{
		fprintf(fpout,"\tgateway %s\n",CPF::hip_to_string(gw));
	}

	fprintf(fpout,"\n");
	fclose(fpout);
	FILE *fpTemp = NULL;
	char* pTemp = NULL;
	char str_cmd[512];
	sprintf(str_cmd,"mv /temp %s", file_linux_PAHT);
	if((fpTemp = popen(str_cmd,"r")) == NULL)
	{
		perror("erro");
	}
	pclose(fpTemp);

	tool_restart_network();
	return 0;

}

int CPF::SetIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	std::string gateway_old;

	if( -1 == GenerationDelIPinfo(if_name,&gateway_old) )
	{
		return -1;
	}

	FILE *fpout = NULL;
	fpout = fopen("/temp","at");
	fprintf(fpout,"auto %s\n",if_name);
	fprintf(fpout,"iface %s inet static\n",if_name);
	fprintf(fpout,"\taddress %s\n",CPF::hip_to_string(ipaddr));
	fprintf(fpout,"\tnetmask %s\n",CPF::hip_to_string(mask));

	if (gw == -1)
	{
		fprintf(fpout,"\t%s\n",gateway_old.c_str());
	}
	else if (gw == 0)
	{
	}
	else
	{
		fprintf(fpout,"\tgateway %s\n",CPF::hip_to_string(gw));
	}
	fprintf(fpout,"\n");
	fclose(fpout);
	FILE *fpTemp = NULL;
	char* pTemp = NULL;
	char str_cmd[512];
	sprintf(str_cmd,"mv /temp %s", file_linux_PAHT);
	if((fpTemp = popen(str_cmd,"r")) == NULL)
	{
		perror("erro");
	}
	pclose(fpTemp);

	tool_restart_network();
}

int CPF::SetIfIPByCmd_Dhcp(const char * if_name)
{
	std::string gateway_old;

	if( -1 == GenerationDelIPinfo(if_name,&gateway_old) )
	{
		return -1;
	}

	FILE *fpout = NULL;
	fpout = fopen("/temp","at");
	fprintf(fpout,"\n");
	fprintf(fpout,"auto %s\n",if_name);
	fprintf(fpout,"iface %s inet dhcp\n",if_name);
	fprintf(fpout,"\n");
	fclose(fpout);
	FILE *fpTemp = NULL;
	char* pTemp = NULL;
	char str_cmd[512];
	sprintf(str_cmd,"mv /temp %s", file_linux_PAHT);

	if((fpTemp = popen(str_cmd,"r")) == NULL)
	{
		perror("erro");
	}
	pclose(fpTemp);

	tool_restart_network();
	
	return 0;

}

int search(const char *str,int length)
{
	FILE *fp = NULL;
	char *line = NULL;
	size_t n;
	const char *compare = NULL;

	if( !(fp = fopen(file_linux_PAHT, "r")) )
	{
		return -1;
	}

	while(getline(&line, &n, fp) != -1)
	{
		std::string str_in(line);
		if(str_in.length()>=5 && str_in.length()<=12)
		{
			compare = str_in.substr(5,str_in.length()-length).c_str();
			if(strcmp(str,compare) == 0)
			{
				fclose(fp);
				fp = NULL;
				return 1;
				break;
			}
		}
	}
	return 0;
}
int GenerationDelIPinfo(const char * if_name,std::string *gateway)
{
	int str_change = 0;
	FILE *fp = NULL;
	FILE *fpout = NULL;
	char *line = NULL;
	size_t n;

	if( !(fp = fopen(file_linux_PAHT, "r")) )
	{
		return -1;
	}
	
	if( !(fpout = fopen("/temp","w")) )
	{
		fclose(fp);
		return -1;
	}
	
	while(getline(&line, &n, fp) != -1)
	{
		std::string str_in(line);
		if(str_change == 1 && (str_in.find("gateway")!=str_in.npos))
		{
			*gateway = line;
		}

		if(str_change == 1 && (str_in.find("auto")!=str_in.npos))
		{
			str_change = 0;
		}
		if(str_in.find(if_name) != str_in.npos)
		{
			str_change = 1;
		}

		if(str_change == 0)
		{
			fprintf(fpout,"%s",str_in.c_str());
		}

	}
	fclose(fp);
	fclose(fpout);
	fp = NULL;
	fpout = NULL;
	return 0;

}
int GenerationGetVirtrueNumber(std::string *changedname,const char * if_name,const char *IPadress)
{
	std::string str_ip;
	str_ip = *IPadress;
	int str_change = 0;
	int virtruenumber = 0;
	const char *virtruename = NULL;
	char *line = NULL;
	size_t n;

	FILE *fp = NULL;
	if( !(fp = fopen(file_linux_PAHT, "r")) )
	{
		return -1;
	}

	while(getline(&line, &n, fp) != -1)
	{
		std::string str_in(line);
		if(str_change == 1)
		{
			const char * str_temp;
			if(str_in.find("address")!=str_in.npos)
			{
				int length = strlen(IPadress);
				str_temp = str_in.substr(9,length).c_str();
				if(strcmp(IPadress,str_temp) == 0)
				{
					char str[512];
					sprintf(str,"%s",virtruename);
					*changedname = str;
					break;
				}
			}
		}

		if(str_change == 1 && (str_in.rfind("auto")!=str_in.npos))
		{
			str_change = 0;
		}
		if(str_in.find(if_name) != str_in.npos)
		{
			if(str_change == 0)
			{
				virtruename = str_in.substr(5,str_in.length()-7).c_str();
				virtruenumber++;
			}
			str_change = 1;
		}

	}
	fclose(fp);
	fp = NULL;
	return virtruenumber -1;
}


////////////////////////////////////////////////////////////////////////////////////////////

#else // OS_LINUX CENT_OS


// 在一个文本文件中删除一行，pszItem如果多次出现均会删除
void tool_del_line(const char* pszFileName, const char* pszItem)
{
	if (!pszFileName || !pszItem)
		return;

	char szBuf[1024];
	
	sprintf(szBuf,"sed -i -e '/%s/d' %s", pszItem, pszFileName);
	system(szBuf);
}

void tool_add_line(const char* pszFileName, const char* pszItem)
{
	FILE* fp = fopen(pszFileName, "at");

	if( fp )
	{
		fprintf(fp, "%s\n", pszItem);

		fclose(fp);
	}

	return;
}

// 删除和 IP配置相关的部分
void tool_del_ipcfg(const char* pszFileName)
{
//	tool_del_line(pszFileName, "BOOTPROTO");
	tool_del_line(pszFileName, "IPADDR");
	tool_del_line(pszFileName, "NETMASK");
	tool_del_line(pszFileName, "NETWORK");
	tool_del_line(pszFileName, "BROADCAST");
	tool_del_line(pszFileName, "PREFIX");
	tool_del_line(pszFileName, "GATEWAY");
	
}

void tool_restart_network()
{
	system("/etc/init.d/network restart");
}


bool tool_run_shell(const char* pszCmd)
{
	FILE* fpTemp = NULL;
	if((fpTemp = popen(pszCmd, "r")) == NULL)
	{
		perror("erro");
		return false;
	}

	pclose(fpTemp);

	return true;
}

void do_effect_ipcfg(const char * strCfgFile)
{
	char szShell[260] = {0};

	sprintf(szShell, "cp -r %s %s", file_linux_TEMP, strCfgFile);
	tool_run_shell(szShell);

	tool_restart_network();

	return;
}

typedef struct _ST_IP_CONFIG
{
	enum{CFG_MAX=4};
	std::string strConfig[4];       // ip, mask, gw, prefix

	void init(ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw)
	{
		strConfig[0] = CPF::hip_to_string(ipaddr);
		strConfig[1] = CPF::hip_to_string(mask);
		if (0!=gw && -1!=gw)
			strConfig[2] = CPF::hip_to_string(gw);
		strConfig[3] = "";

	}

//   std::string strIP;
//   std::string strMASK;
//   std::string strGATEWAY;
//   std::string strPreFix;  // = mask,
}ST_IP_CONFIG;

std::vector<ST_IP_CONFIG> g_ipCfg;

bool tool_get_oneip(int iIdx, const char* pszFile, std::vector<ST_IP_CONFIG> &ipCfg)
{
	char szTmp[ST_IP_CONFIG::CFG_MAX][64] = {0};

	if (iIdx <= 1)
	{
		sprintf(szTmp[0], "grep %s= %s", "IPADDR", pszFile);
		sprintf(szTmp[1], "grep %s= %s", "NETMASK", pszFile);
		sprintf(szTmp[2], "grep %s= %s", "GATEWAY", pszFile);
		sprintf(szTmp[3], "grep %s= %s", "PREFIX", pszFile);
	}
	else
	{
		sprintf(szTmp[0], "grep %s%d %s", "IPADDR", iIdx, pszFile);
		sprintf(szTmp[1], "grep %s%d %s", "NETMASK", iIdx, pszFile);
		sprintf(szTmp[2], "grep %s%d %s", "GATEWAY", iIdx, pszFile);
		sprintf(szTmp[3], "grep %s%d %s", "PREFIX", iIdx, pszFile);
	}

	ST_IP_CONFIG stItem;
	for (int i=0; i<ST_IP_CONFIG::CFG_MAX; ++i)
	{
		printf("\n\n=== %s\n", szTmp[i]);
	
		FILE *fp = popen(szTmp[i], "r");

		if (!fp)
		{
			printf("[tool_get_onip] can't exe command: %s\n", szTmp[i]);
			return false;
		}

		char tmp[256] = {0};
		char* p = fgets(tmp, sizeof(tmp), fp);
		pclose(fp);

		if(!p)
		{
			if (0 == i)
			{
				printf("[tool_get_onip] can't find IPADDR item: %s\n", szTmp[i]);
				return false;
			}
			else
			{
				continue;
			}
		}

		p = strchr(tmp, '=');
		if (!p)
		{
			continue;
		}

		++p;

		{//去掉后面的'\n'
			char * q = p+strlen(p)-1;

			while( q > p )
			{
				if( *q == 'r' || *q == '\n' )
				{
					*q = 0;
				}

				--q;
			}
		}		

		stItem.strConfig[i] = p;
		

//		printf("buf=%s\n", p);
	}

	ipCfg.push_back(stItem);

	return true;
}

bool tool_get_allip(const char* pszFile) 
{
	g_ipCfg.clear();

	for (int i=1; i<10; ++i)
	{
		bool bRet=tool_get_oneip(i, pszFile, g_ipCfg);
		if (!bRet)
			break;
	}

	return true;
}

bool tool_put_oneip(const char* pszFile, int iIdx, ST_IP_CONFIG& stItem)
{
	FILE* fp = fopen(pszFile, "at");
	if (!fp)
	{
		return false;
	}

	if (0 == iIdx)
	{
		fprintf(fp, "%s=%s\n", "IPADDR", stItem.strConfig[0].c_str());
		if ( !stItem.strConfig[1].empty() )
			fprintf(fp, "%s=%s\n", "NETMASK", stItem.strConfig[1].c_str());
		if ( !stItem.strConfig[2].empty() )
			fprintf(fp, "%s=%s\n", "GATEWAY", stItem.strConfig[2].c_str());
		if ( !stItem.strConfig[3].empty() )
			fprintf(fp, "%s=%s\n", "PREFIX", stItem.strConfig[3].c_str());
	}
	else
	{
		fprintf(fp, "%s%d=%s\n", "IPADDR", iIdx+1, stItem.strConfig[0].c_str());
		if ( !stItem.strConfig[1].empty() )
			fprintf(fp, "%s%d=%s\n", "NETMASK", iIdx+1, stItem.strConfig[1].c_str());
		if ( !stItem.strConfig[2].empty() )
			fprintf(fp, "%s%d=%s\n", "GATEWAY", iIdx+1, stItem.strConfig[2].c_str());
		if ( !stItem.strConfig[3].empty() )
			fprintf(fp, "%s%d=%s\n", "PREFIX", iIdx+1, stItem.strConfig[3].c_str());
	}
	
	fclose(fp);

	return true;
}

bool tool_put_allip(const char* pszFile)
{
	for (size_t i=0; i<g_ipCfg.size(); ++i)
	{
		ST_IP_CONFIG& stItem = g_ipCfg[i];

		tool_put_oneip(pszFile, i, stItem);
	}

	return true;
}

//删除地址，如果gw=0表示不修改网关，gw==-1表示删除所有网关，其他表示删除指定网关
//如果ip==0，表示对IP地址不修改，只删除网关
bool tool_vet_removeip(ACE_UINT32 ipaddr, ACE_UINT32 gw)
{
	char strBuff[512];
	sprintf(strBuff, "%s", CPF::hip_to_string(ipaddr));
	if (0 != ipaddr)
	{
		for (size_t i=0; i<g_ipCfg.size(); ++i)
		{
			if (0 == g_ipCfg[i].strConfig[0].compare(strBuff) )
			{
				g_ipCfg.erase(g_ipCfg.begin() + i);

				break;
			}
		}

	}

	if (0 == gw)
	{
		return true;
	}

	sprintf(strBuff, "%s", CPF::hip_to_string(gw));
	for (size_t i=0; i<g_ipCfg.size(); ++i)
	{
		if (-1 == gw)
			g_ipCfg[i].strConfig[2].clear();

		else if (0 == g_ipCfg[i].strConfig[2].compare(strBuff) )
		{
			g_ipCfg[i].strConfig[2].clear();
		}
	}

	return true;
}

bool tool_vet_addip(ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	// 先删除，防止重复
	tool_vet_removeip(ipaddr, 0);

	ST_IP_CONFIG stItem;
	stItem.init(ipaddr, mask, gw);

	g_ipCfg.push_back(stItem);

	return true;
}


int CPF::SetIfDNSByCmd_Dhcp(const char * if_name)
{

	return 0;
}

// ok
int  CPF::DelIfDNSByCmd_Static(const char * if_name, ACE_UINT32 dhsipaddr)
{
	if(dhsipaddr == -1)
	{
		tool_del_line("nameserver", file_linux_DNS_PAHT);
	}

	else if (dhsipaddr != 0)
	{
		char strBuff[512];
		sprintf(strBuff, "%s", CPF::hip_to_string(dhsipaddr));
		tool_del_line(strBuff, file_linux_DNS_PAHT);
	}

	// /etc/resolv.conf修改后立刻生效，不用重启网络服务
	tool_restart_network();

	return 0;

}

// 保留
int CPF::SetIfDNSByCmd_Static(const char * if_name,ACE_UINT32 dns1_ipaddr,ACE_UINT32 dns2_ipaddr)
{
	char strBuff[512];
	FILE *fpTemp = NULL;
	char* pTemp = NULL;

	if(dns1_ipaddr != 0)
	{
		sprintf(strBuff, "echo 'nameserver %s' >> %s",CPF::hip_to_string(dns1_ipaddr),file_linux_DNS_PAHT);

		if((fpTemp = popen(strBuff,"r")) == NULL)
		{
			perror("erro");
		}
		pclose(fpTemp);
	}

	if(dns2_ipaddr != 0)
	{
		sprintf(strBuff, "echo 'nameserver %s' >> %s",CPF::hip_to_string(dns2_ipaddr),file_linux_DNS_PAHT);

		if((fpTemp = popen(strBuff,"r")) == NULL)
		{
			perror("erro");
		}
		pclose(fpTemp);

	}
	
	tool_restart_network();

	return 0;

}
int CPF:: DelIfIPByCmd_Static(const char * if_name, ACE_UINT32 ipaddr,ACE_UINT32 gw)
{
	// to tmp file
	std::string strCfgFile = file_linux_PATH;
	strCfgFile += if_name;

	char szShell[MAX_PATH];
	sprintf(szShell, "cp -r %s %s", strCfgFile.c_str(), file_linux_TEMP);
	tool_run_shell(szShell);

	// get current ip list
	g_ipCfg.clear();
	tool_get_allip(strCfgFile.c_str());

	// remove from vector
	tool_vet_removeip(ipaddr, gw);

	tool_del_line(file_linux_TEMP,"DEVICE");

	tool_del_ipcfg(file_linux_TEMP);

	char device_info[256];

	sprintf(device_info,"DEVICE=\"%s\"",if_name);

	tool_add_line(file_linux_TEMP,device_info);

	// write ipcofnig to tmp file
	tool_put_allip(file_linux_TEMP);

	do_effect_ipcfg(strCfgFile.c_str());

	return 0;
}

int CPF::AddIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	// to tmp file
	std::string strCfgFile = file_linux_PATH;
	strCfgFile += if_name;

	char szShell[MAX_PATH];
	sprintf(szShell, "cp -r %s %s", strCfgFile.c_str(), file_linux_TEMP);
	tool_run_shell(szShell);

	// get current ip list
	g_ipCfg.clear();
	tool_get_allip(strCfgFile.c_str());

	int iCount = g_ipCfg.size();
	ST_IP_CONFIG stItem;
	stItem.init(ipaddr, mask, gw);

	// write ipcofnig to tmp file
	tool_put_oneip(file_linux_TEMP, iCount, stItem);

	do_effect_ipcfg(strCfgFile.c_str());

	return 0;
}

int CPF::SetIfIPByCmd_Static(const char * if_name,ACE_UINT32 ipaddr,ACE_UINT32 mask,ACE_UINT32 gw,int gwmetric)
{
	// to tmp file
	std::string strCfgFile = file_linux_PATH;
	strCfgFile += if_name;

	char szShell[MAX_PATH];
	sprintf(szShell, "cp -r %s %s", strCfgFile.c_str(), file_linux_TEMP);
	tool_run_shell(szShell);

	// delte all cfg
	tool_del_ipcfg(file_linux_TEMP);

	tool_del_line(file_linux_TEMP, "BOOTPROTO");

	tool_del_line(file_linux_TEMP,"DEVICE");

	char device_info[256];

	sprintf(device_info,"DEVICE=\"%s\"",if_name);

	tool_add_line(file_linux_TEMP,device_info);

	// write ipcofnig to tmp file
	ST_IP_CONFIG stItem;
	stItem.init(ipaddr, mask, gw);

	// write ipcofnig to tmp file
	tool_put_oneip(file_linux_TEMP, 0, stItem);

	do_effect_ipcfg(strCfgFile.c_str());

	return 0;
}

int CPF::SetIfIPByCmd_Dhcp(const char * if_name)
{
	// to tmp file
	std::string strCfgFile = file_linux_PATH;
	strCfgFile += if_name;

	char szShell[MAX_PATH];
	sprintf(szShell, "cp -r %s %s", strCfgFile.c_str(), file_linux_TEMP);
	tool_run_shell(szShell);

	// delte all cfg
	tool_del_ipcfg(file_linux_TEMP);

	tool_del_line(file_linux_TEMP, "BOOTPROTO");
	tool_del_line(file_linux_TEMP, "DNS");

	FILE* fp = fopen(file_linux_TEMP, "at");
	if (fp)
	{
		fprintf(fp, "BOOTPROTO=dhcp");
		fclose(fp);
	}

	do_effect_ipcfg(strCfgFile.c_str());

	return 0;
}


#endif
