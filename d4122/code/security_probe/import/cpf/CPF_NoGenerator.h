//////////////////////////////////////////////////////////////////////////
//CPF_NoGenerator.h

#pragma once

#include "cpf/cpf.h"
#include "ace/OS_NS_sys_time.h"
#include "fstream"
#include <vector>

struct CPF_CLASS CPF_No32Genrator
{
	CPF_No32Genrator(bool bsigned=true)
		:m_bsigned(bsigned)
	{
		reset();
	}

	void reset()
	{
		ACE_Time_Value curtime = ACE_OS::gettimeofday();

		m_thetime = (ACE_UINT16)(LOWORD(curtime.sec()));

		if( m_bsigned )
		{
			m_thetime &= 0x7fff;//保证最高位为0。
		}

		m_No = 0;
	}

	ACE_UINT32 next_no()
	{
		++m_No;

		return (ACE_UINT32)(MAKELONG(++m_No,m_thetime));
	}

	ACE_UINT16 get_time() const
	{
		return m_thetime;
	}

	ACE_UINT16 get_cur_no() const
	{
		return m_No;
	}

private:
	ACE_UINT16	m_thetime;
	ACE_UINT16	m_No;

private:
	bool	m_bsigned;
};



struct CPF_CLASS CPF_No64Genrator
{
	CPF_No64Genrator(bool bsigned=true)
		:m_bsigned(bsigned)
	{
		reset();
	}

	void reset()
	{
		ACE_Time_Value curtime = ACE_OS::gettimeofday();

		m_thetime = (ACE_UINT32)(curtime.sec());

		if( m_bsigned )
		{
			m_thetime &= 0x7fffffff;//保证最高位为0。
		}

		m_No = 0;
	}

	ACE_UINT64 next_no()
	{
		return (ACE_UINT64)(MAKELONGLONG(++m_No,m_thetime));
	}

	ACE_UINT32 get_time() const
	{
		return m_thetime;
	}

	ACE_UINT32 get_cur_no() const
	{
		return m_No;
	}

private:
	ACE_UINT32	m_thetime;
	ACE_UINT32	m_No;

private:
	bool	m_bsigned;
};

template<typename NO_TYPE,typename TIME_TYPE,typename GENERATOR_TOOL,typename BUFFER_TYPE>
class CPF_NoGenerator_ex
{
public:
	CPF_NoGenerator_ex(bool bsigned=true)
		:m_no_Genrator(bsigned),m_pbuffer_type(NULL)
	{

	}

public:
	void init(BUFFER_TYPE& buffer_type)
	{
		m_pbuffer_type = &buffer_type;

		m_pbuffer_type->get_timelist(m_vtTime);

		reset();

		return;
	}

	void reset()
	{
		while( 1 )
		{
			m_no_Genrator.reset();

			if( m_vtTime.end() == std::find(m_vtTime.begin(),m_vtTime.end(),m_no_Genrator.get_time()) )
			{
				break;
			}
			else
			{
				ACE_OS::sleep(1);
			}
		}

		m_pbuffer_type->append(m_no_Genrator.get_time());

		m_vtTime.push_back((ACE_UINT32)m_no_Genrator.get_time());

		return;
	}

	NO_TYPE next_no()
	{
		NO_TYPE cur_no = m_no_Genrator.next_no();

		if( m_no_Genrator.get_cur_no() == 0 )
		{
			reset();

			cur_no = m_no_Genrator.next_no();
		}

		return cur_no;
	}

private:
	std::vector<ACE_UINT32>	m_vtTime;

private:
	GENERATOR_TOOL		m_no_Genrator;
	BUFFER_TYPE*		m_pbuffer_type;
};


template<typename NO_TYPE,typename TIME_TYPE,typename GENERATOR_TOOL>
class CPF_NoGenerator_ex_file
{
//	template<typename NO_TYPE,typename TIME_TYPE>
	struct BUFFER_FILE_TYPE
	{
		void init(const char * time_filename)
		{
			m_vtTime.clear();

			if( !( time_filename && time_filename[0] ) )
			{
				ACE_ASSERT(FALSE);
				return;
			}

			m_str_TimeFileName = time_filename;

			std::ifstream 		inputFile;

			inputFile.open(m_str_TimeFileName.c_str(),std::ios::in|std::ios::binary);

			if( !inputFile.good() )
			{
				return;
			}

			inputFile.seekg(0,std::ios::end);
			int nsize = inputFile.tellg()/sizeof(TIME_TYPE);
			inputFile.seekg(0,std::ios::beg);

			m_vtTime.reserve(nsize);

			for(int i = 0; i < nsize; ++i)
			{
				ACE_UINT32 the_time;

				TIME_TYPE the_temp_time;

				inputFile.read((char *)&the_temp_time,sizeof(TIME_TYPE));

				the_time = (ACE_UINT32)the_temp_time;				

				m_vtTime.push_back(the_time);
			}

			return;
		}


		void get_timelist(std::vector<ACE_UINT32>& vtTime) const
		{
			vtTime = m_vtTime;
		}

		void append(TIME_TYPE value)
		{
			std::ofstream 		OutputFile;

			OutputFile.open(m_str_TimeFileName.c_str(),std::ios::out|std::ios::binary|std::ios::app);

			if( OutputFile.good() )
			{
				OutputFile.write((const char *)&value,sizeof(TIME_TYPE));			
			}

			OutputFile.close();
		}

	private:
		std::string					m_str_TimeFileName;
		std::vector<ACE_UINT32>		m_vtTime;

	};
public:
	CPF_NoGenerator_ex_file(bool bsigned=true)
		:m_no_Genrator(bsigned)
	{
	}

public:

	void init(const char * time_filename)
	{
		m_buffer_type.init(time_filename);

		m_no_Genrator.init(m_buffer_type);
	}

	void reset()
	{
		m_no_Genrator.reset();
	}

	NO_TYPE next_no()
	{
		return m_no_Genrator.next_no();
	}

private:
	BUFFER_FILE_TYPE	m_buffer_type;

	CPF_NoGenerator_ex<NO_TYPE, TIME_TYPE, GENERATOR_TOOL, BUFFER_FILE_TYPE >		m_no_Genrator;
};


