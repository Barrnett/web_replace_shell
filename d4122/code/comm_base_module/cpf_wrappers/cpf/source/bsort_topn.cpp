//////////////////////////////////////////////////////////////////////////
//bsort_topn.h

#include "cpf/bsort_topn.h"

#ifdef OS_WINDOWS
#pragma optimize("t", on)
#endif

static void swap(char *p, char *q, size_t width);

void CPF::bsort_topn_asecnd( void *base, size_t num, size_t width, void * pParam,size_t topn,
				int (*compare )(const void *elem1, const void *elem2, const void * pParam) )
{
	int bchange = true;

	if( topn > num )
	{
		topn = num;
	}

	char * pdata_base = (char *)base;

	char * pend = (pdata_base) + (num-1)*width;

	while(pend>pdata_base && topn && bchange)
	{
		bchange = false;

		char * pdata_i = (char *)pdata_base;

		while(pdata_i < pend)
		{
			char * pdata_j = pdata_i + width;

			if( compare(pdata_i,pdata_j,pParam) > 0 )
			{
				swap(pdata_i,pdata_j,width);

				bchange = true;
			}

			pdata_i = pdata_j;		
		}

		--topn;

		pend -= width;
	}

	return;
}


void CPF::bsort_topn_descend( void *base, size_t num, size_t width, void * pParam,size_t topn,
							int (*compare )(const void *elem1, const void *elem2, const void * pParam) )
{
	int bchange = true;

	if( topn > num )
	{
		topn = num;
	}

	char * pdata_base = (char *)base;

	char * pend = (pdata_base) + (num-1)*width;

	while(pend>pdata_base && topn && bchange)
	{
		bchange = false;

		char * pdata_i = (char *)pend;

		while(pdata_i > pdata_base)
		{
			char * pdata_j = pdata_i - width;

			if( compare(pdata_i,pdata_j,pParam) > 0 )
			{
				swap(pdata_i,pdata_j,width);

				bchange = true;
			}

			pdata_i = pdata_j;
		}

		--topn;

		pdata_base += width;
	}

	return;
}


/***
*swap(a, b, width) - swap two elements
*
*Purpose:
*       swaps the two array elements of size width
*
*Entry:
*       char *a, *b = pointer to two elements to swap
*       size_t width = width in bytes of each array element
*
*Exit:
*       returns void
*
*Exceptions:
*
*******************************************************************************/




static void swap (
						  char *a,
						  char *b,
						  size_t width
						  )
{

	if ( a != b )
	{
		char tmp;

		/* Do the swap one character at a time to avoid potential alignment
		problems. */
		while ( width-- ) 
		{
			tmp = *a;
			*a++ = *b;
			*b++ = tmp;
		}
	}

	return;
}


