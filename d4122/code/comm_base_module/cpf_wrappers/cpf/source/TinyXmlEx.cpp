//////////////////////////////////////////////////////////////////////////
//TineXmlEx.cpp

#include "cpf/TinyXmlEx.h"
#include "cpf/strtools.h"
#include "cpf/other_tools.h"
#include "cpf/addr_tools.h"
#include "cpf/path_tools.h"
#include "cpf/URLDecoder.h"

// 支持0x、0X、bit、BIT、Bit
#define MK_U16(xx)	*(ACE_UINT16 *)xx
#define MK_U32(xx)	*(ACE_UINT32 *)xx
ACE_UINT64 SpecialString2Uint(const char* pText)
{
	ACE_UINT64 ivalue = 0;
	size_t len = strlen(pText);
	ACE_UINT16 u16 = *(ACE_UINT16 *)pText;
	ACE_UINT32 u32 = (*(ACE_UINT32 *)pText) & 0x00FFFFFF;

	if (	( len > 2 )
			&&
			( u16 == MK_U16("0x") ||u16 == MK_U16("0X"))
		)
	{
		sscanf(pText+2,"%llx",&ivalue);
	}

	else if (	( len > 3 )
				&&
				( u32==MK_U32("Bit") || u32==MK_U32("bit") || u32==MK_U32("BIT") )
			)
	{
		int iPosi = 0;
		sscanf(pText+3,"%d",&iPosi);

		ivalue = 1;
		ivalue <<= iPosi;
	}

	else
	{
		sscanf(pText,"%llu",&ivalue);
	}


	return ivalue;
}


const BYTE g_szDefaultFileContext[64] = 
{
	"<?xml version=\"1.0\" encoding=\"GB2312\" ?>",
};

//public函数实现
CTinyXmlEx::CTinyXmlEx()
{
}

CTinyXmlEx::~CTinyXmlEx()
{
	close();
}

int CTinyXmlEx::open(const ACE_TCHAR * pszXMLFullName)
{	
	if (!m_XmlDoc.LoadFile(pszXMLFullName))
	{
		return -3;
	}

	return 0;
}

int CTinyXmlEx::open(const char * buf,int length)
{
	if( !m_XmlDoc.LoadFromMem(buf,length) )
	{
		return -3;
	}

	return 0;
}


int CTinyXmlEx::open_ex(const char *pszXMLFullName, OPEN_MODE enMode)
{
	if (NULL == pszXMLFullName)
	{
		return -1;
	}

	// 如果文件存在并且模式为写打开，则正常初始化
	if (   (-1 != access(pszXMLFullName, 0))
		&& (OPEN_MODE_OPEN_CREATE == enMode) )
	{
		return open(pszXMLFullName);
	}

	// 否则，文件存在并且模式为写新文件，
	// 或 文件不存在
	// 则都把文件重新初始化
	FILE *fp = fopen(pszXMLFullName, "w");
	if (NULL == fp)
	{
		return -2;
	}

	fprintf(fp, "%s\n", g_szDefaultFileContext);

	fclose(fp);

	return open(pszXMLFullName);
}

bool CTinyXmlEx::savefile() const
{
	return m_XmlDoc.SaveFile();
}

bool CTinyXmlEx::savefile( const char * filename ) const
{
	return m_XmlDoc.SaveFile(filename);
}


void CTinyXmlEx::close()
{
	return ;
}

TiXmlNode * CTinyXmlEx::GetOrCreateRootElement(const char * root_name,BOOL * pbCreate)
{
	TiXmlNode* pRootNode = this->GetRootNode();

	if( pRootNode )
	{
		if( pbCreate )
			*pbCreate = false;
	}
	else
	{
		pRootNode = 
			this->InsertEndChildElement(&m_XmlDoc,root_name);

		if( pbCreate )
			*pbCreate = true;
	}

	return pRootNode;

}

//获取第一个名字为childname的element,如果没有则创建一个
TiXmlNode * CTinyXmlEx::GetOrCreateFirstChildElement(TiXmlNode *pParentNode,const char * childname,BOOL * pbCreate)
{
	TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

	if( tempNode )
	{
		if( pbCreate )
			*pbCreate = false;
	}
	else
	{
		tempNode = InsertEndChildElement(pParentNode,childname);

		if( pbCreate )
			*pbCreate = true;

	}

	return tempNode;
}

TiXmlNode * CTinyXmlEx::InsertRootElement(const char * name)
{
	return InsertEndChildElement(&m_XmlDoc,name);
}

TiXmlNode * CTinyXmlEx::InsertEndChildElement(TiXmlNode *pParentNode,const char * name)
{	
	TiXmlElement new_child_Element(name);

	return pParentNode->InsertEndChild(new_child_Element);
}


TiXmlNode *CTinyXmlEx::FirstChildElement(TiXmlNode *pNode,const char *_value) const
{
	return pNode->FirstChildElement(_value);
}


TiXmlNode *CTinyXmlEx::NextSiblingElement(TiXmlNode *pNode,const char *_value) const
{
	return pNode->NextSiblingElement(_value);
}

//删除pParentNode下面，所有的名字为subitem的节点
void CTinyXmlEx::RemoveAllChild(TiXmlNode *parent_node,const char * subitem)
{
	TiXmlNode * sub_node = parent_node->FirstChildElement(subitem);

	while( sub_node )
	{
		TiXmlNode * sub_node_next = sub_node->NextSiblingElement(subitem);

		parent_node->RemoveChild(sub_node);

		sub_node = sub_node_next;
	}

	return;
}

BOOL CTinyXmlEx::GetChildElementNodes(TiXmlNode& ParentNode,NodeList &ChildNodes)
{
	if(ParentNode.NoChildren())
		return FALSE;

	for( TiXmlNode *pChildNode =ParentNode.FirstChildElement();
		pChildNode != NULL;
		pChildNode=pChildNode->NextSiblingElement() )
	{
		ChildNodes.push_back(pChildNode);
	}
	return TRUE;
}

bool CTinyXmlEx::GetValue(TiXmlNode *pNode,ACE_UINT16 &ivalue)
{
	ACE_UINT64 u64 = 0;
	if (!GetValue(pNode, u64))
		return false;

	ivalue = (ACE_UINT16)u64;
	return true;
}

bool CTinyXmlEx::GetValue(TiXmlNode *pNode,ACE_UINT32 &ivalue)
{
	ACE_UINT64 u64 = 0;
	if (!GetValue(pNode, u64))
		return false;

	ivalue = (ACE_UINT32)u64;
	return true;
}

bool CTinyXmlEx::GetValue(TiXmlNode *pNode,ACE_UINT64 &ivalue)
{
	if( pNode->ToElement() )
	{
		const char * ptext = pNode->ToElement()->GetText();
		if( !ptext )
		{
			return false;
		}

		ivalue = SpecialString2Uint(ptext);

		return true;
	}

	return false;	
}


bool CTinyXmlEx::GetValue(TiXmlNode *pNode,int &ivalue)
{
	if( pNode->ToElement() )
	{
		const char * ptext = pNode->ToElement()->GetText();

		if( ptext )
		{
			sscanf(ptext,"%d",&ivalue);

			return true;
		}
	}

	return false;
}

bool CTinyXmlEx::GetValue(TiXmlNode *pNode,std::string& value)
{
	if( pNode->ToElement() )
	{
		const char * ptext = pNode->ToElement()->GetText();

		if( ptext )
		{
			value = ptext;
			return true;
		}
	}

	return false;
}


bool CTinyXmlEx::GetValue(TiXmlNode *pNode,char * buf,size_t buflen)
{
	if( pNode->ToElement() )
	{
		const char * ptext = pNode->ToElement()->GetText();

		if( ptext )
		{
			strncpy(buf,ptext,buflen);
			buf[buflen-1]=0;
		}	

		return true;
	}

	return false;
}

bool CTinyXmlEx::GetValue(TiXmlNode *pNode,double& value)
{
	if( pNode->ToElement() )
	{
		const char * ptext = pNode->ToElement()->GetText();

		if( ptext )
		{
			value = atof(ptext);
			return true;
		}

	}

	return false;
}

bool CTinyXmlEx::GetValue(TiXmlNode *pNode,float& value)
{
	double temp_value;

	if( GetValue(pNode,temp_value) )
	{
		value = (float)temp_value;

		return true;
	}

	return false;
}

BOOL CTinyXmlEx::GetValue_UintRange(TiXmlNode *pNode,unsigned int& value1,unsigned int& value2)
{
	std::string str_value;

	if( !GetValue(pNode,str_value) )
		return false;

	return CPF::ParseUintRange(str_value.c_str(),str_value.size(),value1,value2);
}


//用于item=192.168.0.1的IP地址类型或者mask的获取
BOOL CTinyXmlEx::GetValue_HostOrderIP(TiXmlNode *pNode, ACE_UINT32& value)
{
	std::string str_ip;
	
	if( !GetValue(pNode,str_ip) )
		return false;

	value = ACE_NTOHL(inet_addr(str_ip.c_str()));

	return true;
}

//用于item="192.168.0.1-192.168.0.255"的IP地址范围
BOOL CTinyXmlEx::GetValue_HostOrderIPRange(TiXmlNode *pNode, ACE_UINT32& begin_ip,ACE_UINT32& end_ip)
{
	std::string str_ip_range;

	if( !GetValue(pNode,str_ip_range) )
		return false;

	if( !CPF::ParseIPRange(str_ip_range.c_str(),str_ip_range.size(),begin_ip,end_ip) )
		return false;
	
	begin_ip = ACE_NTOHL(begin_ip);
	end_ip = ACE_NTOHL(end_ip);

	return true;
}


BOOL CTinyXmlEx::GetValue_HostOrderIPVector(TiXmlNode * parent_node,
								std::vector<ACE_UINT32>& vt_value,
								const char * subitem)
{
	for( TiXmlNode * iprange_node = parent_node->FirstChildElement(subitem);
		iprange_node;
		iprange_node = iprange_node->NextSiblingElement(subitem) )
	{
		ACE_UINT32 ip=0;

		if( GetValue_HostOrderIP(iprange_node,ip) )
		{
			vt_value.push_back(ip);
		}
	}

	return true;
}


//读取多个IP地址段,其中<ip>是parentnode
//<ip>
//<ip_range>192.168.0.1-192.168.0.255</ip_range>
//<ip_range>192.168.1.1-192.168.1.255</ip_range>
//</ip>
BOOL CTinyXmlEx::GetValue_HostOrderIPRangeVector(TiXmlNode * parent_node,		
									 std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_ip_range,
									 const char * subitem)
{
	for( TiXmlNode * iprange_node = parent_node->FirstChildElement(subitem);
		iprange_node;
		iprange_node = iprange_node->NextSiblingElement(subitem) )
	{
		ACE_UINT32 begin_ip=0,end_ip=0;

		if( GetValue_HostOrderIPRange(iprange_node,begin_ip,end_ip) )
		{
			vt_ip_range.push_back(std::make_pair(begin_ip,end_ip));
		}
	}

	return true;
}

BOOL CTinyXmlEx::GetValue_MacVector(TiXmlNode * parent_node,		
						std::vector< ACE_UINT64 >& vt_mac,
						const char * subitem)
{
	for( TiXmlNode * mac_node = parent_node->FirstChildElement(subitem);
		mac_node;
		mac_node = mac_node->NextSiblingElement(subitem) )
	{
		ACE_UINT64 mac_addr = 0;

		if( GetValue_Macaddr(mac_node,(BYTE *)&mac_addr) )
		{
			vt_mac.push_back(mac_addr);
		}		
	}

	return true;
}

//读取多个整数范围地址段,其中<uint>是parentnode
//<uint>
//<uint_range>192.168.0.1-192.168.0.255</uint_range>
//<uint_range>192.168.1.1-192.168.1.255</uint_range>
//</uint>
BOOL CTinyXmlEx::GetValue_Uint32RangeVector(TiXmlNode * parent_node,		
								std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_uint_range,
								const char * subitem)
{
	for( TiXmlNode * int_node = parent_node->FirstChildElement(subitem);
		int_node;
		int_node = int_node->NextSiblingElement(subitem) )
	{
		ACE_UINT32 begin_data=0,end_data=0;

		if( GetValue_UintRange(int_node,begin_data,end_data) )
		{
			vt_uint_range.push_back(std::make_pair(begin_data,end_data));
		}
	}

	return true;
}

BOOL CTinyXmlEx::GetValue_Uint16RangeVector(TiXmlNode * parent_node,		
								std::vector< std::pair<ACE_UINT16,ACE_UINT16> >&	vt_uint_range,
								const char * subitem)
{
	for( TiXmlNode * int_node = parent_node->FirstChildElement(subitem);
		int_node;
		int_node = int_node->NextSiblingElement(subitem) )
	{
		ACE_UINT32 begin_data=0,end_data=0;

		if( GetValue_UintRange(int_node,begin_data,end_data) )
		{
			vt_uint_range.push_back(std::make_pair((ACE_UINT16)begin_data,(ACE_UINT16)end_data));
		}
	}

	return true;
}

BOOL CTinyXmlEx::GetValue_StringVector(TiXmlNode * parent_node,		
						   std::vector< std::string >&	vt_string,
						   const char * subitem)
{
	for(TiXmlNode * string_node = parent_node->FirstChildElement(subitem);
		string_node;
		string_node = string_node->NextSiblingElement(subitem))
	{
		std::string my_string;

		if( GetValue(string_node,my_string) )
		{
			vt_string.push_back(my_string);
		}
	}

	return true;
}

BOOL CTinyXmlEx::GetValue_Uint64Vector(TiXmlNode * parent_node,		
						   std::vector< ACE_UINT64 >&	vt_uint,
						   const char * subitem)
{
	for(TiXmlNode * uint_node = parent_node->FirstChildElement(subitem);
		uint_node;
		uint_node = uint_node->NextSiblingElement(subitem))
	{
		ACE_UINT64 data;

		if( GetValue(uint_node,data) )
		{
			vt_uint.push_back(data);
		}
	}

	return true;
}


BOOL CTinyXmlEx::GetValue_Uint32Vector(TiXmlNode * parent_node,		
						   std::vector< ACE_UINT32 >&	vt_uint,
						   const char * subitem)
{
	for(TiXmlNode * uint_node = parent_node->FirstChildElement(subitem);
		uint_node;
		uint_node = uint_node->NextSiblingElement(subitem))
	{
		ACE_UINT32 data;

		if( GetValue(uint_node,data) )
		{
			vt_uint.push_back(data);
		}
	}

	return true;
}

BOOL CTinyXmlEx::GetValue_Uint16Vector(TiXmlNode * parent_node,		
						   std::vector< ACE_UINT16 >&	vt_uint,
						   const char * subitem)
{
	for(TiXmlNode * uint_node = parent_node->FirstChildElement(subitem);
		uint_node;
		uint_node = uint_node->NextSiblingElement(subitem))
	{
		ACE_UINT32 data;

		if( GetValue(uint_node,data) )
		{
			vt_uint.push_back((ACE_UINT16)data);
		}
	}

	return true;
}


//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
BOOL CTinyXmlEx::GetValue_Macaddr(TiXmlNode *pNode, BYTE * mac_addr)
{
	std::string str_mac;

	if( !GetValue(pNode,str_mac) )
		return false;

	CPF::mac_a_to_addr(str_mac.c_str(),mac_addr);

	return true;
	
}

int CTinyXmlEx::GetValue(TiXmlNode *pNode,std::vector<std::string>& value,char sep)
{
	value.clear();

	STD_TString strValue;

	if( !GetValue(pNode,strValue) )
		return 0;

	CPF::GetWords<ACE_TCHAR,std::string>(strValue.c_str(),value,sep);

	return (int)value.size();
}

int CTinyXmlEx::GetValue(TiXmlNode *pNode,std::vector<int>& value,char sep)
{
	value.clear();

	STD_TString strValue;

	if( !GetValue(pNode,strValue) )
		return 0;

	int size = CPF::GetWordNums<ACE_TCHAR>(strValue.c_str(),strValue.size(),sep,true);

	if( size <= 0 )
		return 0;

	value.reserve(size);

	for( int i = 0; i < size; ++i )
	{
		int data = 0;

		if( CPF::GetIntWord<ACE_TCHAR>(i,strValue.c_str(),data,sep) )
			value.push_back(data);
	}

	return (int)value.size();
}

int CTinyXmlEx::GetValue(TiXmlNode *pNode,std::vector<unsigned int>& value,char sep)
{
	value.clear();

	STD_TString strValue;

	if( !GetValue(pNode,strValue) )
		return 0;

	int size = CPF::GetWordNums<ACE_TCHAR>(strValue.c_str(),strValue.size(),sep,true);

	if( size <= 0 )
		return 0;

	value.reserve(size);

	for( int i = 0; i < size; ++i )
	{
		unsigned int data = 0;

		if( CPF::GetUintWord<ACE_TCHAR>(i,strValue.c_str(),data,sep) )
			value.push_back(data);
	}

	return (int)value.size();
}

int CTinyXmlEx::GetValue(TiXmlNode *pNode,std::vector<ACE_UINT64>& value,char sep)
{
	value.clear();

	STD_TString strValue;

	if( !GetValue(pNode,strValue) )
		return 0;

	int size = CPF::GetWordNums<ACE_TCHAR>(strValue.c_str(),strValue.size(),sep,true);

	if( size <= 0 )
		return 0;

	value.reserve(size);

	for( int i = 0; i < size; ++i )
	{
		ACE_UINT64 data = 0;

		if( CPF::GetUint64Word<ACE_TCHAR>(i,strValue.c_str(),data,sep) )
			value.push_back(data);
	}

	return (int)value.size();
}


bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,std::string &Attrvalue)
{
	const char * result = pNode->ToElement()->Attribute(attrname);

	if( result )
	{
		Attrvalue = result;

		return true;
	}

	return false;
}


bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,int& value)
{
	std::string strValue;

	if( !GetAttr(pNode,attrname,strValue) )
		return false;

	value = atoi(strValue.c_str());

	return true;
}

bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,bool& value)
{
	int intvalue = 0;
	if( GetAttr(pNode,attrname,intvalue) )
	{
		value = (intvalue!=0);
		return true;
	}

	return false;
}

bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT32& value)
{
	ACE_UINT64 intvalue = 0;
	if( GetAttr(pNode,attrname,intvalue) )
	{
		value = (ACE_UINT32)intvalue;
		return true;
	}

	return false;
}


bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT16& value)
{
	ACE_UINT64 intvalue = 0;
	if( GetAttr(pNode,attrname,intvalue) )
	{
		value = (ACE_UINT16)intvalue;
		return true;
	}

	return false;
}

bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,double& value)
{
	std::string strValue;

	if( !GetAttr(pNode,attrname,strValue) )
		return false;

	value = atof(strValue.c_str());

	return true;
}

bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,float& value)
{
	double temp_value;

	if( GetAttr(pNode,attrname,temp_value) )
	{
		value = (float)temp_value;

		return true;
	}

	return false;
}

bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT64& value)
{
	std::string str_value;

	if( !GetAttr(pNode,attrname,str_value) )
	{
		return false;
	}
	
	value = SpecialString2Uint(str_value.c_str());

	return true;
}

bool CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,char * buf,size_t buflen)
{
	const char * result = pNode->ToElement()->Attribute(attrname);

	if( result )
	{
		strncpy(buf,result,buflen);
		buf[buflen-1]=0;

		return true;
	}

	return false;
}

BOOL CTinyXmlEx::GetAttr_UintRange(TiXmlNode *pNode,const char * attrname, unsigned int& value1,unsigned int& value2)
{
	std::string str_value;

	if( GetAttr(pNode,attrname,str_value) )
	{
		if( CPF::ParseUintRange(str_value.c_str(),str_value.size(),value1,value2) )
		{
			return true;
		}
	}

	return false;
}


BOOL CTinyXmlEx::GetAttr_HostOrderIP(TiXmlNode *pNode,const char * attrname, ACE_UINT32& value)
{
	std::string str_value;

	if( GetAttr(pNode,attrname,str_value) )
	{
		if (!str_value.empty())
		{
			value = ACE_NTOHL(inet_addr(str_value.c_str()));
		}
		else
		{
			value = 0;
		}

		return true;
	}

	return false;
}

BOOL CTinyXmlEx::GetAttr_HostOrderIPRange(TiXmlNode *pNode,const char * attrname, ACE_UINT32& begin_ip,ACE_UINT32& end_ip)
{
	std::string str_value;

	if( GetAttr(pNode,attrname,str_value) )
	{
		if (!str_value.empty())
		{
			if (CPF::ParseIPRange(str_value.c_str(), str_value.size(), begin_ip, end_ip))
			{
				begin_ip = ACE_NTOHL(begin_ip);
				end_ip = ACE_NTOHL(end_ip);

				return true;
			}
		}
	}

	return false;
}

//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
BOOL CTinyXmlEx::GetAttr_Macaddr(TiXmlNode *pNode,const char * attrname, BYTE * mac_addr)
{
	std::string str_value;

	if( GetAttr(pNode,attrname,str_value) )
	{
		CPF::mac_a_to_addr(str_value.c_str(),mac_addr);

		return true;
	}

	return false;
}

int CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<std::string>& value,char sep)
{
	std::string str_value;

	if( GetAttr(pNode,attrname,str_value) )
	{
		CPF::GetWords<ACE_TCHAR,std::string>(str_value.c_str(),value,sep);

		return (int)value.size();
	}

	return 0;
}

int CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<int>& value,char sep)
{
	std::string str_value;

	if( GetAttr(pNode,attrname,str_value) )
	{
		int size = CPF::GetWordNums<ACE_TCHAR>(str_value.c_str(),str_value.size(),sep,true);

		if( size <= 0 )
			return 0;

		value.reserve(size);

		for( int i = 0; i < size; ++i )
		{
			int data = 0;

			if( CPF::GetIntWord<ACE_TCHAR>(i,str_value.c_str(),data,sep) )
				value.push_back(data);
		}
	}

	return 0;
}

int CTinyXmlEx::GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<unsigned int>& value,char sep)
{
	std::string strValue;

	if( GetAttr(pNode,attrname,strValue) )
	{
		int size = CPF::GetWordNums<ACE_TCHAR>(strValue.c_str(),strValue.size(),sep,true);

		if( size <= 0 )
			return 0;

		value.reserve(size);

		for( int i = 0; i < size; ++i )
		{
			unsigned int data = 0;

			if( CPF::GetUintWord<ACE_TCHAR>(i,strValue.c_str(),data,sep) )
				value.push_back(data);
		}

		return (int)value.size();
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//Set
//////////////////////////////////////////////////////////////////////////
//读取XML文件节点内容的封装函数
bool CTinyXmlEx::SetValue(TiXmlNode *pNode,ACE_UINT16 ivalue)
{
	return SetValue(pNode,(ACE_UINT32)ivalue);
}

bool CTinyXmlEx::SetValue(TiXmlNode *pNode,ACE_UINT32 ivalue)
{
	char buf[32] = {0};
	sprintf(buf,"%u",ivalue);

	return SetValue(pNode,buf);
}


bool CTinyXmlEx::SetValue(TiXmlNode *pNode,ACE_UINT64 ivalue)
{
	char buf[64] = {0};

	sprintf(buf,"%llu",ivalue);

	return SetValue(pNode,buf);
}

bool CTinyXmlEx::SetValue(TiXmlNode *pNode,int ivalue)
{
	char buf[32] = {0};
	sprintf(buf,"%d",ivalue);

	return SetValue(pNode,buf);
}

bool CTinyXmlEx::SetValue(TiXmlNode *pNode,const std::string& value)
{
	return SetValue(pNode,value.c_str());
}

bool CTinyXmlEx::SetValue(TiXmlNode *pNode,double value)
{
	char buf[32] = {0};
	sprintf(buf,"%f",value);

	return SetValue(pNode,buf);
}

bool CTinyXmlEx::SetValue(TiXmlNode *pNode,float value)
{
	return SetValue(pNode,(double)value);
}

//buflen:字符个数的缓冲大小
bool CTinyXmlEx::SetValue(TiXmlNode *pNode,const char * buf)
{
	if( !pNode )
		return false;

	if( pNode->FirstChild() 
		&& pNode->FirstChild()->ToText() )
	{
		pNode->FirstChild()->ToText()->SetValue(buf);

		return true;
	}
	else
	{
		TiXmlText text(buf);

		pNode->InsertEndChild(text);

		return true;
	}

	return false;
}

//用于item="1-2"的整数范围
BOOL CTinyXmlEx::SetValue_UintRange(TiXmlNode *pNode, unsigned int value1,unsigned int value2)
{
	char buf[64] = {0};
	sprintf(buf,"%u-%u",value1,value2);

	return SetValue(pNode,buf);
}

//用于item=192.168.0.1的IP地址类型或者mask的获取
BOOL CTinyXmlEx::SetValue_HostOrderIP(TiXmlNode *pNode, ACE_UINT32 value)
{
	value = ACE_HTONL(value);

	return SetValue(pNode,inet_ntoa(*(in_addr *)&value));
}

//用于item="192.168.0.1-192.168.0.255"的IP地址范围
BOOL CTinyXmlEx::SetValue_HostOrderIPRange(TiXmlNode *pNode, ACE_UINT32 begin_ip,ACE_UINT32 end_ip)
{
	char buf1[32] = {0};
	char buf2[32] = {0};

	begin_ip = ACE_HTONL(begin_ip);
	end_ip = ACE_HTONL(end_ip);

	strcpy(buf1,inet_ntoa(*(in_addr *)&begin_ip));
	strcpy(buf2,inet_ntoa(*(in_addr *)&end_ip));

	char buf[64];
	sprintf(buf,"%s-%s",buf1,buf2);

	return SetValue(pNode,buf);
	
}

BOOL CTinyXmlEx::SetValue_HostOrderIPVector(TiXmlNode * parent_node,
								std::vector<ACE_UINT32>& vt_value,
								const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_value.size(); ++i)
	{
		TiXmlNode * ip_node = InsertEndChildElement(parent_node,subitem);

		if( ip_node )
		{
			SetValue_HostOrderIP(ip_node,vt_value[i]);
		}
	}

	return true;
}


//读取多个IP地址段,其中<ip>是parentnode
//<ip>
//<ip_range>192.168.0.1-192.168.0.255</ip_range>
//<ip_range>192.168.1.1-192.168.1.255</ip_range>
//</ip>
BOOL CTinyXmlEx::SetValue_HostOrderIPRangeVector(TiXmlNode * parent_node,		
									  const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_ip_range,
									  const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_ip_range.size(); ++i)
	{
		TiXmlNode * ip_node = InsertEndChildElement(parent_node,subitem);

		if( ip_node )
		{
			SetValue_HostOrderIPRange(ip_node,vt_ip_range[i].first,vt_ip_range[i].second);
		}
	}

	return true;
}

BOOL CTinyXmlEx::SetValue_MacVector(TiXmlNode * parent_node,		
						const std::vector< ACE_UINT64 >& vt_mac,
						const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_mac.size(); ++i)
	{
		TiXmlNode * mac_node = InsertEndChildElement(parent_node,subitem);

		if( mac_node )
		{
			ACE_UINT64 mac_addr = vt_mac[i];

			SetValue_Macaddr(mac_node,(const BYTE *)&mac_addr);
		}
	}

	return true;
}



//读取多个整数范围地址段,其中<uint>是parentnode
//<uint>
//<uint_range>192.168.0.1-192.168.0.255</uint_range>
//<uint_range>192.168.1.1-192.168.1.255</uint_range>
//</uint>
BOOL CTinyXmlEx::SetValue_Uint32RangeVector(TiXmlNode * parent_node,		
								const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_uint_range,
								const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_uint_range.size(); ++i)
	{
		TiXmlNode * ip_node = InsertEndChildElement(parent_node,subitem);

		if( ip_node )
		{
			SetValue_UintRange(ip_node,vt_uint_range[i].first,vt_uint_range[i].second);
		}
	}

	return true;
}

BOOL CTinyXmlEx::SetValue_Uint16RangeVector(TiXmlNode * parent_node,		
								const std::vector< std::pair<ACE_UINT16,ACE_UINT16> >&	vt_uint_range,
								const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_uint_range.size(); ++i)
	{
		TiXmlNode * ip_node = InsertEndChildElement(parent_node,subitem);

		if( ip_node )
		{
			SetValue_UintRange(ip_node,vt_uint_range[i].first,vt_uint_range[i].second);
		}
	}

	return true;
}

BOOL CTinyXmlEx::SetValue_StringVector(TiXmlNode * parent_node,		
						   const std::vector< std::string >&	vt_string,
						   const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_string.size(); ++i)
	{
		TiXmlNode * string_node = InsertEndChildElement(parent_node,subitem);

		if( string_node )
		{
			SetValue(string_node,vt_string[i]);
		}
	}

	return true;
}

BOOL CTinyXmlEx::SetValue_Uint64Vector(TiXmlNode * parent_node,		
						   const std::vector< ACE_UINT64 >&	vt_uint_range,
						   const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_uint_range.size(); ++i)
	{
		TiXmlNode * uint_node = InsertEndChildElement(parent_node,subitem);

		if( uint_node )
		{
			SetValue(uint_node,vt_uint_range[i]);
		}
	}

	return true;
}

BOOL CTinyXmlEx::SetValue_Uint32Vector(TiXmlNode * parent_node,		
						   const std::vector< ACE_UINT32 >&	vt_uint_range,
						   const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_uint_range.size(); ++i)
	{
		TiXmlNode * uint_node = InsertEndChildElement(parent_node,subitem);

		if( uint_node )
		{
			SetValue(uint_node,vt_uint_range[i]);
		}
	}

	return true;
}

BOOL CTinyXmlEx::SetValue_Uint16Vector(TiXmlNode * parent_node,		
						   const std::vector< ACE_UINT16 >&	vt_uint_range,
						   const char * subitem)
{
	RemoveAllChild(parent_node,subitem);

	for(size_t i = 0; i < vt_uint_range.size(); ++i)
	{
		TiXmlNode * uint_node = InsertEndChildElement(parent_node,subitem);

		if( uint_node )
		{
			SetValue(uint_node,vt_uint_range[i]);
		}
	}

	return true;
}

//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
BOOL CTinyXmlEx::SetValue_Macaddr(TiXmlNode *pNode, const BYTE * mac_addr)
{
	char buf[32];
	CPF::mac_addr_to_a(mac_addr,buf,'-');

	return SetValue(pNode,buf);
}

BOOL CTinyXmlEx::SetValue(TiXmlNode *pNode, const std::vector<std::string>& valueArray,char sep)
{
	std::string str_value = CPF::MakeUpStringSetString(valueArray,sep);

	return SetValue(pNode,str_value.c_str());
}

BOOL CTinyXmlEx::SetValue(TiXmlNode *pNode,const std::vector<int>& value,char sep)
{
	std::string str_value = CPF::MakeUpIntSetString(value,sep);

	return SetValue(pNode,str_value.c_str());
}

BOOL CTinyXmlEx::SetValue(TiXmlNode *pNode,const std::vector<unsigned int>& value,char sep)
{
	std::string str_value = CPF::MakeUpIntSetString(value,sep);

	return SetValue(pNode,str_value.c_str());
}

BOOL CTinyXmlEx::SetValue(TiXmlNode *pNode,const std::vector<ACE_UINT64>& value,char sep)
{
	std::string str_value = CPF::MakeUpIntSetString(value,sep);

	return SetValue(pNode,str_value.c_str());
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,const std::string &Attrvalue)
{
	return SetAttr(pNode,attrname,Attrvalue.c_str());

	return false;
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,int value)
{
	if( pNode && pNode->ToElement() )
	{
		pNode->ToElement()->SetAttribute(attrname,value);

		return true;
	}

	return false;
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT64 value)
{
	if( pNode && pNode->ToElement() )
	{
		char buf[64] = {0};
		sprintf(buf,"%llu",value);

		pNode->ToElement()->SetAttribute(attrname,buf);

		return true;
	}

	return false;
}


bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,bool value)
{
	return SetAttr(pNode,attrname,(int)value);

}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,unsigned int value)
{
	if( pNode && pNode->ToElement() )
	{
		char buf[32] = {0};
		sprintf(buf,"%u",value);

		pNode->ToElement()->SetAttribute(attrname,buf);

		return true;
	}

	return false;
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT16 value)
{
	return SetAttr(pNode,attrname,(int)value);
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,double value)
{
	if( pNode && pNode->ToElement() )
	{
		pNode->ToElement()->SetDoubleAttribute(attrname,value);

		return true;
	}

	return false;
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,float value)
{
	if( pNode && pNode->ToElement() )
	{
		pNode->ToElement()->SetDoubleAttribute(attrname,(double)value);

		return true;
	}

	return false;
}

bool CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,const char * buf)
{
	if( pNode && pNode->ToElement() )
	{
		pNode->ToElement()->SetAttribute(attrname,buf);

		return true;
	}

	return false;
}

BOOL CTinyXmlEx::SetAttr_UintRange(TiXmlNode *pNode,const char * attrname, unsigned int value1,unsigned int value2)
{
	if( pNode && pNode->ToElement() )
	{
		char buf[64] = {0};
		sprintf(buf,"%u-%u",value1,value2);

		return SetAttr(pNode,attrname,buf);

		return true;
	}

	return false;
}
//用于item=192.168.0.1的IP地址类型或者mask的获取
BOOL CTinyXmlEx::SetAttr_HostOrderIP(TiXmlNode *pNode,const char * attrname, ACE_UINT32 value)
{
	value = ACE_HTONL(value);

	return SetAttr(pNode,attrname,inet_ntoa(*(in_addr *)&value));

}

//用于item="192.168.0.1-192.168.0.255"的IP地址范围
BOOL CTinyXmlEx::SetAttr_HostOrderIPRange(TiXmlNode *pNode,const char * attrname, ACE_UINT32 begin_ip,ACE_UINT32 end_ip)
{
	char buf1[32] = {0};
	char buf2[32] = {0};

	begin_ip = ACE_HTONL(begin_ip);
	end_ip = ACE_HTONL(end_ip);

	strcpy(buf1,inet_ntoa(*(in_addr *)&begin_ip));
	strcpy(buf2,inet_ntoa(*(in_addr *)&end_ip));

	char buf[64];
	sprintf(buf,"%s-%s",buf1,buf2);

	return SetAttr(pNode,attrname,buf);
}

//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
BOOL CTinyXmlEx::SetAttr_Macaddr(TiXmlNode *pNode,const char * attrname, const BYTE * mac_addr)
{
	char buf[32];
	CPF::mac_addr_to_a(mac_addr,buf,'-');

	return SetAttr(pNode,attrname,buf);
}


BOOL CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname, const std::vector<std::string>& valueArray,char sep)
{
	std::string str_value = CPF::MakeUpStringSetString(valueArray,sep);

	return SetAttr(pNode,attrname,str_value.c_str());
}

BOOL CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,const std::vector<int>& value,char sep)
{
	std::string str_value = CPF::MakeUpIntSetString(value,sep);

	return SetAttr(pNode,attrname,str_value.c_str());
}

BOOL CTinyXmlEx::SetAttr(TiXmlNode *pNode,const char * attrname,const std::vector<unsigned int>& value,char sep)
{
	std::string str_value = CPF::MakeUpIntSetString(value,sep);

	return SetAttr(pNode,attrname,str_value.c_str());
}


static FILE* TiXmlFOpen( const char* filename, const char* mode )
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
	FILE* fp = 0;
	errno_t err = fopen_s( &fp, filename, mode );
	if ( !err && fp )
		return fp;
	return 0;
#else
	return fopen( filename, mode );
#endif
}

//返回只-1表示文件没有打开
//返回其他表示缓存区的大小，buf这个时候有效，用户应该使用完成后，delete []buf.
int CTinyXmlEx::ReadOneXmlFileForSend(const char * filename,char *& buf)
{
	FILE* file = TiXmlFOpen(filename, "rb" );	

	if( !file )
	{
		buf = NULL;
		return -1;
	}

	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	buf = new char[ 4+length+1 ];

	char* text = buf+4;

	if( length > 0 )
	{
		if ( fread( text, length, 1, file ) != 1 ) 
		{
			fclose(file);
			delete [] buf;
			buf = NULL;
			return -1;
		}
	}

	fclose(file);

	text[length] = 0;

	*(int *)buf = (length+1);

	return 4+length+1;

}



BOOL CTinyXmlEx::PacketOneFileIntoOneNode(TiXmlNode * node, const char * short_filename, const char * path)
{
	char full_name[MAX_PATH] = { 0 };

	CPF::JoinPathToPath(full_name, sizeof(full_name), path, short_filename);

	FILE * myfile = fopen(full_name, "rb");

	if (!myfile)
		return false;

	//先得到文件大小
	fseek(myfile, 0, SEEK_END);

	int file_length = ftell(myfile);

	char * file_data_buf = new char[file_length];

	//再读出文件内容
	fseek(myfile, 0, SEEK_SET);

	fread(file_data_buf, 1, file_length, myfile);

	fclose(myfile);

	//编码
	char * encode_pdata_buf = new char[(file_length + 1) * 3];

	CURLDecoder::encode(file_data_buf, file_length, encode_pdata_buf);

	delete[]file_data_buf;

	//再写入xml
	SetAttr(node, "file_name", short_filename);

	SetValue(node, encode_pdata_buf);

	delete[]encode_pdata_buf;

	return true;
}

BOOL CTinyXmlEx::UnPacketOneFileFomeOneNode(TiXmlNode * node, const char * pPath,std::string * out_file_name)
{
	std::string str_file_name;

	GetAttr(node, "file_name", str_file_name);

	if (out_file_name)
	{
		*out_file_name = str_file_name;
	}

	char full_file_name[MAX_PATH] = { 0 };

	CPF::JoinPathToPath(full_file_name, sizeof(full_file_name), pPath, str_file_name.c_str());

	std::string str_value;
	GetValue(node, str_value);

	CURLDecoder::unencode(str_value);

	FILE * myfile = fopen(full_file_name, "wb");

	if (!myfile)
		return false;

	fwrite(str_value.c_str(), 1, str_value.length(), myfile);

	fclose(myfile);

	return true;
}

BOOL CTinyXmlEx::PacketOneFolderIntoOneNode(TiXmlNode * node, const char * path)
{
	//扫描源目录的所有文件
	ACE_DIRENT **namelist = NULL;

	int nEntityNum = ACE_OS::scandir(
		path,
		&namelist,
		NULL,
		NULL);

	//如果目录为空，返回
	if (nEntityNum <= 0)
	{
		return false;
	}

	for (int nFileIndex = 0; nFileIndex < nEntityNum; nFileIndex++)
	{
		if (strcmp(namelist[nFileIndex]->d_name, ".") == 0)
			continue;
		if (strcmp(namelist[nFileIndex]->d_name, "..") == 0)
			continue;

		{
			char full_name[MAX_PATH] = { 0 };
			CPF::JoinPathToPath(full_name, sizeof(full_name), path, namelist[nFileIndex]->d_name);

			if (CPF::IsDirectory(full_name))
			{
				TiXmlNode * sub_folder_node = InsertEndChildElement(node, "one_folder");

				if (sub_folder_node)
				{
					SetAttr(sub_folder_node, "folder_name", namelist[nFileIndex]->d_name);

					PacketOneFolderIntoOneNode(sub_folder_node, full_name);
				}

				continue;
			}
		}

		{
			const char * short_filename = namelist[nFileIndex]->d_name;

			TiXmlNode * one_file_node = InsertEndChildElement(node, "one_file");

			if (one_file_node)
			{
				if (!PacketOneFileIntoOneNode(one_file_node, short_filename, path))
				{
					return false;
				}
			}
		}		
	}

	//释放空间
	CPF::freedirent(namelist, nEntityNum);

	return true;
}

BOOL CTinyXmlEx::UnPacketOneFolderFromOneNode(TiXmlNode * pParentNode, const char * pPath)
{
	CPF::CreateFullDirecory(pPath);

	for (TiXmlNode * one_file_node = FirstChildElement(pParentNode, "one_file");
		one_file_node;
		one_file_node = one_file_node->NextSiblingElement("one_file"))
	{
		if (!UnPacketOneFileFomeOneNode(one_file_node, pPath,NULL))
		{
			return false;
		}
	}

	for (TiXmlNode * one_folder_node = FirstChildElement(pParentNode, "one_folder");
		one_folder_node;
		one_folder_node = one_folder_node->NextSiblingElement("one_folder"))
	{
		std::string str_sub_folder_name;

		GetAttr(one_folder_node, "folder_name", str_sub_folder_name);

		char full_path[MAX_PATH] = { 0 };

		CPF::JoinPathToPath(full_path,sizeof(full_path),pPath, str_sub_folder_name.c_str());

		if (!UnPacketOneFolderFromOneNode(one_folder_node, full_path))
		{
			return false;
		}
	}

	return true;
}
