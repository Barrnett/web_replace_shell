/*=========================================================
*	�ļ���		��	MyStr.h
*	������		��	
*	��������	��	2001-7-5 17:07:18
*	����		��	
*
*	�޸ļ�¼  	��	
*=========================================================*/
#ifndef __MYSTR_H__
#define __MYSTR_H__

#define CSTR_NOT_MT_SAFE

// ���� CSTR_NOT_MT_SAFE ��������ٶ�
//  ����֧�ֶ��߳� 
// �ַ���ʵ�ֵ��ڲ��ṹ
// ������ֱ�ӷ���!

#include "cpf/cpf.h"
#include "ace/Recursive_Thread_Mutex.h"

#pragma pack(1)
typedef struct SDesc
{
	// ������
	int	    m_Usage;	// Usage counter, 0 in empty slots, always 2 in m_Null
	char*	m_Text;		// 0-terminated string, or next free SDesc* if m_Usage==0
	int	    m_Length;	// Actual string length, excl. 0
	int	    m_Alloc;	// Allocated length, excl. 0
} SDesc;
#pragma pack()

/*********************************************************************
* Class:	CStr
* Purpose:	ʵ�ֶ�̬string
*********************************************************************/
class CPF_CLASS CStr
{

// ���ݿ鼰�ڲ����ʷ���
private:
		SDesc* data;
		int   m_wordfindpos;//��¼������һ�����ʵ���ʼλ��
		static void freestr(SDesc* tofree);
		static void freestr_nMT(SDesc* tofree);
		static SDesc* find_desc();

// ����, ����, ����
public:
		CStr();			//ȱʡ���캯��
		CStr(const CStr& source);	//���ƹ��캯��
		CStr(const char* s, int prealloc = 0);		//����0�������ַ������й���
		CStr(int prealloc);		//����һ���ַ���,����Ϊ��,������prealloc���ֽ�
		const CStr& operator=(const CStr& source);	//'='���������,CStr
		const CStr& operator=(const char* s);		//'='���������,��0�����ַ���
		const CStr& operator=(const char ch);		//'='���������,�ַ�
		~CStr();		//��������
//��MFC��CString����

#ifdef __AFX_H__
		CStr(const CString& source, int prealloc = 0);	//��mfc�е�CString���й���
		const CStr& operator=(const CString& source);	//'='���������,CString
#endif

// �����ԣ����ݣ����ȽϵĲ���

		bool IsEmpty() const;			//�ж��Ƿ�Ϊ��,Ϊ���򷵻�TRUE,
		int GetLength() const;			//ȡ�ó���
		operator const char* () const;	//ǿ��ת��Ϊ��ָ̬��
		operator char* () const;	//ǿ��ת��Ϊָ��
		
		const char* GetString() const;	//ȡ���ַ���,��\0����		
		char GetFirstChar() const;		//ȡ�õ�һ���ַ�
		char GetLastChar() const;		//ȡ�����һ���ַ�
		const char& operator[](int idx) const;//ȡ��Ӧλ�õ��ַ�
		char& operator[](int idx);//ȡ��Ӧλ�õ��ַ�
		char GetAt(int idx) const;		//ȡ��Ӧλ�õ��ַ�
		void GetLeft (int chars, CStr& result) const;	//ȡ��chars���ַ�������Ӵ�
		void GetRight (int chars, CStr& result) const;	//ȡ��chars���ַ����ҵ��Ӵ�
		void GetMiddle (int start, int chars, CStr& result) const;	//��start��ʼȡchars���ַ���Ϊ�Ӵ�
		int  GetWord(CStr& result);		//ȡ��һ������(�Կո�Ϊ�ֽ�,�������ո�).����1��ʾ�ɹ�,-1��ʾʧ��.�ú����ɷ�������,���Զ���󰤸�ȡ�����е���,ֱ������-1Ϊֹ.
		int  GetFirstWord(CStr& result);//ȡ����һ������(�Կո�Ϊ�ֽ�,�������ո�).����1�ɹ�,-1ʧ��
		int  GetLastWord(CStr& result);	//ȡ�����һ������(�Կո�Ϊ�ֽ�,�������ո�).����1�ɹ�,-1ʧ��
    
		int  IsAlpha();		//�ж�str���Ƿ�ȫΪ��ĸ,����Ƿ���0,���򷵻ص�һ������ĸ���ַ��±�
		int  IsDigit();		//�ж�str���Ƿ�ȫΪ����,����Ƿ���0,���򷵻ص�һ�������ֵ��ַ��±�
		int  ConvertInt();	//��str�е�����ת������Ӧ������,���Զ��жϳ�str�е����ֲ���,���ص���ת���Ľ��,����ɲμ�atoi��MSDN����

		int  Find (char ch, int startat = 0) const;				//��λ��startat��ʼ���ҵ�һ�������ַ�ch��λ��.�����ҳɹ�������Ӧ�±�,���򷵻�-1
		int  Find( const char *match,int startat = 0) const;		//��λ��startat��ʼ���ҵ�һ�������Ӵ�match��λ��.�����ҳɹ�������Ӧ�±�,���򷵻�-1
		int  ReverseFind (char ch, int startat = (int) -1) const;	//��λ��startat��ʼ���Ų��ҵ�һ������ch��λ��.�����ҳɹ�������Ӧ�±�,���򷵻�-1
		int  ReverseFind (const char *match,int len, int startat = (int) -1) const;	//add by xiebing ��λ��startat��ʼ���Ų��ҵ�һ�������Ӵ�match��λ��.�����ҳɹ�������Ӧ�±�,���򷵻�-1
	
		BOOL Equal(const char* match,int charnum) const;
		int  Compare (const char* match,int chars=0) const;    	//�ʹ�match����chars���ַ��ıȽ�,����ֵ�ֱ�Ϊ -1, 0 or 1 , ��charsΪ0ʱ��ʾȫ���Ƚ�
		int  CompareNoCase (const char* match,int chars=0) const;	//ͬ��,�����ִ�Сд

		const CStr& Assign(const char * pdata,int length);

// ȫ���޸�
		void Empty();			//����ַ����е�����(�����ͷ���ռ���ڴ�,��һ��Գ���Ա����͸��)
		void Reset();			//����ַ���,ͬʱ�ͷ��ڴ� 
		void GrowTo(int size);	//�����ڴ����ռ�
		void Compact(int only_above = 0);	//���ַ�������ѹ������,������
		void Format(const char* fmt, ...);	//���ַ������и�ʽ��
		//void FormatRes(UINT resid, ...);	//��mfc�е��ַ�����Դ���и�ʽ��,����δʵ��
		//BOOL LoadString(UINT resid);		//

// ׷��, ��ȡ
		void operator += (const CStr& obj);	//'+='���������,׷��CStr
		void operator += (const char* s);	//'+='���������,׷��0�����ַ���
		void operator += (const char ch);	//'+='���������,׷���ַ�	
		void AddString(const CStr& obj);	//׷��CStr	
		void AddString(const char* s);		//׷��0�����ַ���	
		void AddChar(char ch);				//׷���ַ�
		void AddChars(const char* s, int startat, int howmany);	//׷���ַ���s�д�startat��ʼ��howmany���ַ�,
		void AddStringAtLeft(const CStr& obj);			//�ڿ�ͷ������ַ���CStr
		void AddStringAtLeft(const char* s);			//�ڿ�ͷ�����0�����ַ���s
		void AddInt(int value);				//׷������value
		void AddDouble(double value, UINT after_dot);	//׷��ʵ��value,����ʵ��С�����λ��Ϊafter_dot
		void RemoveLeft(int count);		//ȥ����ߵ�count���ַ�
		void RemoveMiddle(int start, int count);		//ȥ����startλ�ÿ�ʼ��count���ַ�
		void RemoveRight(int count);		//ȥ���ұߵ�count���ַ�
		void TruncateAt(int idx);			//��λ��idx�ض�(�����\0�ķ�ʽ)
		CPF_CLASS friend CStr operator+(const CStr& s1, const CStr& s2);		//ƴ�Ӳ���
		CPF_CLASS friend CStr operator+(const CStr& s, const char* lpsz);		//ƴ�Ӳ���
		CPF_CLASS friend CStr operator+(const char* lpsz, const CStr& s);		//ƴ�Ӳ���
		CPF_CLASS friend CStr operator+(const CStr& s, const char ch);		//ƴ�Ӳ���
		CPF_CLASS friend CStr operator+(const char ch, const CStr& s);		//ƴ�Ӳ���
		void RTrim(const char* charset = NULL);		//ȥ�������ұߵĶ����ַ�,�������ΪNULLʱ��ʾ�ո�,�û������Լ�����(����������һ���ַ�)
		void LTrim(const char* charset = NULL);		//������˵��
		void AllTrim(const char* charset = NULL);	//ǰ����˵���϶�Ϊһ
		
	
// ������ʵ�ַ���
protected:
		void NewFromString(const char* s, int slen, int prealloc);	//[�ڲ�ʵ��]
		void Buffer (int newlength);				//[�ڲ�ʵ��]
		void CoreAddChars(const char* s, int howmany);		//[�ڲ�ʵ��]
		void FormatCore (const char* x, va_list& marker);	//[�ڲ�ʵ��]
		BOOL FmtOneValue (const char*& x, va_list& marker);	//[�ڲ�ʵ��]

public:	//����,�û��ٲ���Ϥ�����,Ӧ����ֱ�ӵ���
		static SDesc  m_Null;

#ifndef CSTR_NOT_MT_SAFE
		static ACE_Recursive_Thread_Mutex m_CacheArbiter;
#endif
		static bool m_CacheArbiterInited;
		static UINT m_ArbiterWithinCS;
		
		// ������
		static void ThrowIfNull(void* p);	//[�ڲ�ʵ��]
		static void ThrowPgmError();		//[�ڲ�ʵ��]
		static void ThrowNoUnicode();		//[�ڲ�ʵ��]
		static void ThrowBadIndex();		//[�ڲ�ʵ��]


//Ϊ�˺�CString���÷������,�ر������¹��ܺ���

		//ȥ�������ڼ���(����)�е��ַ�,������˼�෴,����ɲμ�MSDN
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
		//����ת������
		void MakeUpper();
		void MakeLower();
		//void MakeReverse();//ǰ����
		int  Replace( const char chOld, const char chNew );
		int  Replace( const char* lpszOld, const char* lpszNew );
		void SetEndChar(const char ch);
		int  FindOneOf( const char* lpszCharSet ) const;
};

CPF_CLASS BOOL operator ==(const CStr& s1, const CStr& s2);	//�������ַ����ıȽϲ���
CPF_CLASS BOOL operator ==(const CStr& s1, LPCTSTR s2);		//�������ַ����ıȽϲ���
CPF_CLASS BOOL operator ==(LPCTSTR s1, const CStr& s2);		//�������ַ����ıȽϲ���
CPF_CLASS BOOL operator !=(const CStr& s1, const CStr& s2);	//�������ַ����ıȽϲ���
CPF_CLASS BOOL operator !=(const CStr& s1, LPCTSTR s2);		//�������ַ����ıȽϲ���
CPF_CLASS BOOL operator !=(LPCTSTR s1, const CStr& s2);		//�������ַ����ıȽϲ���

/*********************************************************************
* Procs:	CStr::ThrowXXX()
* Purpose:	�쳣����
*********************************************************************/

#ifndef CSTR_OWN_ERRORS

void cstr_abort_application(int fatal_type);
inline void CStr::ThrowBadIndex()		{ cstr_abort_application(2); }
inline void CStr::ThrowPgmError()		{ cstr_abort_application(3); }
inline void CStr::ThrowNoUnicode()		{ cstr_abort_application(4); }


//  ��������ָ���飬��ʵ�����·���
//      void CStr::ThrowIfNull(void*)
//      { if (p == NULL)
//           ThrowSomeFatalError();
//      }
inline void CStr::ThrowIfNull(void*)  { /* no-op */ }


#endif


/*********************************************************************
* һЩ�򵥵ĺ�� inline����
*********************************************************************/

#define SET_data_EMPTY()		\
	data = &CStr::m_Null;		\
	data->m_Usage++;

/*********************************************************************
* Proc:		CStr::CStr()
* Purpose:	����յ�ʵ��
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
//��ͷβ��ȥ charset �ַ���Ĭ��Ϊ�ո�
inline void CStr::AllTrim(const char* charset /*= NULL*/)
{
	RTrim(charset);
	LTrim(charset);
}

#endif // __MYSTR_H__

