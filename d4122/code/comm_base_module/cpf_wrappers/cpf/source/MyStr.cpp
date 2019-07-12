/*=========================================================
*	File name	：	MyStr.cpp
*	Authored by	：	
*	Date		：	
*	Description	：	
*	Modify  	：	
*=========================================================*/

#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"

#include "cpf/MyStr.h"
#include "cpf/memmgr.h"
#include "cpf/other_tools.h"

/*********************************************************************
* Proc:		cstr_abort_application
* Purpose:	简单的错误处理，有对话框提示
*********************************************************************/
//static char buf[65535];

#ifndef CSTR_OWN_ERRORS
void cstr_abort_application(int fatal_type)
{
	// Display message and abort
	char msg[64];
	sprintf (msg, "CStr fatal %d", fatal_type);
	abort();
}
#endif


// Define specific type of ACE_ASSERT
#ifdef _DEBUG
#define CSTR_ASSERT(x)  { if ((x) == 0)  cstr_abort_application (1111); }
#else
#define CSTR_ASSERT(x)  { }
#endif

/*********************************************************************
* Class:	strcache_arb
* Purpose:	Helper class for maintaining the cache consistent
*			across threads
*********************************************************************/

struct strcache_arb
{
	strcache_arb();
	~strcache_arb();
};

#ifndef CSTR_NOT_MT_SAFE

// Thread-safe cache arbiter
strcache_arb::strcache_arb()
{
	if (!CStr::m_CacheArbiterInited) {
		CStr::m_CacheArbiterInited = true;
	}
	CStr::m_CacheArbiter.acquire();
	CStr::m_ArbiterWithinCS++;
}

inline strcache_arb::~strcache_arb()
{
	CStr::m_ArbiterWithinCS--;
	CStr::m_CacheArbiter.release();
}

#else

// Thread-unsafe cache arbiter
inline strcache_arb::strcache_arb()
{ }
inline strcache_arb::~strcache_arb()
{ }
#endif



SDesc CStr::m_Null = { 2, "", 0, 1 };
bool  CStr::m_CacheArbiterInited = false;

#ifndef CSTR_NOT_MT_SAFE

UINT  CStr::m_ArbiterWithinCS = 0;
ACE_Recursive_Thread_Mutex CStr::m_CacheArbiter;

#endif

/*********************************************************************
* Proc:		find_desc
* Purpose:	Helper.  Allocates (from the cache or memory) a SDesc block
*********************************************************************/

inline  SDesc* CStr::find_desc()
{
	BYTE* x = CPF::MyMemAlloc(sizeof(SDesc));
	CStr::ThrowIfNull(x);
	return (SDesc*) x;
}


/*********************************************************************
* Proc:		CStr::freestr (static method)
* Purpose:	Decrements the usage count of the specified string.  When
*			the count reaches zero, CPF::MyMemFrees the data bytes and moves
*			the SDesc block to the pool of available blocks.
*********************************************************************/

void CStr::freestr_nMT(SDesc* tofree)
{
#ifndef CSTR_NOT_MT_SAFE
	CSTR_ASSERT (m_ArbiterWithinCS > 0);
#endif
	
	// Check validity
	ACE_ASSERT(tofree);
	ACE_ASSERT(tofree->m_Usage>0);
	if (NULL==tofree || tofree->m_Usage<=0) 
	{
		//DUMPSTATE("tofree->m_Usage = %d s=%s\r\n", tofree->m_Usage, tofree->m_Text);
		return;
	}
	// Decrement counter, don't deallocate anything if we're using m_Null
	tofree->m_Usage--;
	if (tofree == &CStr::m_Null) {
		return;
	}
	// When the usage count reaches 0, CPF::MyMemFree the block
	if (tofree->m_Usage == 0) {
		// Deallocate associated data block
		CPF::MyMemFree (tofree->m_Text);
		// Deallocate the descriptor itself
		CPF::MyMemFree (tofree);
	}
}

void CStr::freestr(SDesc* tofree)
{
	strcache_arb keeper;
	freestr_nMT (tofree);
}


/*********************************************************************
* Proc:		CStr::destructor
*********************************************************************/

CStr::~CStr()
{
	// Note that we don't have to check for m_Null here, because
	//   the usage count there will always be 2 or more.
	// MT note: InterlockedDecrement() is used instead of --
	if (data->m_Usage<=0) 
	{
		//DUMPSTATE("data->m_Usage = %d this=%p s=%s\r\n", data->m_Usage, this, data->m_Text);
		return;
	}
	
	strcache_arb keeper;
	if (--data->m_Usage == 0) {
		if (data == &CStr::m_Null)
			return;
		// CPF::MyMemFree associated text block and descriptor
		CPF::MyMemFree(data->m_Text);
		CPF::MyMemFree(data);
	}
}


/*********************************************************************
* Proc:		CStr::NewFromString
* Purpose:	Core code for 'copy char* constructor' and '= operator'.
*			Assumes our 'data' field is garbage on entry.
*********************************************************************/

void CStr::NewFromString(const char* s, int slen, int prealloc)
{
	// Determine actual size of buffer that needs to be allocated
	if (slen > prealloc)
		prealloc = slen;
	// Empty string?
	if (slen == 0  &&  prealloc == 0) {
		SET_data_EMPTY();
		return;
	}
	// Round up to CPF::MyMemAlloc() granularity
	// Get memory for string
	char* target = (char*) CPF::MyMemAlloc (prealloc+1);
	ThrowIfNull(target);
	// Locate CPF::MyMemFree descriptor, fill it in
	data = find_desc();
	data->m_Text = target;
	data->m_Usage = 1;
	data->m_Length = slen;
	data->m_Alloc = prealloc;

	// Copy the string bytes
	
	//memcpy (target, s, slen+1); modify by zhuzp
	memcpy (target, s, slen);
	target[slen] = 0;

	return;
}


/*********************************************************************
* Proc:		CStr::CStr (UINT prealloc)
* Purpose:	Instantiates an empty string, but with the specified
*			number of bytes in the preallocated buffer.
* In:		prealloc - number of bytes to reserve
*********************************************************************/

CStr::CStr(int prealloc)
{
  
	m_wordfindpos =0;

	// Get memory for string
	char* target = (char*) CPF::MyMemAlloc (prealloc+1);
	ThrowIfNull(target);
	target[0] = 0;
	// Locate CPF::MyMemFree descriptor, fill it in
	data = find_desc();
	data->m_Text = target;
	data->m_Usage = 1;
	data->m_Length = 0;
	data->m_Alloc = prealloc;
}


/*********************************************************************
* Proc:		CStr::CStr(const char*)
* Purpose:	Construct an instance that exactly copies the specified
*			string.  An optional second parameter specifies the buffer
*			size (will be ignored if it is less than what's needed)
*********************************************************************/

CStr::CStr(const char* s, int prealloc /*= 0*/)
{
    m_wordfindpos =0;
	// No need to check for limit in CSTR_LARGE_STRINGS, because
	//   NewFromString will blow up because of the buffer size
	int slen = (int)strlen(s);
	NewFromString(s, slen, prealloc);
}


/*********************************************************************
* Proc:		CStr::copy constructor
*********************************************************************/

CStr::CStr(const CStr& source)
{
	if (this == &source)
		return;
    m_wordfindpos =0;
	data = source.data;
	data->m_Usage++;
}


/*********************************************************************
* Proc:		CStr::CStr(const CString&)
* Purpose:	Implemented only when using MFC.  Instantiates the string
*			from an MFC-provided one.
*********************************************************************/

#ifdef __AFX_H__
CStr::CStr(const CString& source, int prealloc /*= 0*/)
{
    m_wordfindpos =0;
	NewFromString((const char*) source, (int) source.GetLength(), prealloc);
}
#endif


/*********************************************************************
* Proc:		CStr::operator = CStr
* Purpose:	Copies a string into another string, destroying the
*			previous content.
*********************************************************************/

const CStr& CStr::operator=(const CStr& source)
{
    m_wordfindpos =0;
	if (this != &source) {
		freestr(data);
		data = source.data;
		data->m_Usage++;
	}
	return *this;
}


/*********************************************************************
* Proc:		CStr::operator = [const CString&]
* Purpose:	Implemented only when MFC is used.  
*********************************************************************/

#ifdef __AFX_H__
const CStr& CStr::operator=(const CString& source)
{
    m_wordfindpos =0;
	*this = (const char*) source;
	return *this;
}
#endif


/*********************************************************************
* Proc:		CStr::Buffer
* Purpose:	Helper.  Makes sure that our internal buffer has
*			the specified number of bytes available, and that
*			we can overwrite it (i.e. m_Usage is 1).  If this
*			isn't so, prepares a copy of our internal data.
*********************************************************************/

void CStr::Buffer (int newlength)
{
	// Reallocate. First handle case where we cannot just
	//   touch the buffer.  We don't need to test for m_Null
	//   here because it's m_Usage field is always >1
	strcache_arb keeper;
	if (data->m_Usage == 1) {
		// Buffer already big enough?
		if (data->m_Alloc >= newlength)
			return;
		// Nope. We need to reallocate here.
	}

	SDesc* prevdata = data;
	if (newlength < prevdata->m_Length)
		newlength = prevdata->m_Length;
	NewFromString(prevdata->m_Text, prevdata->m_Length, newlength);
	freestr_nMT(prevdata);
}


/*********************************************************************
* Proc:		CStr::Compact
* Purpose:	If m_Alloc is bigger than m_Length, shrinks the
*			buffer to hold only as much memory as necessary.
* In:		only_above - will touch the object only if the difference
*				is greater than this value.  By default, 0 is passed,
*				but other good values might be are 1 to 7.
* Rems:		Will compact even the buffer for a string shared between
*			several CStr instances.
*********************************************************************/

void CStr::Compact(int only_above /*= 0*/)
{
	// Nothing to do?
	if (data == &CStr::m_Null)
		return;
	strcache_arb keeper;
	int diff = (int) int(data->m_Alloc - data->m_Length);
	if (diff <= only_above)
		return;
	// Shrink buffer.  The up_alloc() call is because it is no good
	//   to allocate 14 or 15 bytes when we can get 16; the memory
	//   manager will waste the few remaining bytes anyway.
	int to_shrink = data->m_Length;
	// Using realloc to shrink a memory block might cause heavy
	//   fragmentation.  Therefore, a preferred method is to
	//   allocate a new block and copy the data there.
	char* xnew = (char*) CPF::MyMemAlloc (to_shrink+1);
	if (xnew == NULL)
		return;
	// No need to throw 'no mem' here
	// Copy data, substitute fields
	char* xold = data->m_Text;
	//UINT  xoal = data->m_Alloc;
	data->m_Text = xnew;
	memcpy (xnew, xold, data->m_Length+1);
	CPF::MyMemFree ((BYTE*) xold);
	data->m_Alloc = to_shrink;
}




/*********************************************************************
* Proc:		CStr::operator = const char
* Purpose:	Copies a char into our internal buffer, if it is big
*			enough and is used only by us; otherwise, CPF::MyMemFrees the
*			current instance and allocates a new one.
*********************************************************************/


const CStr& CStr::operator=(const char ch)
{
   return Assign(&ch,1);
}


/*********************************************************************
* Proc:		CStr::operator = const char*
* Purpose:	Copies a string into our internal buffer, if it is big
*			enough and is used only by us; otherwise, CPF::MyMemFrees the
*			current instance and allocates a new one.
*********************************************************************/

const CStr& CStr::operator=(const char* s)
{
	// Check for zero length string.
	size_t slen = strlen(s);

	return Assign(s,slen);
}

const CStr& CStr::Assign(const char * s,int slen)
{
	m_wordfindpos =0;

	if (s == NULL || slen == 0) 
	{
		if (data == &CStr::m_Null)
			return *this;
		freestr(data);
		SET_data_EMPTY();
		return *this;
	}
	// Can we handle this without reallocations?  NOTE: we do
	//   not use Buffer() here because if the string needs to
	//   be expanded, the old one will be copied, and we don't
	//   care about it anyway.
	strcache_arb keeper;
	if (data->m_Usage == 1  &&  data->m_Alloc >= (int)slen) {
		// Yes, copy bytes and get out
		memcpy (data->m_Text, s, slen);
		data->m_Text[slen] = 0;
		data->m_Length = (int) slen;
		return *this;
	}
	// No. CPF::MyMemFree old string, allocate new one.
	freestr_nMT(data);
	NewFromString(s, (int) slen, 0);
	return *this;
}

/*********************************************************************
* Proc:		CStr::Empty
* Purpose:	Sets the string to NULL.  However, the allocated buffer
*			is not released.
*********************************************************************/

void CStr::Empty()
{
    m_wordfindpos =0;
	// Already empty, and with buffer zero?
	if (data == &CStr::m_Null)
		return;
	// More than one instance served?
	strcache_arb keeper;
	if (data->m_Usage != 1) {
		// Get a copy of the buffer size, release shared instance
		UINT to_alloc = data->m_Alloc;
		freestr_nMT(data);
		// Get memory for string
		char* target = (char*) CPF::MyMemAlloc (to_alloc+1);
		ThrowIfNull(target);
		// Locate CPF::MyMemFree descriptor, fill it in
		data = find_desc();
		data->m_Text = target;
		data->m_Usage = 1;
		data->m_Length = 0;
		data->m_Alloc = (int) to_alloc;
		target[0] = 0;
		return;
	}
	// Only one instance served, so just set the charcount to zero.
	data->m_Text[0] = 0;
	data->m_Length = 0;
}


/*********************************************************************
* Proc:		CStr::Reset
* Purpose:	Sets the string to NULL, deallocates buffer
*********************************************************************/

void CStr::Reset()
{
    m_wordfindpos =0;
	if (data != &CStr::m_Null) {
		freestr(data);
		SET_data_EMPTY();
	}
}


/*********************************************************************
* Proc:		CStr::AddChar
* Purpose:	Appends a single character to the end of the string
*********************************************************************/

void CStr::AddChar(char ch)
{
	// Get a copy if m_Usage>1, expand buffer if necessary
	UINT cur_len = data->m_Length;
	Buffer (int(cur_len + 1));
	// And append the character (no need for CStrCritical because of Buffer)
	data->m_Text[cur_len] = ch;
	data->m_Text[cur_len+1] = 0;
	data->m_Length = int(cur_len+1);
}


/*********************************************************************
* Proc:		CStr::AddInt
* Purpose:	Appends a decimal signed integer, possibly with - sign
*********************************************************************/

void CStr::AddInt(int value)
{
	char buf[32]={0};
	sprintf(buf,"%d",value);
	AddString (buf);
}


/*********************************************************************
* Proc:		CStr::AddDouble
* Purpose:	Appends a signed double value, uses specified # of digits
*********************************************************************/

void CStr::AddDouble(double value, UINT after_dot)
{
	if (after_dot > 48)
		after_dot = 48;
	char fmt[16];
	sprintf (fmt, "%%.%df", (int) after_dot);
	char buf[256];
	sprintf (buf, fmt, value);
	AddString (buf);
}


/*********************************************************************
* Proc:		CStr::CoreAddChars
* Purpose:	Core code for AddChars() and operators +=; assumes
*			howmany is bigger than 0
*********************************************************************/

void CStr::CoreAddChars(const char* s, int howmany)
{
	ACE_ASSERT(howmany>=0&&howmany<1024*1024);
	// Prepare big enough buffer
	Buffer (int(data->m_Length + howmany));
	// And copy the bytes
	char* dest = data->m_Text + data->m_Length;
	memcpy (dest, s, howmany);
	dest[howmany] = 0;
	data->m_Length = int(data->m_Length + howmany);
}


/*********************************************************************
* Proc:		CStr::operator += (both from const char* and from CStr)
* Purpose:	Append a string to what we already contain.
*********************************************************************/

void CStr::operator += (const CStr& obj)
{
	if (obj.data->m_Length != 0)
		CoreAddChars (obj.data->m_Text, obj.data->m_Length);
}

void CStr::operator += (const char* s)
{
	size_t slen = strlen(s);
	if (slen != 0) {
		CoreAddChars (s, (int) slen);
	}
}


/*********************************************************************
* Proc:		CStr::AddChars
* Purpose:	Catenates a number of characters to our internal data.
*********************************************************************/

void CStr::AddChars(const char* s, int startat, int howmany)
{
	if (howmany != 0) {
		CoreAddChars (s+startat, howmany);
	}
}


/*********************************************************************
* Proc:		CStr::AddStringAtLeft
* Purpose:	Prepend a string before us
*********************************************************************/

void CStr::AddStringAtLeft(const char* s)
{
	size_t slen = strlen(s);
	if (slen == 0)
		return;
	Buffer (int(GetLength() + slen));
	// Move existing data -- do NOT use memcpy!!
	memmove (data->m_Text+slen, data->m_Text, GetLength()+1);
	// And copy bytes to be prepended
	memcpy (data->m_Text, s, slen);
	data->m_Length = int(data->m_Length + slen);
}


/*********************************************************************
* Proc:		operator +LPCSTR for CStr
*********************************************************************/

CStr operator+(const char* lpsz, const CStr& s)
{
	CStr temp (int(s.GetLength() + strlen(lpsz)));
	temp  = lpsz;
	temp += s;
	return temp;
}


/*********************************************************************
* Proc:		operator +char for CStr
*********************************************************************/

CStr operator+(const char ch, const CStr& s)
{
	CStr temp (int(s.GetLength() + 1));
	temp = ch;
	temp += s;
	return temp;
}


/*********************************************************************
* Proc:		CStr::GetLastChar
*********************************************************************/

char CStr::GetLastChar() const
{
	UINT l = GetLength();
	if (l < 1)
		ThrowBadIndex();
	return data->m_Text[l-1];
}

/*********************************************************************
* Proc:		CStr::GetWord 解析出单词 -1终止
//解析出第一个单词 -1终止
//解析出最后一个单词 -1终止
*********************************************************************/

int  CStr::GetFirstWord(CStr& result)
{
	result.Empty();
	int start,end;
	end = start= 0;
	char *p = data->m_Text;
	while (p[start] == ' ')
			start++;
	end    =  start;
	while (p[end] !=' '&&p[end] !='\0')
		    end++;
	if(end>start)
	{
	   GetMiddle(start,end-start,result);
	   return 1;
	}
	return -1;
}

int  CStr::GetLastWord(CStr& result)
{
	//2001/4/27 bug
	result.Empty();

	if(IsEmpty())
		return -1;

	int start,end;
	end = GetLength()-1;
	start= 0;

	char *p = data->m_Text;
	while (end &&p[end] == ' ')
			end--;

	if((end==0)&&p[end] == ' ')
		return -1;

	start    =  end;
	while (start&&p[start] !=' ')
		    start--;

	if((start==0)&&p[start] !=' ')
	{
		GetMiddle(start,end-start+1,result);
		return 1;
	}

	if(end>start)
	{
		GetMiddle(start+1,end-start,result);
		return 1;
	}
	
	return -1;

}

int  CStr::GetWord(CStr& result)
{
	result.Empty();
	int start,end;
	end = 0;
    start = m_wordfindpos;
	char *p = data->m_Text;
	while (p[start] == ' ')
			start++;
	end    =  start;
	while (p[end] !=' '&&p[end] !='\0')
		    end++;
	if(end>start)
	{
       m_wordfindpos = end;
	   GetMiddle(start,end-start,result);
	   return 1;
	}
	return -1;
}


/*********************************************************************
* Proc:		CStr::GetLeft
*********************************************************************/

void CStr::GetMiddle (int start, int chars, CStr& result) const
{
	result.Empty();
	// Nothing to return?
	int l = GetLength();
	if (l == 0  ||  (UINT(start)+chars) == 0)
		return;
	// Do not return data beyond the end of the string
	if (start >= l)
		return;
	if ((start+chars) >= l)
		chars = int(l - start);
	// Copy bytes
	result.CoreAddChars(GetString()+start, chars);
}

void CStr::GetLeft (int chars, CStr& result) const
{
	GetMiddle(0, chars, result);
}

void CStr::GetRight (int chars, CStr& result) const
{
	if (chars >= GetLength()) {
		result = *this;
		return;
	}
	GetMiddle(int(GetLength()-chars), chars, result);
}


/*********************************************************************
* Proc:		CStr::IsAlpha CStr::IsDigit
*********************************************************************/

int CStr::IsAlpha()
{
	char *p = data->m_Text;
	int len = GetLength();
	int i;
	for(i=0;i<len;i++)
	{
		if(isalpha(p[i])==0)
			return 0;
	}
	return i;
}

int CStr::IsDigit()
{
	char *p = data->m_Text;
	int len = GetLength();
	int i;
	for(i=0;i<len;i++)
	{
		if(isdigit(p[i])==0)
			return 0;
	}
	return i;
}

/*********************************************************************
* Proc:		CStr::ConvertInt 
*********************************************************************/

int  CStr::ConvertInt()
{
	return atoi (data->m_Text);
}

/*********************************************************************
* Proc:		CStr::TruncateAt
* Purpose:	Cuts off the string at the character with the specified
*			index.  The allocated buffer remains the same.
*********************************************************************/

void CStr::TruncateAt (int idx)
{
	if (idx >= GetLength())
		return;
	// Spawn a copy if necessary
	Buffer (data->m_Alloc);		// Preserve buffer size
	// And do the truncation
	data->m_Text[idx] = 0;
	data->m_Length = idx;
}


/*********************************************************************
* Proc:		CStr::Find and ReverseFind
* Purpose:	Scan the string for a particular character (must not
*			be 0); return the index where the character is found
*			first, or -1 if cannot be met
*********************************************************************/

int CStr::Find (char ch, int startat /*= 0*/) const
{
	// Start from middle of string?
	if (startat > 0) {
		if (startat >= GetLength())
			ThrowBadIndex();
	}
	char* scan = strchr (data->m_Text+startat, ch);
	if (scan == NULL)
		return -1;
	else
		return (int)(scan - data->m_Text);
}

int CStr::ReverseFind (char ch, int startat /*= -1*/) const
{
	if (startat == (int) -1) {
		// Scan entire string
		char* scan = strrchr (data->m_Text, ch);
		if (scan)
			return (int)(scan - data->m_Text);
	}
	else {
		// Make sure the index is OK
		if (startat > GetLength())
			ThrowBadIndex();
		for (int findex = (int) startat-1; findex >= 0; findex--) {
			if (data->m_Text[findex] == ch)
				return findex;
		}
	}
	return -1;
}
int  CStr::ReverseFind (const char *match,int len, int startat ) const
{
	int datalen;
	if (-1==startat)
		datalen = data->m_Length;
	else
	{
		if (startat>=0&&startat<=data->m_Length)
			datalen = startat;
		else
			return -1;
	}

	for (int i=datalen - len-1; i>=0 ; i--)
	{
		if ( data->m_Text[i]!=*match )
			continue;
		if ( 0!=memcmp(match,data->m_Text+i,len ) )
			continue;
		return i;
	}
	return -1;
}

/*********************************************************************
 * NAME:    Find()
 * PURPOSE: Find a string in text
 *
 * PARAM:   match   = the string we are looking for
 *          startat = where to start search  string 
*********************************************************************/

int CStr::Find( const char *match,int startat /*= 0*/) const
{

	// Start from middle of string?
	if (startat > 0) {
		if (startat >= GetLength())
			ThrowBadIndex();
	}

	char* scan = strstr (data->m_Text+startat, match);
	if (scan == NULL)
		return -1;
	else
		return (int)(scan - data->m_Text);
}



/*********************************************************************
* Proc:		CStr::Compare and CompareNoCase
*********************************************************************/


int CStr::Compare (const char* match,int chars) const
{
	int i;
	if(chars)
		i = strncmp (data->m_Text, match,chars);
	else
		i = strcmp (data->m_Text, match);

	if (i == 0)
		return 0;
	else if (i < 0)
		return -1;
	else
		return 1;
}

BOOL CStr::Equal(const char* match,int charnum) const
{
	const char *p = data->m_Text;
	int len = data->m_Length;
	if(len>=charnum&&memcmp(p,match,charnum)==0)
		return TRUE;
	return FALSE;
}


int CStr::CompareNoCase (const char* match,int chars) const
{
	int i ;
	if(chars)
		i= ACE_OS::strncasecmp (data->m_Text, match,chars);
	else
		i= ACE_OS::strcasecmp (data->m_Text, match);

	if (i == 0)
		return 0;
	else if (i < 0)
		return -1;
	else
		return 1;
}


/*********************************************************************
* Proc:		CStr::GrowTo
* Purpose:	If the buffer is smaller than the amount of characters
*			specified, reallocates the buffer.  This function cannot
*			reallocate to a buffer smaller than the existing one.
*********************************************************************/

void CStr::GrowTo(int size)
{
	Buffer (size);
}


/*********************************************************************
* Proc:		CStr::operator == (basic forms, the rest are inline)
*********************************************************************/

BOOL operator ==(const CStr& s1, const CStr& s2)
{
	int slen = s2.GetLength();
	if (s1.GetLength() != slen)
		return FALSE;
	return memcmp((void *)s1.GetString(), (void *)s2.GetString(), slen) == 0;
}

BOOL operator ==(const CStr& s1, LPCTSTR s2)
{
	size_t slen = strlen(s2);
	if (s1.GetLength() != (int)slen)
		return FALSE;
	return memcmp(s1.GetString(), s2, slen) == 0;
}


/*********************************************************************
* Proc:		CStr::RemoveLeft
*********************************************************************/

void CStr::RemoveLeft(int count)
{
	if (GetLength() <= count) {
		Empty();
		return;
	}
	if (count == 0)
		return;
	Buffer (data->m_Alloc);		// Preserve buffer size
	memmove (data->m_Text, data->m_Text+count, GetLength()-count+1);
	data->m_Length = int(data->m_Length - count);
}

void CStr::RemoveMiddle(int start, int count)
{
	if (GetLength() <= start) {
		Empty();
		return;
	}
	Buffer (data->m_Alloc);		// Preserve buffer size
	char* pstart = data->m_Text + start;
	if (GetLength() <= (start+count)) {
		pstart[0] = 0;
		data->m_Length = start;
		return;
	}
	memmove (pstart, pstart+count, GetLength()-(start+count)+1);
	data->m_Length = int(data->m_Length - count);
}

void CStr::RemoveRight(int count)
{
	if (GetLength() <= count)
		Empty();
	else
		TruncateAt (int(GetLength() - count));
}


/*********************************************************************
* Proc:		CStr::FmtOneValue
* Purpose:	Helper for CStr::Format, formats one %??? item
* In:		x - ptr to the '%' sign in the specification; on exit,
*				will point to the first char after the spec.
* Out:		True if OK, False if should end formatting (but also copy
*			the remaining characters at x)
*********************************************************************/

BOOL CStr::FmtOneValue (const char*& x, va_list& marker)
{
	// Start copying format specifier to a local buffer
	char fsbuf[64];
	fsbuf[0] = '%';
	int fsp = 1;
GetMoreSpecifiers:
	// Get one character
#ifdef _DEBUG
	if (fsp >= sizeof(fsbuf)) {
		CStr::ThrowPgmError();
		return FALSE;
	}
#endif
	char ch = x[0];
	if (ch == 0)
		return FALSE;		// unexpected end of format string
	x++;
	// Chars that may exist in the format prefix
	const char fprefix[] = "-+0 #*.123456789hlL";
	if (strchr (fprefix, ch) != NULL) {
		fsbuf[fsp] = ch;
		fsp++;
		goto GetMoreSpecifiers;
	}
	// 's' is the most important parameter specifier type
	if (ch == 's') {
		// Find out how many characters should we actually print.
		//   To do this, get the string length, but also try to
		//   detect a .precision field in the format specifier prefix.
		const char* value = va_arg (marker, const char*);
		size_t slen = strlen(value);
		fsbuf[fsp] = 0;
		char* precis = strchr (fsbuf, '.');
		if (precis != NULL  &&  precis[1] != 0) {
			// Convert value after dot, put within 0 and slen
			char* endptr;
			int result = (int) strtol (precis+1, &endptr, 10);
			if (result >= 0  &&  result < int(slen))
				slen = (UINT) result;
		}
		// Copy the appropriate number of characters
		if (slen > 0)
			CoreAddChars (value, (int) slen);
		return TRUE;
	}
	// '!' is our private extension, allows direct passing of CStr*
	if (ch == '!') {
		// No precision characters taken into account here.
		const CStr* value = va_arg (marker, const CStr*);
		*this += *value;
		return TRUE;
	}
	// Chars that format an integer value
	const char intletters[] = "cCdiouxX";
	if (strchr (intletters, ch) != NULL) {
		fsbuf[fsp] = ch;
		fsbuf[fsp+1] = 0;
		char valbuf[64];
		int value = va_arg (marker, int);
		sprintf (valbuf, fsbuf, value);
		*this += valbuf;
		return TRUE;
	};
	// Chars that format a double value
	const char dblletters[] = "eEfgG";
	if (strchr (dblletters, ch) != NULL) {
		fsbuf[fsp] = ch;
		fsbuf[fsp+1] = 0;
		char valbuf[256];
		double value = va_arg (marker, double);
		sprintf (valbuf, fsbuf, value);
		*this += valbuf;
		return TRUE;
	}
	// 'Print pointer' is supported
	if (ch == 'p') {
		fsbuf[fsp] = ch;
		fsbuf[fsp+1] = 0;
		char valbuf[64];
		void* value = va_arg (marker, void*);
		sprintf (valbuf, fsbuf, value);
		*this += valbuf;
		return TRUE;
	};
	// 'store # written so far' is obscure and unsupported
	if (ch == 'n') {
		ThrowPgmError();
		return FALSE;
	}
	// 'Print unicode string' is not supported also
	if (ch == 'S') {
		ThrowNoUnicode();
		return FALSE;
	}
	// If we fall here, the character does not represent an item
	AddChar (ch);
	return TRUE;
}


/*********************************************************************
* Proc:		CStr::Format
* Purpose:	sprintf-like method
*********************************************************************/

void CStr::FormatCore (const char* x, va_list& marker)
{
	for (;;) {
		// Locate next % sign, copy chunk, and exit if no more
		LPCSTR next_p = strchr (x, '%');
		if (next_p == NULL)
			break;
		if (next_p > x)
			CoreAddChars (x, int(next_p-x));
		x = next_p+1;
		// We're at a parameter
		if (!FmtOneValue (x, marker))
			break;		// Copy rest of string and return
	}
	if (x[0] != 0)
		*this += x;
}

void CStr::Format(const char* fmt, ...)
{
	Empty();
	// Walk the string
	va_list marker;
	va_start(marker, fmt);
	FormatCore (fmt, marker);
	va_end(marker);
}
/*
void CStr::FormatRes(UINT resid, ...)
{
	Empty();
	// Get a stack-based buffer
	char buffer[512];
	if (::LoadString((HINSTANCE) get_stringres(), resid, buffer, sizeof(buffer)) == 0)
		return;
	// Walk the string
	va_list marker;
	va_start(marker, resid);
	FormatCore (buffer, marker);
	va_end(marker);
}
*/


/*********************************************************************
* Proc:		operator + (CStr and CStr, CStr and LPCSTR)
*********************************************************************/

CStr operator+(const CStr& s1, const CStr& s2)
{
	CStr out (int(s1.GetLength() + s2.GetLength()));
	out  = s1;
	out += s2;
	return out;
}

CStr operator+(const CStr& s, const char* lpsz)
{
	size_t slen = strlen(lpsz);
	CStr out (int(s.GetLength() + slen));
	out.CoreAddChars(s.data->m_Text, s.GetLength());
	out += lpsz;
	return out;
}

CStr operator+(const CStr& s, const char ch)
{
	CStr out (int(s.GetLength() + 1));
	out.CoreAddChars(s.data->m_Text, s.GetLength());
	out += ch;
	return out;
}


/*********************************************************************
* Proc:		CStr::LoadString
* Purpose:	Loads a string from the stringtable.
* In:		resid - resource ID
* Out:		True if OK, FALSE if could not find such a string
*********************************************************************/
/*
BOOL CStr::LoadString(UINT resid)
{
	HANDLE hLoad = get_stringres();
	// Try smaller resources first
	char buffer[96];
	if (::LoadString((HINSTANCE) hLoad, resid, buffer, sizeof(buffer)) == 0)
		return FALSE;
	UINT slen = strlen(buffer);
	if (slen < sizeof(buffer)-1) {
		Empty();
		CoreAddChars(buffer, (int) slen);
		return TRUE;
	}
	// We have a large string, use a big buffer
	const UINT s_big = 16384;
	char* bigbuf = (char*) CPF::MyMemAlloc (s_big);
	if (!bigbuf)
		return FALSE;
	if (::LoadString((HINSTANCE) hLoad, resid, bigbuf, s_big) == 0) {
		CPF::MyMemFree(bigbuf, s_big);
		return FALSE;
	}
	*this = (const char*) bigbuf;
	CPF::MyMemFree (bigbuf, s_big);
	return TRUE;
}
*/

/*==========================================================
* 函数名      : CStr::LTrim
* 描述        : Remove leading characters from a string.  All characters
*             : to be excluded are passed as a parameter; NULL means
*             : 'truncate spaces'
* 返回        : void 
* 参数        : const char* charset 
* 注释        : 
*=========================================================*/
void CStr::LTrim(const char* charset /*= NULL*/)
{
	int good = 0;
	if (charset) {
		while (strchr (charset, data->m_Text[good]) != NULL)
			good++;
	}
	else {
		while (data->m_Text[good] == ' ')
			good++;
	}
	if (good > 0)
		RemoveLeft (good);
}

/*==========================================================
* 函数名      : CStr::RTrim
* 描述        : Remove trailing characters; see LTrim
* 返回        : void 
* 参数        : const char* charset 
* 注释        : 
*=========================================================*/
void CStr::RTrim(const char* charset /*= NULL*/)
{
	int good = data->m_Length;
	if (good == 0)
		return;
	if (charset) {
		while (good > 0  &&  strchr (charset, data->m_Text[good-1]) != NULL)
			--good;
	}
	else {
		while (good > 0  &&  data->m_Text[good-1] == ' ')
			--good;
	}
	TruncateAt (good);		// Also works well with good == 0
}

/*==========================================================
* 函数名      : CStr::SpanInclude
* 描述        : 把所给字符串中非sz包含的字符全部清除
* 返回        : CStr 
* 参数        : const char *sz
* 注释        : 重载
*=========================================================*/
CStr CStr::SpanInclude(const char *sz) const
{
	ACE_ASSERT(sz != NULL);
	CStr str(sz);
	return SpanInclude(str);
}

/*==========================================================
* 函数名      : CStr::SpanInclude
* 描述        : 把所给字符串中非str包含的字符全部清除
* 返回        : CStr 
* 参数        : const CStr str
* 注释        : 重载
*=========================================================*/
CStr CStr::SpanInclude(const CStr str) const
{
	CStr tempstr(str);
	CStr copystr(*this);
	CStr leftstr;
	CStr rightstr;

	for( int loop = 0 ; loop < copystr.GetLength() ; loop++ )
	{
		if( tempstr.Find(copystr[loop]) == -1 )
		{
			copystr.GetLeft(loop , leftstr);
			copystr.GetRight(copystr.GetLength()-loop-1 , rightstr);
			copystr = (leftstr + rightstr);
			loop--;
		}
	}
		
	return copystr;
}

/*==========================================================
* 函数名      : CStr::SpanExclude
* 描述        : 把所给字符串中sz包含的字符全部清除
* 返回        : CStr 
* 参数        : const char *sz
* 注释        : 重载, 
*=========================================================*/
CStr CStr::SpanExclude(const char *sz) const
{
	ACE_ASSERT(sz != NULL);
	CStr str(sz);
	return SpanExclude(str);
}

/*==========================================================
* 函数名      : CStr::SpanExclude
* 描述        : 把所给字符串中str包含的字符全部清除
* 返回        : CStr 
* 参数        : const CStr str
* 注释        : 重载
*=========================================================*/
CStr CStr::SpanExclude(const CStr str) const
{
	CStr tempstr(str);
	CStr copystr(*this);
	CStr leftstr;
	CStr rightstr;

	for( int loop = 0 ; loop < copystr.GetLength() ; loop++ )
	{
		if( tempstr.Find(copystr[loop]) != -1 )
		{
			copystr.GetLeft(loop , leftstr);
			copystr.GetRight(copystr.GetLength()-loop-1 , rightstr);
			copystr = (leftstr + rightstr);
			loop--;
		}
	}
		
	return copystr;
}

/*==========================================================
* 函数名      : CStr::MakeUpper
* 描述        : 
* 返回        : void 
* 注释        : 
*=========================================================*/
void CStr::MakeUpper()
{
#ifdef OS_WINDOWS
	strcpy( data->m_Text , strupr(data->m_Text) );
#else
	strcpy( data->m_Text , CPF::str_upr(data->m_Text) );
#endif
	

	return;
}

/*==========================================================
* 函数名      : CStr::MakeLower
* 描述        : 
* 返回        : void 
* 注释        : 
*=========================================================*/
void CStr::MakeLower()
{
#ifdef OS_WINDOWS
	strcpy( data->m_Text , strlwr(data->m_Text) );
#else
	strcpy( data->m_Text , CPF::str_lwr(data->m_Text) );
#endif

	return;
}

/*==========================================================
* 函数名      : CStr::MakeReverse
* 描述        : 
* 返回        : void 
* 注释        : 
*=========================================================*/
//void CStr::MakeReverse()
//{
//	strcpy( data->m_Text , strrev(data->m_Text) );
//
//	return;
//}

/*==========================================================
* 函数名      : CStr::Replace
* 描述        : 
* 返回        : int  
* 参数        :  const char chOld
* 参数        : const char chNew
* 注释        : 
*=========================================================*/
int  CStr::Replace( const char chOld, const char chNew )
{
	//如是修改前和修改的字符相同，则什么也不发生.
	if( chOld == chNew ) return 0;
	
	int iCount = 0;//用来进行计数的
	for( int pos = 0 ; pos < data->m_Length ; pos++ )
	{
		if( data->m_Text[pos] == chOld )
		{
			data->m_Text[pos] = chNew;
			iCount++;
		}
	}

	return iCount;//note , msdn
}

void CStr::SetEndChar(const char ch)
{
	char *p1 = data->m_Text;
	int len =data->m_Length;
	char *p2 = (char*)memchr(p1,ch,len);
	if(p2)
	{
		*p2 = '\0';
		data->m_Length = (int)(p2 -p1);
	}
}


/*==========================================================
* 函数名      : CStr::Replace
* 描述        : 
* 返回        : int  
* 参数        :  const char* lpszOld
* 参数        : const char* lpszNew
* 注释        : 
*=========================================================*/
int  CStr::Replace( const char* lpszOld, const char* lpszNew )
{
	//如果两个字符相同或OLD是NEW的子串,则什么也不做
//	if( strcmp( lpszOld , lpszNew ) == 0 )
//		return 0;

	if( strlen(lpszOld) == 0 )
		return 0;

	if( strstr( lpszNew , lpszOld ) != NULL )
		return 0;

	CStr strLeft,strRight;

	int iCount = 0;
	int	iPlace;
	while(true)
	{
		if( (iPlace = Find(lpszOld)) == -1 )
		{
			return iCount;
		}
		else//发现了子串
		{
			GetLeft(iPlace , strLeft);
			GetRight( GetLength()-iPlace-(int)strlen(lpszOld) , strRight);
			*this = strLeft + lpszNew + strRight;
		}

	}

//	return iCount;//note , msdn
}

/*==========================================================
* 函数名      : CStr::FindOneOf
* 描述        : 
* 返回        : int  
* 参数        : const char* lpszCharSet
* 注释        : 
*=========================================================*/
int  CStr::FindOneOf( const char* lpszCharSet ) const
{
	//取出m_Text中的每个字符,判断是否在lpszCharSet中,一旦出现立即返回
	for( int pos = 0 ; pos < data->m_Length ; pos++ )
	{
		if( strchr(lpszCharSet , data->m_Text[pos]) != NULL )
			return pos;
	}

	return -1;//没有找到对应的项
}
