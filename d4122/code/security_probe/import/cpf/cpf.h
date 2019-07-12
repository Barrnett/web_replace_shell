#ifndef CPF_H_ZHUZP_2006_04_08
#define CPF_H_ZHUZP_2006_04_08
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"

/**********************************************************************************

���Ҫʹ��ARITH��Ϊ��̬�⣬������ʹ���ߵĹ��������Ӷ���CPF_AS_STATIC_LIBS��
������ʹ�����ͷ�ļ���ǰ���

#define CPF_AS_STATIC_LIBS

�����ARITH��ΪDLL������Ҫ���κι���
**********************************************************************************/

#if defined (CPF_AS_STATIC_LIBS)
# if !defined (CPF_HAS_DLL)
#   define CPF_HAS_DLL 0
# endif /* ! CPF_HAS_DLL */
#else
# if !defined (CPF_HAS_DLL)
#   define CPF_HAS_DLL 1
# endif /* ! CPF_HAS_DLL */
#endif /* ACE_AS_STATIC_LIB */

#if defined (CPF_HAS_DLL)
#  if (CPF_HAS_DLL == 1)
#      if defined (CPF_DLL_EXPORTS)
#        define CPF_CLASS MPLAT_Export_Flag
#      else
#        define CPF_CLASS MPLAT_Import_Flag
#      endif /* ACE_BUILD_DLL */
#  else
#    define CPF_CLASS
#  endif   /* ! CPF_HAS_DLL == 1 */
#else
#  define CPF_CLASS
#endif     /* CPF_HAS_DLL */

#define CPF_EXPORT	CPF_CLASS

namespace CPF
{
	CPF_CLASS void init(UINT heap_size=0);
	CPF_CLASS void fini(int log_leak=0);
	CPF_CLASS void set_home_charset(const char * homecharset);
	CPF_CLASS const char * get_home_charset();

	CPF_CLASS BOOL GetOSVerIs64Bit();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#endif//CPF_H_ZHUZP_2006_04_08


