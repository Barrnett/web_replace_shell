//////////////////////////////////////////////////////////////////////////
//web_host_Finder.h

#pragma once

#include "cpf2/cpf2.h"
#include "cpf2/CBlurKeyMatchSet.h"

//用于host或者url等的查找，支持模糊匹配
//支持 abcd*efg，或者abcd#efg,或者 abcd|efg

class CPF2_CLASS Cweb_host_Finder
{
public:
	Cweb_host_Finder(void);
	~Cweb_host_Finder(void);

public:
	class Host_Cmp_Item
	{
	public:
		Host_Cmp_Item()
		{
			match_type = 0;
			data = 0;
		}

		Host_Cmp_Item& operator = (const Host_Cmp_Item& other)
		{
			if(this != &other )
			{
				hostname = other.hostname;
				match_type = other.match_type;
				data = other.data;
				
			}

			return *this;
		}

		int operator == (const Host_Cmp_Item& other) const
		{
			return (hostname==other.hostname
				&& match_type == other.match_type
				&& data == other.data);
		}

		int operator != (const Host_Cmp_Item& other) const
		{
			return (!(*this == other));
		} 

	public:
		std::string		hostname;
		int				match_type;//匹配模式：0表示模糊匹配，1表示精确匹配
		ULONG_PTR		data;

	};

private:
	void CloseFinder();

private:
	CBlurKeyMatchSet< ULONG_PTR,VECTOR_GROUP_TYPE<ULONG_PTR> > *	m_fuzzy_matching_www_url_finder;
	CBlurKeyMatchSet< ULONG_PTR,VECTOR_GROUP_TYPE<ULONG_PTR> > *	m_fuzzy_matching_nowww_url_finder;

	CBlurKeyMatchSet< ULONG_PTR,VECTOR_GROUP_TYPE<ULONG_PTR> > *	m_exact_matching_www_url_finder;
	CBlurKeyMatchSet< ULONG_PTR,VECTOR_GROUP_TYPE<ULONG_PTR> > *	m_exact_matching_nowww_url_finder;

public:
	void begin_add_host();
	BOOL add_host(int match_type,const char * hostname,ULONG_PTR data_for_host,int lwr_key);
	int end_add_host();

public:
	ULONG_PTR find_host(const char * hostname,int hostname_len) const;

	BOOL find_host(const char * hostname,int hostname_len,
		VECTOR_GROUP_TYPE<ULONG_PTR>& flt_grp) const;

public:
	inline void SetData(ULONG_PTR data)
	{
		m_data = data;
	}

	inline ULONG_PTR GetData() const
	{
		return m_data;
	}

private:	
	void add_web_host(const std::vector<Host_Cmp_Item>& vt_invalid_urls);

private:
	std::vector<Host_Cmp_Item>	m_temp_host_array;

private:
	ULONG_PTR	m_data;

};

typedef Cweb_host_Finder * LP_Cweb_host_Finder;


