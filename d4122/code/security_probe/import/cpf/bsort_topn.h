//////////////////////////////////////////////////////////////////////////
//bsort_topn.h
//����ð�����򣬵õ�topN(���ֵ��


#pragma once

#include "cpf/cpf.h"

namespace CPF
{
	//���ֵ�������β��
	//topn���벻����num
	CPF_CLASS void bsort_topn_asecnd( void *base, size_t num, size_t width, void * pParam,size_t topn,
				int (*compare )(const void *elem1, const void *elem2, const void * pParam) );

	//���ֵ�������ͷ��
	//topn���벻����num
	CPF_CLASS void bsort_topn_descend( void *base, size_t num, size_t width, void * pParam,size_t topn,
		int (*compare )(const void *elem1, const void *elem2, const void * pParam) );

}
