//////////////////////////////////////////////////////////////////////////
//cpf_std.h

#pragma once

#include <vector>
#include <list>
#include "ostypedef.h"

namespace CPF
{
	template<typename VALUE_TYPE>
	inline
		BOOL find(const std::vector<VALUE_TYPE>& vt_data,const VALUE_TYPE& data,BOOL bNot=false)
	{
		BOOL brtn = false;

		for(size_t i = 0;i < vt_data.size(); ++i)
		{
			if( vt_data[i] == data )
			{
				brtn = true;
				break;
			}
		}

		return (bNot?(!brtn):(brtn));
	}

	template<typename VALUE_TYPE>
	inline
		BOOL find(const std::list<VALUE_TYPE>& vt_data,const VALUE_TYPE& data,BOOL bNot=false)
	{
		BOOL brtn = false;

		typename std::list< VALUE_TYPE >::const_iterator it;

		for(it = vt_data.begin(); it != vt_data.end(); ++it)
		{
			if( *it == data )
			{
				brtn = true;
				break;
			}
		}

		return (bNot?(!brtn):(brtn));
	}

	template<class Valuetype>
	inline
	BOOL remove_at(Valuetype& data, int index)
	{	
		for(typename Valuetype::iterator it  = data.begin();
			it != data.end(); 
			++it,--index )
		{
			if( 0 == index )
			{
				data.erase(it);
				return true;
			}
		}

		return false;
	}

	template<typename VALUE_TYPE>
	inline 
	BOOL range_find(const std::vector< std::pair<VALUE_TYPE,VALUE_TYPE> >& vt_data,const VALUE_TYPE& data)
	{
		for(size_t i = 0; i < vt_data.size(); ++i)
		{
			if( vt_data[i].first <= data && vt_data[i].second >= data )
			{
				return true;
			}
		}

		return false;		
	}

	template<typename VALUE_TYPE>
	inline 
	BOOL range_find(const std::list< std::pair<VALUE_TYPE,VALUE_TYPE> >& vt_data,const VALUE_TYPE& data)
	{
		typename std::list< std::pair<VALUE_TYPE,VALUE_TYPE> >::const_iterator it;

		for(it = vt_data.begin(); it != vt_data.end(); ++it)
		{
			if( it->first <= data && it->second >= data )
			{
				return true;
			}
		}

		return false;		
	}

	template<typename VALUE_TYPE>
	inline 
	BOOL flt_range_find(const std::vector< std::pair<VALUE_TYPE,VALUE_TYPE> >& vt_data,const VALUE_TYPE& data,BOOL bNot=false)
	{
		if( vt_data.size() == 0 )
			return true;

		BOOL brtn = range_find(vt_data,data);

		return (bNot?(!brtn):(brtn));
	}

	template<typename VALUE_TYPE>
	inline 
	BOOL flt_range_find(const std::list< std::pair<VALUE_TYPE,VALUE_TYPE> >& vt_data,const VALUE_TYPE& data,BOOL bNot=false)
	{
		if( vt_data.size() == 0 )
			return true;

		BOOL brtn = range_find(vt_data,data);

		return (bNot?(!brtn):(brtn));
	}

	template<typename VALUE_TYPE>
	inline
	BOOL flt_find(const std::vector<VALUE_TYPE>& vt_data,const VALUE_TYPE& data,BOOL bNot=false)
	{
		if( vt_data.size() == 0 )
			return true;

		return find(vt_data,data,bNot);
	}

	template<typename VALUE_TYPE>
	inline
		BOOL flt_find(const std::list<VALUE_TYPE>& vt_data,const VALUE_TYPE& data,BOOL bNot=false)
	{
		if( vt_data.size() == 0 )
			return true;

		return find(vt_data,data,bNot);
	}



	//typedef enum
	//{
	//	flt_dir_uncare = 0,
	//	flt_addr1_to_addr2 = 1,
	//	flt_addr2_to_addr1 = -1,

	//}CPF_FILTER_DIR;

	template<typename VALUE_TYPE>
	inline 
	BOOL flt_range_find_two_side(int dir,
		const std::vector< std::pair<VALUE_TYPE,VALUE_TYPE> >& vt_data1,const VALUE_TYPE& data1,BOOL bNot1,
		const std::vector< std::pair<VALUE_TYPE,VALUE_TYPE> >& vt_data2,const VALUE_TYPE& data2,BOOL bNot2)
	{
		if( vt_data1.size() == 0 )
			bNot1 = false;

		if( vt_data2.size() == 0 )
			bNot2 = false;

		if( dir == 0 )
		{
			if( !bNot1 && !bNot2 )
			{
				return ( (flt_range_find(vt_data1,data1,false)&&flt_range_find(vt_data2,data2,false))
					|| (flt_range_find(vt_data1,data2,false)&&flt_range_find(vt_data2,data1,false)) );
			}
			else if( bNot1 && bNot2 )
			{
				return ((flt_range_find(vt_data1,data1,bNot1)||flt_range_find(vt_data1,data2,bNot1))
					&& (flt_range_find(vt_data2,data1,bNot2)||flt_range_find(vt_data2,data2,bNot2)) );
				
			}
			else
			{
				return ( !( (flt_range_find(vt_data1,data1,false)&&flt_range_find(vt_data2,data2,false))
					|| (flt_range_find(vt_data1,data2,false)&&flt_range_find(vt_data2,data1,false)) ) );
			}
		}
		else if( dir == 1 )
		{
			return flt_range_find(vt_data1,data1,bNot1)&&flt_range_find(vt_data2,data2,bNot2);

		}
		else if( dir == -1 )
		{
			return flt_range_find(vt_data1,data2,bNot1)&&flt_range_find(vt_data2,data1,bNot2);
		}
		else
		{
			ACE_ASSERT(FALSE);
			return false;
		}
	}


	template<typename VALUE_TYPE>
	inline
		BOOL flt_find_two_side(int dir,
		const std::vector<VALUE_TYPE>& vt_data1,const VALUE_TYPE& data1,BOOL bNot1,
		const std::vector<VALUE_TYPE>& vt_data2,const VALUE_TYPE& data2,BOOL bNot2)
	{
		if( vt_data1.size() == 0 )
			bNot1 = false;

		if( vt_data2.size() == 0 )
			bNot2 = false;

		if( dir == 0 )
		{
			BOOL temp1 = (flt_find(vt_data1,data1,false)||flt_find(vt_data1,data2,false));

			if( bNot1 )
				temp1 = !temp1;

			BOOL temp2 = (flt_find(vt_data2,data1,false)||flt_find(vt_data2,data2,false));

			if( bNot2 )
				temp2 = !temp2;

			return (temp1&&temp2);
		}
		else if( dir == 1 )
		{
			return flt_find(vt_data1,data1,bNot1)&&flt_find(vt_data2,data2,bNot2);

		}
		else if( dir == -1 )
		{
			return flt_find(vt_data1,data2,bNot1)&&flt_find(vt_data2,data1,bNot2);
		}
		else
		{
			ACE_ASSERT(FALSE);
			return false;
		}
	}

	//注意vector应该里面没有重复的数据
	template<typename VALUE_TYPE>
	inline 
		BOOL advance_compare(const std::vector< VALUE_TYPE >& vt_data1,const std::vector< VALUE_TYPE >& vt_data2)
	{
		if( vt_data1.size() != vt_data2.size() )
			return false;

		for(size_t i = 0; i < vt_data1.size(); ++i)
		{
			if( !CPF::find(vt_data2,vt_data1[i],false) )
				return false;
		}

		return true;
	}

	//注意list应该里面没有重复的数据
	template<typename VALUE_TYPE>
	inline 
		BOOL advance_compare(const std::list< VALUE_TYPE >& vt_data1,const std::list< VALUE_TYPE >& vt_data2)
	{
		if( vt_data1.size() != vt_data2.size() )
			return false;

		typename std::list< VALUE_TYPE >::const_iterator it;

		for(it = vt_data1.begin(); it != vt_data1.end(); ++it)
		{
			if( !CPF::find(vt_data2,*it,false) )
				return false;
		}

		return true;
	}
}

