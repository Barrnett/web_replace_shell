//////////////////////////////////////////////////////////////////////////
//bsort_topn.h
//利用冒泡排序，得到topN(最大值）


#pragma once

#include "cpf/cpf.h"

namespace CPF
{
	//最大值在数组的尾部
	//topn必须不大于num
	CPF_CLASS void bsort_topn_asecnd( void *base, size_t num, size_t width, void * pParam,size_t topn,
				int (*compare )(const void *elem1, const void *elem2, const void * pParam) );

	//最大值在数组的头部
	//topn必须不大于num
	CPF_CLASS void bsort_topn_descend( void *base, size_t num, size_t width, void * pParam,size_t topn,
		int (*compare )(const void *elem1, const void *elem2, const void * pParam) );

}
