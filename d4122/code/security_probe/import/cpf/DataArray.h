#ifndef  _DATAARRAY_H__
#define _DATAARRAY_H__

#include "cpf/cpf.h"

class CPF_CLASS CDataArray  
{
public:
	// Construction
	CDataArray();
	CDataArray(int nNewSize, int nGrowBy = -1);
	
	~CDataArray();
	
	// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);
	
	// Operations
	// Clean up
	void FreeExtra();
	void Clear();
	void RemoveAll();
	
	// Accessing elements
	UINT_PTR GetAt(int nIndex) const;
	void SetAt(int nIndex, UINT_PTR newElement);
	UINT_PTR& ElementAt(int nIndex);
	
	// Direct Access to the element data (may return NULL)
	const UINT_PTR* GetData() const;
	UINT_PTR* GetData();
	
	// Potentially growing the array
	void SetAtGrow(int nIndex, UINT_PTR newElement);
	int Add(UINT_PTR newElement);
	int Append(const CDataArray& src);
	void Copy(const CDataArray& src);
	
	// overloaded operator helpers
	UINT_PTR operator[](int nIndex) const;
	UINT_PTR& operator[](int nIndex);
	
	// Operations that move elements around
	void InsertAt(int nIndex, UINT_PTR newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CDataArray* pNewArray);
	
	// Implementation
protected:
	UINT_PTR* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount
	
};

inline int CDataArray::GetSize() const
{ return m_nSize; }

inline void CDataArray::Clear()
{ m_nSize = 0; }


inline int CDataArray::GetUpperBound() const
{ return m_nSize-1; }

inline void CDataArray::RemoveAll()
{ SetSize(0, -1); }

inline UINT_PTR CDataArray::GetAt(int nIndex) const
{ ACE_ASSERT(nIndex >= 0 && nIndex < m_nSize);
return m_pData[nIndex]; }

inline void CDataArray::SetAt(int nIndex, UINT_PTR newElement)
{ ACE_ASSERT(nIndex >= 0 && nIndex < m_nSize);
m_pData[nIndex] = newElement; }

inline UINT_PTR& CDataArray::ElementAt(int nIndex)
{ ACE_ASSERT(nIndex >= 0 && nIndex < m_nSize);
return m_pData[nIndex]; }

inline const UINT_PTR* CDataArray::GetData() const
{ return (const UINT_PTR*)m_pData; }

inline UINT_PTR* CDataArray::GetData()
{ return (UINT_PTR*)m_pData; }

inline int CDataArray::Add(UINT_PTR newElement)
{ int nIndex = m_nSize;
SetAtGrow(nIndex, newElement);
return nIndex; }

inline UINT_PTR CDataArray::operator[](int nIndex) const
{ return GetAt(nIndex); }

inline UINT_PTR& CDataArray::operator[](int nIndex)
{ return ElementAt(nIndex); }



struct BUFFERDATA
{
	char *pData;
	int len;
};

class CPF_CLASS CBufferArray  
{
public:
	// Construction
	CBufferArray();
	CBufferArray(int nNewSize, int nGrowBy = -1);
	
	~CBufferArray();
	
	// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);
	
	// Operations
	// Clean up
	void FreeExtra();
	void Clear();
	void RemoveAll();
	
	// Accessing elements
	BUFFERDATA GetAt(int nIndex) const;
	void SetAt(int nIndex, BUFFERDATA newElement);
	BUFFERDATA& ElementAt(int nIndex);
	
	// Direct Access to the element data (may return NULL)
	const BUFFERDATA* GetData() const;
	BUFFERDATA* GetData();
	
	// Potentially growing the array
	void SetAtGrow(int nIndex, BUFFERDATA newElement);
	int Add(BUFFERDATA newElement);
	int Append(const CBufferArray& src);
	void Copy(const CBufferArray& src);
	
	// overloaded operator helpers
	BUFFERDATA operator[](int nIndex) const;
	BUFFERDATA& operator[](int nIndex);
	
	// Operations that move elements around
	void InsertAt(int nIndex, BUFFERDATA newElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CBufferArray* pNewArray);
	
	// Implementation
protected:
	BUFFERDATA* m_pData;   // the actual array of data
	int m_nSize;     // # of elements (upperBound - 1)
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount
	
};

inline int CBufferArray::GetSize() const
{ return m_nSize; }

inline void CBufferArray::Clear()
{ m_nSize = 0; }


inline int CBufferArray::GetUpperBound() const
{ return m_nSize-1; }

inline void CBufferArray::RemoveAll()
{ SetSize(0, -1); }

inline BUFFERDATA CBufferArray::GetAt(int nIndex) const
{ ACE_ASSERT(nIndex >= 0 && nIndex < m_nSize);
return m_pData[nIndex]; }

inline void CBufferArray::SetAt(int nIndex, BUFFERDATA newElement)
{ ACE_ASSERT(nIndex >= 0 && nIndex < m_nSize);
m_pData[nIndex] = newElement; }

inline BUFFERDATA& CBufferArray::ElementAt(int nIndex)
{ ACE_ASSERT(nIndex >= 0 && nIndex < m_nSize);
return m_pData[nIndex]; }

inline const BUFFERDATA* CBufferArray::GetData() const
{ return (const BUFFERDATA*)m_pData; }

inline BUFFERDATA* CBufferArray::GetData()
{ return (BUFFERDATA*)m_pData; }

inline int CBufferArray::Add(BUFFERDATA newElement)
{ int nIndex = m_nSize;
SetAtGrow(nIndex, newElement);
return nIndex; }

inline BUFFERDATA CBufferArray::operator[](int nIndex) const
{ return GetAt(nIndex); }

inline BUFFERDATA& CBufferArray::operator[](int nIndex)
{ return ElementAt(nIndex); }

#endif //_DATAARRAY_H__

