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
	//��һ���Ѿ����ڵ��ļ�������ļ������ڷ���-1
	int open(const ACE_TCHAR * ConfigFilename);//�������ļ� 0:�ɹ���-1��ʧ��

	//������0Ϊ��β,length��ʾ�ַ��ĸ������������buf�ĳ���Ӧ�ò�С��length+1
	int open(const char * buf,int length);

	void close();

	// imode=0,�ļ������ڵ�ʱ�򴴽����ļ����������
	// imode=1���ļ�����Ҳ������ݣ����¹���
	enum OPEN_MODE {
		OPEN_MODE_OPEN_CREATE = 0,
		OPEN_MODE_CREATE_TRUNC = 1,
	};
	 
	int open_ex(const char *pszXMLFullName, OPEN_MODE enMode=OPEN_MODE_OPEN_CREATE);

	//�򿪣����ҿ��Ƿ���root�ڵ㣬���û��root�ڵ㷵��-1
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

	//����ָ���ڵ�������ӽڵ�
	BOOL GetChildElementNodes(TiXmlNode& ParentNode,NodeList &ChildNodes);

public:
	//��ȡXML�ļ��ڵ����ݵķ�װ����
	bool GetValue(TiXmlNode *pNode,ACE_UINT16 &ivalue);//��ȡXML��ֵ--ACE_UINT16
	bool GetValue(TiXmlNode *pNode,ACE_UINT32 &ivalue);//��ȡXML��ֵ--ACE_UINT32
	bool GetValue(TiXmlNode *pNode,ACE_UINT64 &ivalue);//��ȡXML��ֵ--ACE_UINT64
	bool GetValue(TiXmlNode *pNode,int &ivalue);//��ȡXML��ֵ--int
	bool GetValue(TiXmlNode *pNode,std::string& value);//��ȡXML��ֵ
	bool GetValue(TiXmlNode *pNode,double& value);//��ȡXML��ֵ
	bool GetValue(TiXmlNode *pNode,float& value);//��ȡXML��ֵ

	//buflen:�ַ������Ļ����С
	bool GetValue(TiXmlNode *pNode,char * buf,size_t buflen);

	//����item="1-2"��������Χ
	BOOL GetValue_UintRange(TiXmlNode *pNode, unsigned int& value1,unsigned int& value2);

	//����item=192.168.0.1��IP��ַ���ͻ���mask�Ļ�ȡ
	BOOL GetValue_HostOrderIP(TiXmlNode *pNode, ACE_UINT32& value);

	//��ȡ���IP��ַ,����<ips>��parentnode,subitem="single_ip"
	//<ips>
	//<single_ip>192.168.0.1</single_ip>
	//<single_ip>192.168.1.1</single_ip>
	//</ips>
	BOOL GetValue_HostOrderIPVector(TiXmlNode * pParentnode,
		std::vector<ACE_UINT32>& vt_value,
		const char * subitem);

	//����item="192.168.0.1-192.168.0.255"��IP��ַ��Χ
	BOOL GetValue_HostOrderIPRange(TiXmlNode *pNode, ACE_UINT32& begin_ip,ACE_UINT32& end_ip);

	//��ȡ���IP��ַ��,����<ip>��parentnode,subitem="ip_range"
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


	//��ȡ���������Χ��ַ��,����<uint>��parentnode
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

	//����item=01-02-03-04-05-06����01:02:03:04:05:06�Ļ�ȡ
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

	//buflen:�ַ������Ļ����С
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

	//����item=192.168.0.1��IP��ַ���ͻ���mask�Ļ�ȡ
	BOOL GetValueLikeIni_HostOrderIP(TiXmlNode *pParentNode,const char * childname, ACE_UINT32& value)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue_HostOrderIP(tempNode,value);
	}

	//����item="192.168.0.1-192.168.0.255"��IP��ַ��Χ
	BOOL GetValueLikeIni_HostOrderIPRange(TiXmlNode *pParentNode,const char * childname, ACE_UINT32& begin_ip,ACE_UINT32& end_ip)
	{
		TiXmlNode * tempNode = FirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return GetValue_HostOrderIPRange(tempNode,begin_ip,end_ip);
	}
	//����item=01-02-03-04-05-06����01:02:03:04:05:06�Ļ�ȡ
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
	bool GetAttr(TiXmlNode *pNode,const char * attrname,std::string &Attrvalue);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,int& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,bool& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,unsigned int& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT16& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT64& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,double& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,float& value);//��ȡָ���ڵ��ָ��������ֵ

	bool GetAttr(TiXmlNode *pNode,const char * attrname,char * buf,size_t buflen);

	BOOL GetAttr_UintRange(TiXmlNode *pParentNode,const char * attrname, unsigned int& value1,unsigned int& value2);
	
	//����item=192.168.0.1��IP��ַ���ͻ���mask�Ļ�ȡ
	BOOL GetAttr_HostOrderIP(TiXmlNode *pNode,const char * attrname, ACE_UINT32& value);

	//����item="192.168.0.1-192.168.0.255"��IP��ַ��Χ
	BOOL GetAttr_HostOrderIPRange(TiXmlNode *pNode,const char * attrname, ACE_UINT32& begin_ip,ACE_UINT32& end_ip);

	//����item=01-02-03-04-05-06����01:02:03:04:05:06�Ļ�ȡ
	BOOL GetAttr_Macaddr(TiXmlNode *pNode,const char * attrname, BYTE * mac_addr);

	int GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<std::string>& value,char sep=',');
	int GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<int>& value,char sep=',');
	int GetAttr(TiXmlNode *pNode,const char * attrname,std::vector<unsigned int>& value,char sep=',');

public:
	//��ȡXML�ļ��ڵ����ݵķ�װ����
	bool SetValue(TiXmlNode *pNode,ACE_UINT16 ivalue);//��ȡXML��ֵ--ACE_UINT16
	bool SetValue(TiXmlNode *pNode,ACE_UINT32 ivalue);//��ȡXML��ֵ--ACE_UINT32
	bool SetValue(TiXmlNode *pNode,ACE_UINT64 ivalue);//��ȡXML��ֵ--ACE_UINT64
	bool SetValue(TiXmlNode *pNode,int ivalue);//��ȡXML��ֵ--int
	bool SetValue(TiXmlNode *pNode,const std::string& value);//��ȡXML��ֵ
	bool SetValue(TiXmlNode *pNode,double value);//��ȡXML��ֵ
	bool SetValue(TiXmlNode *pNode,float value);//��ȡXML��ֵ

	bool SetValue(TiXmlNode *pNode,const char * buf);

	//����item="1-2"��������Χ
	BOOL SetValue_UintRange(TiXmlNode *pNode, unsigned int value1,unsigned int value2);

	//����item=192.168.0.1��IP��ַ���ͻ���mask�Ļ�ȡ
	BOOL SetValue_HostOrderIP(TiXmlNode *pNode, ACE_UINT32 value);

	//���ö��IP��ַ,����<ips>��parentnode,subitem="single_ip"
	//<ips>
	//<single_ip>192.168.0.1</single_ip>
	//<single_ip>192.168.1.1</single_ip>
	//</ips>
	BOOL SetValue_HostOrderIPVector(TiXmlNode * pParentnode,
		std::vector<ACE_UINT32>& vt_value,
		const char * subitem);

	//����item="192.168.0.1-192.168.0.255"��IP��ַ��Χ
	BOOL SetValue_HostOrderIPRange(TiXmlNode *pNode, ACE_UINT32 begin_ip,ACE_UINT32 end_ip);

	//��ȡ���IP��ַ��,����<ip>��parentnode,subitem="ip_range"
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

	//��ȡ���������Χ��ַ��,����<uint>��parentnode
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

	//����item=01-02-03-04-05-06����01:02:03:04:05:06�Ļ�ȡ
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

	//buflen:�ַ������Ļ����С
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

	//����item=192.168.0.1��IP��ַ���ͻ���mask�Ļ�ȡ
	BOOL SetValueLikeIni_HostOrderIP(TiXmlNode *pParentNode,const char * childname, ACE_UINT32 value)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue_HostOrderIP(tempNode,value);
	}

	//����item="192.168.0.1-192.168.0.255"��IP��ַ��Χ
	BOOL SetValueLikeIni_HostOrderIPRange(TiXmlNode *pParentNode,const char * childname, ACE_UINT32 begin_ip,ACE_UINT32 end_ip)
	{
		TiXmlNode * tempNode = GetOrCreateFirstChildElement(pParentNode,childname);

		if( !tempNode )
			return false;

		return SetValue_HostOrderIPRange(tempNode,begin_ip,end_ip);
	}
	//����item=01-02-03-04-05-06����01:02:03:04:05:06�Ļ�ȡ
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
	bool SetAttr(TiXmlNode *pNode,const char * attrname,const std::string &Attrvalue);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,int value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,bool value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,unsigned int value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT16 value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,ACE_UINT64 value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,double value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,float value);//��ȡָ���ڵ��ָ��������ֵ

	bool SetAttr(TiXmlNode *pNode,const char * attrname,const char * buf);

	BOOL SetAttr_UintRange(TiXmlNode *pNode,const char * attrname, unsigned int value1,unsigned int value2);

	//����item=192.168.0.1��IP��ַ���ͻ���mask�Ļ�ȡ
	BOOL SetAttr_HostOrderIP(TiXmlNode *pNode,const char * attrname, ACE_UINT32 value);

	//����item="192.168.0.1-192.168.0.255"��IP��ַ��Χ
	BOOL SetAttr_HostOrderIPRange(TiXmlNode *pNode,const char * attrname, ACE_UINT32 begin_ip,ACE_UINT32 end_ip);

	//����item=01-02-03-04-05-06����01:02:03:04:05:06�Ļ�ȡ
	BOOL SetAttr_Macaddr(TiXmlNode *pNode,const char * attrname, const BYTE * mac_addr);

	BOOL SetAttr(TiXmlNode *pNode,const char * attrname, const std::vector<std::string>& valueArray,char sep=',');

	BOOL SetAttr(TiXmlNode *pNode,const char * attrname,const std::vector<int>& value,char sep=',');
	BOOL SetAttr(TiXmlNode *pNode,const char * attrname,const std::vector<unsigned int>& value,char sep=',');

public:
	//��ȡ��һ������Ϊchildname��element,���û���򴴽�һ��
	TiXmlNode * GetOrCreateFirstChildElement(TiXmlNode *pParentNode,const char * childname,BOOL * pbCreate=NULL);
	TiXmlNode * GetOrCreateRootElement(const char * root_name,BOOL * pbCreate=NULL);

public:
	TiXmlNode * InsertEndChildElement(TiXmlNode *pParentNode,const char * name);
	TiXmlNode * InsertRootElement(const char * name);

public:
	TiXmlNode *FirstChildElement(TiXmlNode *pNode,const char * _value ) const;
	TiXmlNode *NextSiblingElement(TiXmlNode *pNode,const char * _value ) const;

public:
	//ɾ��pParentNode���棬���е�����Ϊsubitem�Ľڵ�
	void RemoveAllChild(TiXmlNode *pParentNode,const char * subitem);

public:
	//����ֻ-1��ʾ�ļ�û�д�
	//����������ʾ�������Ĵ�С��buf���ʱ����Ч���û�Ӧ��ʹ����ɺ�delete []buf.
	//����buf�Ľṹ��ǰ4���Լ���ʾ�ļ����ݵ�len+1.�����������ļ������ݡ����һ����0��
	static int ReadOneXmlFileForSend(const char * filename,char *& buf);

public:
	BOOL UnPacketOneFileFomeOneNode(TiXmlNode * node,const char * pPath, std::string * out_file_name);
	BOOL PacketOneFileIntoOneNode(TiXmlNode * node, const char * short_filename, const char * path);

	//�����·�����µ������ļ�������һ���ڵ㣨�����ӽڵ��ڣ�����·������û�з����ȥ
	//·���Ļ�������xml����״�ṹ
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
