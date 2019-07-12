//////////////////////////////////////////////////////////////////////////
//unierm_ctrl_dll_macro.h

#ifndef UNIERM_CTRL_DLL_DLL_MACRO_H_
#define UNIERM_CTRL_DLL_DLL_MACRO_H_


#include "cpf/ostypedef.h"

#if defined(UNIERM_CTRL_DLL_EXPORTS)
#define UNIERM_CTRL_DLL_API		MPLAT_API_Export_Flag
#define UNIERM_CTRL_DLL_CLASS	MPLAT_CLASS_Export_Flag
#else
#define UNIERM_CTRL_DLL_API		MPLAT_API_Import_Flag
#define UNIERM_CTRL_DLL_CLASS	MPLAT_CLASS_Import_Flag
#endif

#endif//UNIERM_CTRL_DLL_DLL_MACRO_H_