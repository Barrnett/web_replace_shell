///////////////////////////////////////////////////////////////////////////
//Common_Func_Macor.h

#pragma once

#include "cpf/ostypedef.h"

//获取数据元素的个数。注意a必须是定义为数组的形式。
#define GET_NUMS_OF_ARRAY(a)	(sizeof(a)/sizeof((a)[0]))

#define CPF_UINT_TEST_BIT(x,i)		((x)&((UINT)1<<(i)))
#define CPF_UINT_SET_BIT(x,i)		((x)|=((UINT)1<<(i)))
#define CPF_UINT_CLR_BIT(x,i)		((x)&=(~((UINT)1<<(i))))

#define CPF_UINT64_TEST_BIT(x,i)		((x)&((ACE_UINT64)1<<(i)))
#define CPF_UINT64_SET_BIT(x,i)			((x)|=((ACE_UINT64)1<<(i)))
#define CPF_UINT64_CLR_BIT(x,i)			((x)&=(~((ACE_UINT64)1<<(i))))


//注意total必须是整数类型

#define CPF_GET_INT32_RATE(data,total)		(ACE_INT32)(((total)==0)?0:(((ACE_INT64)(data))*100/(total)))
#define CPF_GET_UINT32_RATE(data,total)		(ACE_UINT32)(((total)==0)?0:(((ACE_UINT64)(data))*100/(total)))
#define CPF_GET_INT64_RATE(data,total)		(ACE_INT64)(((total)==0)?0:(((ACE_INT64)(data))*100/(total)))
#define CPF_GET_UINT64_RATE(data,total)		(ACE_UINT64)(((total)==0)?0:(((ACE_UINT64)(data))*100/(total)))

#define CPF_GET_DOUBLE_RATE(data,total)	    (double)((((total)==0)?0:((((double)(data)))*100/(total))))

//注意total可以不是整数类型
#define CPF_GET_INT32_NUMS_BY_RATE(rate,total)	(ACE_INT32)((ACE_INT64(rate))*(total)/100)
#define CPF_GET_UINT32_NUMS_BY_RATE(rate,total)	(ACE_UINT32)((ACE_UINT64(rate))*(total)/100)
#define CPF_GET_INT64_NUMS_BY_RATE(rate,total)	(ACE_INT64)((ACE_INT64(rate))*(total)/100)
#define CPF_GET_UINT64_NUMS_BY_RATE(rate,total)	(ACE_UINT64)((ACE_UINT64(rate))*(total)/100)

#define CPF_GET_DOUBLE_NUMS_BY_RATE(rate,total)	(double)((double(rate))*(total)/100)

//strncpy的实现，将最后一个字符为0
//len表示字符个数
#define SAFE_strncpyA(dst,src,len) {memcpy((void *)(dst),(const void *)(src),(len));((char *)(dst))[(len)-1]=0;}

//如果目标是一个字符数组,使用下面的宏更简单
#define SAFE_ARRAY_strncpyA(dst,src)	SAFE_strncpyA((dst),(src),sizeof(dst))

//strncpy的实现，将最后一个字符为0
//len表示字符个数
#define SAFE_strncpyW(dst,src,len) {memcpy((void *)(dst),(const void *)(src),(len)*sizeof(wchar_t));((wchar_t *)(dst))[(len)-1]=0;}

//如果目标是一个字符数组,使用下面的宏更简单
#define SAFE_ARRAY_strncpyW(dst,src)	SAFE_strncpyW((dst),(src),sizeof(dst)/sizeof(wchar_t))

//strncpy的实现，将最后一个字符为0
//len表示字符个数
#define SAFE_strncpy(dst,src,len) {memcpy((void *)(dst),(const void *)(src),(len)*sizeof(ACE_TCHAR));((TCHAR *)(dst))[(len)-1]=0;}

//如果目标是一个字符数组,使用下面的宏更简单
#define SAFE_ARRAY_strncpy(dst,src)	SAFE_strncpy((dst),(src),sizeof(dst)/sizeof(ACE_TCHAR))

#define NOT_EMPTY_STR(mystr) ( (mystr) && ((mystr)[0]) )
#define IS_EMPTY_STR(mystr)  (!(NOT_EMPTY_STR(mystr)))

//字节对齐

//GET_BYTE_ALIGN(32,4)=32
//GET_BYTE_ALIGN(31,4)=32

//GET_BYTE_ALIGN(20,4)=20
//GET_BYTE_ALIGN(20,8)=24

//len表示需要转换的大小,align表示需要对齐的字节数,align必须是2的n次方

#define SIZE0F_POINTER		sizeof(void *)

#define GET_BYTE_ALIGN(len,align) \
	( ((len)+(align)-1) & (~((align)-1)) )


//判断size是否是2的N次方
#define IS_POWER_2(size) \
	( ( (~(size)+1) & (size) ) == (size) )


//以一个int的大小来对齐
#define INT32_ALIGN(len) GET_BYTE_ALIGN(len,sizeof(int))

#define INT64_ALIGN(len) GET_BYTE_ALIGN(len,sizeof(ULONGLONG))

#define INT_PTR_ALLIGN(len)	GET_BYTE_ALIGN(len,SIZE0F_POINTER)

#define IS_ALLIGN(x,n)		( ( ((ULONG_PTR)(x)) & ((n)-1) ) == 0L )
#define IS_ALLIGN_PTR(x)	( IS_ALLIGN((x),SIZE0F_POINTER) )


//计算时间的差值，如果新时间小于以前的时间，返回0，并且把旧时间设置成新时间

#define DIFF_TIME_EX(old_time,new_time)	(((new_time)>=(old_time))?(new_time)-(old_time):(old_time=(new_time),((new_time)-old_time)))

#define DIFF_TIME_STAMP_SEC_EX(old_time,new_time)	(((new_time).GetSEC()>=old_time.GetSEC())?(new_time).GetSEC()-old_time.GetSEC():(old_time=(new_time),(new_time).GetSEC()-old_time.GetSEC()))

#define DIFF_TIME_VALUE_SEC_EX(old_time,new_time)	(((new_time).sec()>=old_time.sec())?(new_time).sec()-old_time.sec():(old_time=(new_time),(new_time).sec()-old_time.sec()))

