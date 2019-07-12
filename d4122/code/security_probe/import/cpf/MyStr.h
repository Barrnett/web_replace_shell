/*=========================================================
*	文件名		：	MyStr.h
*	创建者		：	
*	创建日期	：	2001-7-5 17:07:18
*	描述		：	
*
*	修改记录  	：	
*=========================================================*/
#ifndef __MYSTR_H__
#define __MYSTR_H__

#define CSTR_NOT_MT_SAFE

// 定义 CSTR_NOT_MT_SAFE 可以提高速度
//  但不支持多线程 
// 字符串实现的内部结构
// 不允许直接访问!

#include "cpf/cpf.h"
#include "ace/Recursive_Thread_Mutex.h"

#pragma pack(1)
typedef struct SDesc
{
	// 数据区
	int	    m_Usage;	// Usage counter, 0 in empty slots, always 2 in m_Null
	char*	m_Text;		// 0-terminated string, or next free SDesc* if m_Usage==0
	int	    m_Length;	// Actual string length, excl. 0
	int	    m_Alloc;	// Allocated length, excl. 0
} SDesc;
#pragma pack()

/*********************************************************************
* Class:	CStr
* Purpose:	实现动态string
*********************************************************************/
class CPF_CLASS CStr
{

// 数据块及内部访问方法
private:
		SDesc* data;
		int   m_wordfindpos;//记录搜索下一个单词的起始位置
		static void freestr(SDesc* tofree);
		static void freestr_nMT(SDesc* tofree);
		static SDesc* find_desc();

// 构造, 复制, 分配
public:
		CStr();			//缺省构造函数
		CStr(const CStr& source);	//复制构造函数
		CStr(const char* s, int prealloc = 0);		//从以0结束的字符串进行构造
		CStr(int prealloc);		//开辟一个字符串,内容为空,但分配prealloc个字节
		const CStr& operator=(const CStr& source);	//'='运算符重载,CStr
		const CStr& operator=(const char* s);		//'='运算符重载,以0结束字符串
		const CStr& operator=(const char ch);		//'='运算符重载,字符
		~CStr();		//析构函数
//从MFC的CString构造

#ifdef __AFX_H__
		CStr(const CString& source, int prealloc = 0);	//从mfc中的CString进行构造
		const CStr& operator=(const CString& source);	//'='运算符重载,CString
#endif

// 对属性，数据，及比较的操作

		bool IsEmpty() const;			//判断是否为空,为空则返回TRUE,
		int GetLength() const;			//取得长度
		operator const char* () const;	//强制转换为静态指针
		operator char* () const;	//强制转换为指针
		
		const char* GetString() const;	//取得字符串,以\0结束		
		char GetFirstChar() const;		//取得第一个字符
		char GetLastChar() const;		//取得最后一个字符
		const char& operator[](int idx) const;//取相应位置的字符
		char& operator[](int idx);//取相应位置的字符
		char GetAt(int idx) const;		//取相应位置的字符
		void GetLeft (int chars, CStr& result) const;	//取第chars个字符以左的子串
		void GetRight (int chars, CStr& result) const;	//取第chars个字符以右的子串
		void GetMiddle (int start, int chars, CStr& result) const;	//从start开始取chars个字符作为子串
		int  GetWord(CStr& result);		//取出一个单词(以空格为分界,不包括空格).返回1表示成功,-1表示失败.该函数可反复调用,会自动向后挨个取出所有单词,直到返回-1为止.
		int  GetFirstWord(CStr& result);//取出第一个单词(以空格为分界,不包括空格).返回1成功,-1失败
		int  GetLastWord(CStr& result);	//取出最后一个单词(以空格为分界,不包括空格).返回1成功,-1失败
    
		int  IsAlpha();		//判断str中是否全为字母,如果是返回0,否则返回第一个非字母的字符下标
		int  IsDigit();		//判断str中是否全为数字,如果是返回0,否则返回第一个非数字的字符下标
		int  ConvertInt();	//将str中的数字转换成相应的整数,会自动判断出str中的数字部分,返回的是转换的结果,具体可参见atoi的MSDN帮助

		int  Find (char ch, int startat = 0) const;				//从位置startat开始查找第一个出现字符ch的位置.当查找成功返回相应下标,否则返回-1
		int  Find( const char *match,int startat = 0) const;		//从位置startat开始查找第一个出现子串match的位置.当查找成功返回相应下标,否则返回-1
		int  ReverseFind (char ch, int startat = (int) -1) const;	//从位置startat开始倒着查找第一个出现ch的位置.当查找成功返回相应下标,否则返回-1
		int  ReverseFind (const char *match,int len, int startat = (int) -1) const;	//add by xiebing 从位置startat开始倒着查找第一个出现子串match的位置.当查找成功返回相应下标,否则返回-1
	
		BOOL Equal(const char* match,int charnum) const;
		int  Compare (const char* match,int chars=0) const;    	//和串match进行chars个字符的比较,返回值分别为 -1, 0 or 1 , 当chars为0时表示全部比较
		int  CompareNoCase (const char* match,int chars=0) const;	//同上,不区分大小写

		const CStr& Assign(const char * pdata,int length);

// 全局修改
		void Empty();			//清空字符串中的内容(但不释放所占的内存,这一点对程序员而言透明)
		void Reset();			//清空字符串,同时释放内存 
		void GrowTo(int size);	//增加内存分配空间
		void Compact(int only_above = 0);	//对字符串进行压缩处理,不常用
		void Format(const char* fmt, ...);	//对字符串进行格式化
		//void FormatRes(UINT resid, ...);	//对mfc中的字符串资源进行格式化,但并未实现
		//BOOL LoadString(UINT resid);		//

// 追加, 提取
		void operator += (const CStr& obj);	//'+='运算符重载,追加CStr
		void operator += (const char* s);	//'+='运算符重载,追加0结束字符串
		void operator += (const char ch);	//'+='运算符重载,追加字符	
		void AddString(const CStr& obj);	//追加CStr	
		void AddString(const char* s);		//追加0结束字符串	
		void AddChar(char ch);				//追加字符
		void AddChars(const char* s, int startat, int howmany);	//追加字符串s中从startat开始的howmany个字符,
		void AddStringAtLeft(const CStr& obj);			//在开头处添加字符串CStr
		void AddStringAtLeft(const char* s);			//在开头处添加0结束字符串s
		void AddInt(int value);				//追加整数value
		void AddDouble(double value, UINT after_dot);	//追加实数value,其中实数小数点后位数为after_dot
		void RemoveLeft(int count);		//去掉左边的count个字符
		void RemoveMiddle(int start, int count);		//去掉从start位置开始的count个字符
		void RemoveRight(int count);		//去掉右边的count个字符
		void TruncateAt(int idx);			//从位置idx截断(用添加\0的方式)
		CPF_CLASS friend CStr operator+(const CStr& s1, const CStr& s2);		//拼接操作
		CPF_CLASS friend CStr operator+(const CStr& s, const char* lpsz);		//拼接操作
		CPF_CLASS friend CStr operator+(const char* lpsz, const CStr& s);		//拼接操作
		CPF_CLASS friend CStr operator+(const CStr& s, const char ch);		//拼接操作
		CPF_CLASS friend CStr operator+(const char ch, const CStr& s);		//拼接操作
		void RTrim(const char* charset = NULL);		//去掉所有右边的多余字符,传入参数为NULL时表示空格,用户可以自己传入(甚至可以是一组字符)
		void LTrim(const char* charset = NULL);		//见上者说明
		void AllTrim(const char* charset = NULL);	//前两者说明合二为一
		
	
// 其他的实现方法
protected:
		void NewFromString(const char* s, int slen, int prealloc);	//[内部实现]
		void Buffer (int newlength);				//[内部实现]
		void CoreAddChars(const char* s, int howmany);		//[内部实现]
		void FormatCore (const char* x, va_list& marker);	//[内部实现]
		BOOL FmtOneValue (const char*& x, va_list& marker);	//[内部实现]

public:	//变量,用户再不熟悉的情况,应避免直接调用
		static SDesc  m_Null;

#ifndef CSTR_NOT_MT_SAFE
		static ACE_Recursive_Thread_Mutex m_CacheArbiter;
#endif
		static bool m_CacheArbiterInited;
		static UINT m_ArbiterWithinCS;
		
		// 错误处理
		static void ThrowIfNull(void* p);	//[内部实现]
		static void ThrowPgmError();		//[内部实现]
		static void ThrowNoUnicode();		//[内部实现]
		static void ThrowBadIndex();		//[内部实现]


//为了和CString的用法相兼容,特别补允以下功能函数

		//去掉所有在集合(参数)中的字符,两者意思相反,具体可参见MSDN
		CStr Right(int nchars) const
		{
			CStr temp;
			GetRight(nchars,temp);
			return temp;
		}

		CStr Left(int nchars) const
		{
			CStr temp;
			GetLeft(nchars,temp);
			return temp;
		}

		CStr SpanInclude(const char *sz) const;
		CStr SpanInclude(const CStr str) const;
		CStr SpanExclude(const char *sz) const;
		CStr SpanExclude(const CStr str) const;
		//各种转换函数
		void MakeUpper();
		void MakeLower();
		//void MakeReverse();//前后倒置
		int  Replace( const char chOld, const char chNew );
		int  Replace( const char* lpszOld, const char* lpszNew );
		void SetEndChar(const char ch);
		int  FindOneOf( const char* lpszCharSet ) const;
};

CPF_CLASS BOOL operator ==(const CStr& s1, const CStr& s2);	//对两个字符串的比较操作
CPF_CLASS BOOL operator ==(const CStr& s1, LPCTSTR s2);		//对两个字符串的比较操作
CPF_CLASS BOOL operator ==(LPCTSTR s1, const CStr& s2);		//对两个字符串的比较操作
CPF_CLASS BOOL operator !=(const CStr& s1, const CStr& s2);	//对两个字符串的比较操作
CPF_CLASS BOOL operator !=(const CStr& s1, LPCTSTR s2);		//对两个字符串的比较操作
CPF_CLASS BOOL operator !=(LPCTSTR s1, const CStr& s2);		//对两个字符串的比较操作

/*********************************************************************
* Procs:	CStr::ThrowXXX()
* Purpose:	异常处理
*********************************************************************/

#ifndef CSTR_OWN_ERRORS

void cstr_abort_application(int fatal_type);
inline void CStr::ThrowBadIndex()		{ cstr_abort_application(2); }
inline void CStr::ThrowPgmError()		{ cstr_abort_application(3); }
inline void CStr::ThrowNoUnicode()		{ cstr_abort_application(4); }


//  若想做空指针检查，请实现以下方法
//      void CStr::ThrowIfNull(void*)
//      { if (p == NULL)
//           ThrowSomeFatalError();
//      }
inline void CStr::ThrowIfNull(void*)  { /* no-op */ }


#endif


/*********************************************************************
* 一些简单的宏和 inline函数
*********************************************************************/

#define SET_data_EMPTY()		\
	data = &CStr::m_Null;		\
	data->m_Usage++;

/*********************************************************************
* Proc:		CStr::CStr()
* Purpose:	构造空的实例
*********************************************************************/

inline CStr::CStr()
{
    m_wordfindpos =0;
	SET_data_EMPTY();
}


/*********************************************************************
* Proc:		CStr::GetFirstChar
*********************************************************************/

inline char CStr::GetFirstChar() const
{
	return data->m_Text[0];
}


/*********************************************************************
* Proc:		CStr::IsEmpty and GetLength
*********************************************************************/

inline bool CStr::IsEmpty() const
{
	return data->m_Length == 0;
}

inline int CStr::GetLength() const
{
	return data->m_Length;
}


/*********************************************************************
* Proc:		CStr::operator []
*********************************************************************/

inline const char& CStr::operator[](int idx) const
{
#ifdef _DEBUG
	if (idx >= GetLength())
		ThrowBadIndex();
#endif

	return data->m_Text[idx];
}

inline char& CStr::operator[](int idx)
{
#ifdef _DEBUG
	if (idx >= GetLength())
		ThrowBadIndex();
#endif

	return data->m_Text[idx];
}

inline char CStr::GetAt(int idx) const
{
	//return *this[idx];
	return data->m_Text[idx];
}


/*********************************************************************
* Proc:		CStr::operator 'cast to const char*'
*********************************************************************/

inline CStr::operator const char* () const
{
	return data->m_Text;
}

inline CStr::operator  char* () const
{
	return data->m_Text;
}

inline const char* CStr::GetString() const
{
	return data->m_Text;
}


/*********************************************************************
* Proc:		CStr::[operator ==] and [operator !=] inlined forms
*********************************************************************/

inline CPF_CLASS BOOL operator ==(LPCTSTR s1, const CStr& s2)
{ return (s2 == s1); }

inline CPF_CLASS BOOL operator !=(const CStr& s1, const CStr& s2)
{ return !(s1 == s2); }

inline CPF_CLASS BOOL operator !=(const CStr& s1, LPCTSTR s2)
{ return !(s1 == s2); }

inline CPF_CLASS BOOL operator !=(LPCTSTR s1, const CStr& s2)
{ return !(s2 == s1); }


/*********************************************************************
* Proc:		CStr::AddString - synonyms for operators +=
*********************************************************************/

inline void CStr::AddString(const CStr& obj)
{ *this += obj; }

inline void CStr::AddString(const char* s)
{ *this += s; }

inline void CStr::AddStringAtLeft(const CStr& obj)
{
	AddStringAtLeft (obj.GetString());
}

inline void CStr::operator += (const char ch)
{
	AddChar (ch);
}


/*********************************************************************
* Proc:		CStr::AllTrim - a combination of RTrim and LTrim
*********************************************************************/
//从头尾移去 charset 字符，默认为空格
inline void CStr::AllTrim(const char* charset /*= NULL*/)
{
	RTrim(charset);
	LTrim(charset);
}

#endif // __MYSTR_H__

