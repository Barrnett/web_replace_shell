//////////////////////////////////////////////////////////////////////////
//HttpKeyHelper.h

#pragma once

#include "cpf/cpf.h"
#include "cpf/CmdKeyFinderTable.h"
#include "cpf/CommonMacTCPIPDecode.h"
#include "cpf/MyStr.h"

//http-packet包的种类定义
enum HTTP_PACKET_STYLE{
	/* client to server Reguest*/
	PACKET_STYLENULL = 0,
	GET_HEAD = 1,	//GET head        
	HEAD_HEAD = 2,	//HEAD' head      
	POST_HEAD = 3,	//POST head       
	PUT_HEAD = 4,	//PUT head		  
	DELETE_HEAD = 5,//DELETE head	  
	OPTION_HEAD = 6,//OPTION head
	TRACE_HEAD = 7,	//TRACE head
	CONNECT_HEAD = 8,// CONNECT head

	HTTP_HEAD = 9,	//HTTP head
	DATA_PACKET = -1,
	OTHER_HEAD = 11,
	/*server to client Response*/
};



//应答码
typedef enum tagRspnsCode
{
	RSPNSCODE_UNKNOWN = -1,
	RSPNSCODE_200 = 1 ,
	RSPNSCODE_302 ,
	RSPNSCODE_304 ,
}RSPNSCODE;


#define HTTP_MAX_FILELEN    (2*1024*1024)
#define HTTP_MAX_TITLELEN   256

/////////////////////////////////////////////////////////////////////////
//http协议中特殊符号和关键字的定义(rfc2616)
#define ASCII_SP ' '	//space 空格
#define ASCII_CR '\r'	//
#define ASCII_LF '\n'	//
#define ASCII_HT '\9'	//
#define ASCII_DQ '\"'	//double quote 双引号
#define ASCII_SQ '\''	//single quote 单引号
#define ASCII_DT '.'	//dot 句点
#define ASCII_VG '/'	//virgule 斜线号 
#define ASCII_TV '\\'	//turn virgule 反斜线号
#define ASCII_AT '@'	//at
#define ASCII_ZR '\0'   //零结束符
#define ASCII_CO ':'	//冒号 Colon

#define STR_NULL ""
#define STR_TV "\\"
#define STR_CRLF "\r\n"
#define STR_DBCRLF "\r\n\r\n"
#define STR_LWS "\r\n "
#define STR_URLCHAR "?;=&:* "

#ifdef _TEST
#define FILE_CRLF "\r\n"
#else
#define FILE_CRLF "<br>"
#endif //_TEST

#define KEY_HOST "Host: "
#define KEY_REFERER_EX "Referer: http://"
#define KEY_REFERER "Referer: "
#define KEY_CONTENTTYPE_EX "Content-Type: text/html"
#define KEY_CONTENTTYPE "Content-Type: "
#define KEY_CONTENTLEN "Content-Length: "
#define KEY_TRANSENCODE "Transfer-Encoding: "
#define KEY_CONTENTENCODE "Content-Encoding: "

#define KEY_CONTENT_TEXTHTML "text/html"
#define KEY_CONTENT_TEXTPLAIN "text/plain"
#define KEY_CONTENT_JAVA "java-internal/java.util.Vector"
#define KEY_CONTENT_IMAGE	 "image/"

#define KEY_CONTENT_WORD "application/msword"

#define KEY_CONTENT_APPFORM "application/x-www-form-urlencoded"

#define KEY_CONTENT_DISPOSITION "Content-Disposition:"

#define KEY_SRC "src="
#define KEY_WNDOPEN "window.open(\""
#define KEY_HREF "href=\""
#define KEY_100 "100"
#define KEY_200 "200"
#define KEY_206 "206"
#define KEY_302 "302"
#define KEY_304 "304"

#define HTML_START "<html>"

#define SUFFIX_HTM "htm"
#define SUFFIX_XML "xml"
#define SUFFIX_HTML "html"
#define SUFFIX_DHTML "dhtml"
#define SUFFIX_SHTML "shtml"

class CPF_CLASS CHttpKeyHelper
{
public:
	static void S_init();
	static void S_Close();

private:
	static int	ms_ref;

public:
	static CCmdKeyFinderTable<HTTP_PACKET_STYLE>	m_cmdTable;

public:
	typedef struct tagHTTP_HEAD_ITEM
	{
		UINT_PTR head_int_key;//取头4个字节

		char head_string[32];//host,cookie等,或者'GET ' 'POST '等

		INT_PTR head_len;//head_strng的长度		

		INT_PTR	head_type;

	}HTTP_HEAD_ITEM;

	typedef HTTP_HEAD_ITEM	HTTP_LINE_ITEM;

public:
	enum{MAX_HTTP_HEAD_TYPE=9};
	static HTTP_HEAD_ITEM m_http_head_array[];


public:
	static HTTP_PACKET_STYLE GetPacketStyle(int find_optimize,const BYTE * pBuf,size_t len);

public:
	//fiiled_name="Host: "或者"Referer: "等，注意区分大小写，还有后面的空格
	static BOOL GetHttpFiledContent(const char * fiiled_name,unsigned int filed_len,
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		CStr& str_content);

	static inline BOOL GetHostName(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		CStr& str_content)
	{
		static const char flag[] = "Host: ";

		return GetHttpFiledContent(flag,(unsigned int)sizeof(flag)-1,context,str_content);
	}

	static inline BOOL GetReferer(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		CStr& str_content)
	{
		static const char flag[] = "Referer: ";

		if( GetHttpFiledContent(flag,(unsigned int)sizeof(flag)-1,context,str_content) )
		{
			static const char http_head[] = "http://";

			if( strncmp(str_content.GetString(),http_head,sizeof(http_head)-1) == 0 )
			{
				str_content.RemoveLeft(sizeof(http_head)-1);				
			}

			return true;		 
		}

		return false;
	}

	static inline BOOL GetUserAgent(
		const CCommonMacTCPIPDecode::TCPIP_DECODE_CONTEXT& context,
		CStr& str_content)
	{
		static const char flag[] = "User-Agent: ";

		return GetHttpFiledContent(flag,(unsigned int)sizeof(flag)-1,context,str_content);
	}

	static BOOL GetTitle(const char * p1,const char *p2,CStr& title);

	static BOOL GetContentType(const char * stopstr,CStr& contenttype);

	static BOOL ParseFileNameFromURI(CStr& strfilename,
		const CStr& uri,const CStr& ContentType,
		const CStr& hostname,const CStr& szContentEncode,
		BOOL bungizp);

public:
	inline static BOOL ContentTypeIsText(const char * pContentType)
	{
		return (pContentType == ACE_OS::strstr(pContentType,"text/"));
	}

	inline static BOOL ContentTypeIsMultiPartForm(const char * pContentType)
	{
		static char key_type[] = "multipart/form-data";

		return ACE_OS::strncmp(pContentType,key_type, sizeof(key_type) - 1) ==0;
	}

	inline static BOOL ContentTypeIsURLForm(const char * pContentType)
	{
		static char key_type[] = "application/x-www-form-urlencoded";

		return ACE_OS::strncmp(pContentType,key_type, sizeof(key_type) - 1) ==0;
	}

	inline static BOOL ContentTypeIsHtml(const char * pContentType)
	{
		return ACE_OS::strncmp(pContentType,KEY_CONTENT_TEXTHTML, sizeof(KEY_CONTENT_TEXTHTML) - 1) ==0;
	}

	static BOOL FileTypeIsHtml(const char * suf);
	static BOOL FileTypeIsPic(const char * suf);
	static BOOL FileTypeIsVideo(const char * suf);
	static BOOL FileTypeIsAudio(const char * suf);

	inline static BOOL ContentCodeIsGzip(const char * pContentCode)
	{
		static char zip_type[] = "gzip";

		return (*(ACE_UINT32 *)pContentCode == *(ACE_UINT32 *)zip_type);
	}

	inline static BOOL TransEncodeIschunked(const char *pTransEncode)
	{
		static char key_type[] = "chunked";

		return ACE_OS::strncmp(pTransEncode,key_type, sizeof(key_type) - 1) ==0;
	}

};
