//////////////////////////////////////////////////////////////////////////
//TinyXmlEx.h

#pragma once

#include "cpf/tinyxml.h"

class CPF_CLASS CTinyXmlEx
{
public:
	typedef std::vector<TiXmlNode* > NodeList;

public:
	CTinyXmlEx();
	virtual ~CTinyXmlEx();

public:
	//打开一个已经存在的文件。如果文件不存在返回-1
	int open(const ACE_TCHAR * ConfigFilename);//打开配置文件 0:成功；-1：失败

	//必须以0为结尾,length表示字符的个数，因此整个buf的长度应该不小于length+1
	int open(const char * buf,int length);

	void close();

	// imode=0,文件不存在的时候创建新文件，存在则打开
	// imode=1，文件存在也清除内容，重新构建
	enum OPEN_MODE {
		OPEN_MODE_OPEN_CREATE = 0,
		OPEN_MODE_CREATE_TRUNC = 1,
	};
	 
	int open_ex(const char *pszXMLFullName, OPEN_MODE enMode=OPEN_MODE_OPEN_CREATE);

	//打开，并且看是否有root节点，如果没有root节点返回-1
	int open_for_readonly(const char *pszXMLFullName)
	{
		int nrtn = open(pszXMLFullName);

		if( nrtn == 0 )
		{
			if( GetRootNode() != NULL )
				return 0;
			else
				return -1;
		}

		return nrtn;
	}

	bool savefile() const;
	bool savefile( const char * filename ) const;

public:
	TiXmlNode * GetRootNode()
	{
		return m_XmlDoc.FirstChildElement();
	}

	//查找指定节点的所有子节点
	BOOL GetChildElementNodes(TiXmlNode& ParentNode,NodeList &ChildNodes);

public:
	//读取XML文件节点内容的封装函数
	bool GetValue(TiXmlNode *pNode,ACE_UINT16 &ivalue);//读取XML的值--ACE_UINT16
	bool GetValue(TiXmlNode *pNode,ACE_UINT32 &ivalue);//读取XML的值--ACE_UINT32
	bool GetValue(TiXmlNode *pNode,ACE_UINT64 &ivalue);//读取XML的值--ACE_UINT64
	bool GetValue(TiXmlNode *pNode,int &ivalue);//读取XML的值--int
	bool GetValue(TiXmlNode *pNode,std::string& value);//读取XML的值
	bool GetValue(TiXmlNode *pNode,double& value);//读取XML的值
	bool GetValue(TiXmlNode *pNode,float& value);//读取XML的值

	//buflen:字符个数的缓冲大小
	bool GetValue(TiXmlNode *pNode,char * buf,size_t buflen);

	//用于item="1-2"的整数范围
	BOOL GetValue_UintRange(TiXmlNode *pNode, unsigned int& value1,unsigned int& value2);

	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL GetValue_HostOrderIP(TiXmlNode *pNode, ACE_UINT32& value);

	//读取多个IP地址,其中<ips>是parentnode,subitem="single_ip"
	//<ips>
	//<single_ip>192.168.0.1</single_ip>
	//<single_ip>192.168.1.1</single_ip>
	//</ips>
	BOOL GetValue_HostOrderIPVector(TiXmlNode * pParentnode,
		std::vector<ACE_UINT32>& vt_value,
		const char * subitem);

	//用于item="192.168.0.1-192.168.0.255"的IP地址范围
	BOOL GetValue_HostOrderIPRange(TiXmlNode *pNode, ACE_UINT32& begin_ip,ACE_UINT32& end_ip);

	//读取多个IP地址段,其中<ip>是parentnode,subitem="ip_range"
	//<ip>
	//<ip_range>192.168.0.1-192.168.0.255</ip_range>
	//<ip_range>192.168.1.1-192.168.1.255</ip_range>
	//</ip>
	BOOL GetValue_HostOrderIPRangeVector(TiXmlNode * pParentnode,		
		std::vector< std::pair<ACE_UINT32,ACE_UINT32> >& vt_ip_range,
		const char * subitem);

	BOOL GetValue_MacVector(TiXmlNode * pParentnode,		
		std::vector< ACE_UINT64 >& vt_mac,
		const char * subitem);


	//读取多个整数范围地址段,其中<uint>是parentnode
	//<uint>
	//<uint_range>192.168.0.1-192.168.0.255</uint_range>
	//<uint_range>192.168.1.1-192.168.1.255</uint_range>
	//</uint>
	BOOL GetValue_Uint32RangeVector(TiXmlNode * pParentnode,		
		std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_uint_range,
		const char * subitem);

	BOOL GetValue_Uint16RangeVector(TiXmlNode * pParentnode,		
		std::vector< std::pair<ACE_UINT16,ACE_UINT16> >&	vt_uint_range,
		const char * subitem);

	BOOL GetValue_StringVector(TiXmlNode * pParentnode,		
		std::vector< std::string >&	vt_string,
		const char * subitem);

	BOOL GetValue_Uint64Vector(TiXmlNode * pParentnode,		
		std::vector< ACE_UINT64 >&	vt_uint,
		const char * subitem);

	BOOL GetValue_Uint32Vector(TiXmlNode * pParentnode,		
		std::vector< ACE_UINT32 >&	vt_uint,
		const char * subitem);

	BOOL GetValue_Uint16Vector(TiXmlNode * pParentnode,		
		std::vector< ACE_UINT16 >&	vt_uint,
		const char * subitem);

	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL GetValue_Macaddr(TiXmlNode *pNode, BYTE * mac_addr);

	int GetValue(TiXmlNode *pNode,std::vector<std::string>& value,char sep=',');

	int GetValue(TiXmlNode *pNode,std::vector<int>& value,char sep=',');
	int GetValue(TiXmlNode *pNode,std::vector<unsigned int>& value,char sep=',');
	int GetValue(TiXmlNode *pNode,std::vector<ACE_UINT64>& value,char sep=',');


public:
	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,ACE_UINT16 &ivalue)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,ivalue);
	}

	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,ACE_UINT32 &ivalue)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,ivalue);
	}

	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,int &ivalue)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,ivalue);
	}

	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,ACE_UINT64 &ivalue)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,ivalue);
	}

	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,std::string& value)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value);
	}

	//buflen:字符个数的缓冲大小
	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,char * buf,size_t buflen)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,buf,buflen);
	}

	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,double& value)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value);
	}

	bool GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,float& value)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value);
	}

	BOOL GetValueLikeIni_UintRange(TiXmlNode *pParentNode, const char * childname, unsigned int& value1,unsigned int& value2)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue_UintRange(tempNode,value1,value2);
	}

	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL GetValueLikeIni_HostOrderIP(TiXmlNode *pParentNode,const char * childname, ACE_UINT32& value)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue_HostOrderIP(tempNode,value);
	}

	//用于item="192.168.0.1-192.168.0.255"的IP地址范围
	BOOL GetValueLikeIni_HostOrderIPRange(TiXmlNode *pParentNode,const char * childname, ACE_UINT32& begin_ip,ACE_UINT32& end_ip)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue_HostOrderIPRange(tempNode,begin_ip,end_ip);
	}
	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL GetValueLikeIni_Macaddr(TiXmlNode *pParentNode,const char * childname, BYTE * mac_addr)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue_Macaddr(tempNode,mac_addr);
	}

	int GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,std::vector<std::string>& value,char sep=',')
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value,sep);

	}

	int GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,std::vector<int>& value,char sep=',')
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value,sep);

	}

	int GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,std::vector<unsigned int>& value,char sep=',')
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value,sep);
	}

	int GetValueLikeIni(TiXmlNode *pParentNode,const char * childname,std::vector<ACE_UINT64>& value,char sep=',')
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue(tempNode,value,sep);
	}

public:
	bool GetAttr(TiXmlNode *pNode,const char * attrname,std::string &Attrvalue);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,int& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,bool& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,unsigned int& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT16& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT64& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,double& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,float& value);//读取指定节点的指定的属性值

	bool GetAttr(TiXmlNode *pNode,const char * attrname,char * buf,size_t buflen);

	BOOL GetAttr_UintRange(TiXmlNode *pParentNode,const char * attrname, unsigned int& value1,unsigned int& value2);
	
	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL GetAttr_HostOrderIP(TiXmlNode *pNode,const char * attrname, ACE_UINT32& value);

	//用于item="192.168.0.1-192.168.0.255"的IP地址范围
	BOOL GetAttr_HostOrderIPRange(TiXmlNode *pNode,const char * attrname, ACE_UINT32& begin_ip,ACE_UINT32& end_ip);

	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL GetAttr_Macaddr(TiXmlNode *pNode,const char * attrname, BYTE * mac_addr);

	int GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<std::string>& value,char sep=',');
	int GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<int>& value,char sep=',');
	int GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<unsigned int>& value,char sep=',');

public:
	//读取XML文件节点内容的封装函数
	bool SetValue(TiXmlNode *pNode,ACE_UINT16 ivalue);//读取XML的值--ACE_UINT16
	bool SetValue(TiXmlNode *pNode,ACE_UINT32 ivalue);//读取XML的值--ACE_UINT32
	bool SetValue(TiXmlNode *pNode,ACE_UINT64 ivalue);//读取XML的值--ACE_UINT64
	bool SetValue(TiXmlNode *pNode,int ivalue);//读取XML的值--int
	bool SetValue(TiXmlNode *pNode,const std::string& value);//读取XML的值
	bool SetValue(TiXmlNode *pNode,double value);//读取XML的值
	bool SetValue(TiXmlNode *pNode,float value);//读取XML的值

	bool SetValue(TiXmlNode *pNode,const char * buf);

	//用于item="1-2"的整数范围
	BOOL SetValue_UintRange(TiXmlNode *pNode, unsigned int value1,unsigned int value2);

	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL SetValue_HostOrderIP(TiXmlNode *pNode, ACE_UINT32 value);

	//设置多个IP地址,其中<ips>是parentnode,subitem="single_ip"
	//<ips>
	//<single_ip>192.168.0.1</single_ip>
	//<single_ip>192.168.1.1</single_ip>
	//</ips>
	BOOL SetValue_HostOrderIPVector(TiXmlNode * pParentnode,
		std::vector<ACE_UINT32>& vt_value,
		const char * subitem);

	//用于item="192.168.0.1-192.168.0.255"的IP地址范围
	BOOL SetValue_HostOrderIPRange(TiXmlNode *pNode, ACE_UINT32 begin_ip,ACE_UINT32 end_ip);

	//读取多个IP地址段,其中<ip>是parentnode,subitem="ip_range"
	//<ip>
	//<ip_range>192.168.0.1-192.168.0.255</ip_range>
	//<ip_range>192.168.1.1-192.168.1.255</ip_range>
	//</ip>
	BOOL SetValue_HostOrderIPRangeVector(TiXmlNode * pParentnode,		
		const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_ip_range,
		const char * subitem);

	BOOL SetValue_MacVector(TiXmlNode * pParentnode,		
		const std::vector< ACE_UINT64 >& vt_mac,
		const char * subitem);

	//读取多个整数范围地址段,其中<uint>是parentnode
	//<uint>
	//<uint_range>192.168.0.1-192.168.0.255</uint_range>
	//<uint_range>192.168.1.1-192.168.1.255</uint_range>
	//</uint>
	BOOL SetValue_Uint32RangeVector(TiXmlNode * pParentnode,		
		const std::vector< std::pair<ACE_UINT32,ACE_UINT32> >&	vt_uint_range,
		const char * subitem);
	BOOL SetValue_Uint16RangeVector(TiXmlNode * pParentnode,		
		const std::vector< std::pair<ACE_UINT16,ACE_UINT16> >&	vt_uint_range,
		const char * subitem);

	BOOL SetValue_StringVector(TiXmlNode * pParentnode,		
		const std::vector< std::string >&	vt_string,
		const char * subitem);

	BOOL SetValue_Uint64Vector(TiXmlNode * pParentnode,		
		const std::vector< ACE_UINT64 >&	vt_uint,
		const char * subitem);

	BOOL SetValue_Uint32Vector(TiXmlNode * pParentnode,		
		const std::vector< ACE_UINT32 >&	vt_uint,
		const char * subitem);

	BOOL SetValue_Uint16Vector(TiXmlNode * pParentnode,		
		const std::vector< ACE_UINT16 >&	vt_uint,
		const char * subitem);

	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL SetValue_Macaddr(TiXmlNode *pNode, const BYTE * mac_addr);

	BOOL SetValue(TiXmlNode *pNode, const std::vector<std::string>& valueArray,char sep=',');

	BOOL SetValue(TiXmlNode *pNode,const std::vector<int>& value,char sep=',');
	BOOL SetValue(TiXmlNode *pNode,const std::vector<unsigned int>& value,char sep=',');

	BOOL SetValue(TiXmlNode *pNode,const std::vector<ACE_UINT64>& value,char sep=',');

public:
	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,ACE_UINT16 ivalue)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,ivalue);
	}

	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,ACE_UINT32 ivalue)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,ivalue);
	}

	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,int ivalue)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,ivalue);
	}

	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,ACE_UINT64 ivalue)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,ivalue);
	}

	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,const std::string& value)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,value);
	}

	//buflen:字符个数的缓冲大小
	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,const char * buf)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,buf);
	}

	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,double value)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,value);
	}

	bool SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,float value)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,value);
	}

	BOOL SetValueLikeIni_UintRange(TiXmlNode *pParentNode, const char * childname, unsigned int value1,unsigned int value2)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue_UintRange(tempNode,value1,value2);
	}

	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL SetValueLikeIni_HostOrderIP(TiXmlNode *pParentNode,const char * childname, ACE_UINT32 value)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue_HostOrderIP(tempNode,value);
	}

	//用于item="192.168.0.1-192.168.0.255"的IP地址范围
	BOOL SetValueLikeIni_HostOrderIPRange(TiXmlNode *pParentNode,const char * childname, ACE_UINT32 begin_ip,ACE_UINT32 end_ip)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue_HostOrderIPRange(tempNode,begin_ip,end_ip);
	}
	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL SetValueLikeIni_Macaddr(TiXmlNode *pParentNode,const char * childname,const BYTE * mac_addr)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue_Macaddr(tempNode,mac_addr);
	}

	BOOL SetValueLikeIni(TiXmlNode *pParentNode,const char * childname, const std::vector<std::string>& valueArray,char sep=',')
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,valueArray,sep);
	}


	BOOL SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,const std::vector<int>& value,char sep=',')
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,value,sep);
	}

	BOOL SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,const std::vector<unsigned int>& value,char sep=',')
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,value,sep);
	}

	BOOL SetValueLikeIni(TiXmlNode *pParentNode,const char * childname,const std::vector<ACE_UINT64>& value,char sep=',')
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue(tempNode,value,sep);
	}

public:
	bool SetAttr(TiXmlNode *pNode,const char * attrname,const std::string &Attrvalue);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,int value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,bool value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,unsigned int value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT16 value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT64 value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,double value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,float value);//读取指定节点的指定的属性值

	bool SetAttr(TiXmlNode *pNode,const char * attrname,const char * buf);

	BOOL SetAttr_UintRange(TiXmlNode *pNode,const char * attrname, unsigned int value1,unsigned int value2);

	//用于item=192.168.0.1的IP地址类型或者mask的获取
	BOOL SetAttr_HostOrderIP(TiXmlNode *pNode,const char * attrname, ACE_UINT32 value);

	//用于item="192.168.0.1-192.168.0.255"的IP地址范围
	BOOL SetAttr_HostOrderIPRange(TiXmlNode *pNode,const char * attrname, ACE_UINT32 begin_ip,ACE_UINT32 end_ip);

	//用于item=01-02-03-04-05-06或者01:02:03:04:05:06的获取
	BOOL SetAttr_Macaddr(TiXmlNode *pNode,const char * attrname, const BYTE * mac_addr);

	BOOL SetAttr(TiXmlNode *pNode,const char * attrname, const std::vector<std::string>& valueArray,char sep=',');

	BOOL SetAttr(TiXmlNode *pNode,const char * attrname,const std::vector<int>& value,char sep=',');
	BOOL SetAttr(TiXmlNode *pNode,const char * attrname,const std::vector<unsigned int>& value,char sep=',');

public:
	//获取第一个名字为childname的element,如果没有则创建一个
	TiXmlNode * GetOrCreateFirstChildElement(TiXmlNode *pParentNode,const char * childname,BOOL * pbCreate=NULL);
	TiXmlNode * GetOrCreateRootElement(const char * root_name,BOOL * pbCreate=NULL);

public:
	TiXmlNode * InsertEndChildElement(TiXmlNode *pParentNode,const char * name);
	TiXmlNode * InsertRootElement(const char * name);

public:
	TiXmlNode *FirstChildElement(TiXmlNode *pNode,const char * _value ) const;
	TiXmlNode *NextSiblingElement(TiXmlNode *pNode,const char * _value ) const;

public:
	//删除pParentNode下面，所有的名字为subitem的节点
	void RemoveAllChild(TiXmlNode *pParentNode,const char * subitem);

public:
	//返回只-1表示文件没有打开
	//返回其他表示缓存区的大小，buf这个时候有效，用户应该使用完成后，delete []buf.
	//返回buf的结构：前4个自己表示文件内容的len+1.后面是整个文件的内容。最后一个是0。
	static int ReadOneXmlFileForSend(const char * filename,char *& buf);

public:
	BOOL UnPacketOneFileFomeOneNode(TiXmlNode * node,const char * pPath, std::string * out_file_name);
	BOOL PacketOneFileIntoOneNode(TiXmlNode * node, const char * short_filename, const char * path);

	//将这个路径以下的所有文件都放在一个节点（包括子节点内），该路径本身没有放入进去
	//路径的机构就是xml的树状结构
	BOOL PacketOneFolderIntoOneNode(TiXmlNode * node,const char * path);
	BOOL UnPacketOneFolderFromOneNode(TiXmlNode * node, const char * pPath);

public:
	TiXmlDocument& GetDoc()
	{
		return m_XmlDoc;
	}

	const TiXmlDocument& GetDoc() const
	{
		return m_XmlDoc;
	}

private:
	TiXmlDocument	m_XmlDoc;


};
