//////////////////////////////////////////////////////////////////////////
//strtools.cpp

#include "cpf/strtools.h"

//---------------------------------------------*/

#include <string.h>


//同一字段各项内容之间的间隔符的转义符
char g_item_sep(';');

//同一字段各项内容之间的间隔符的转义符
wchar_t g_w_item_sep(L';');

//同一项个子项之间的间隔符(char)
char g_subitem_sep('|');

//同一项个子项之间的间隔符(char)
wchar_t g_w_subitem_sep(L'|');


namespace CPF
{
//查找字符串
int  FindingString(const char* lpszSour, const char* lpszFind, int nStart = 0);
}

//功  能：在lpszSour中查找字符串lpszFind，lpszFind中可以包含通配字符‘?’
//参  数：nStart为在lpszSour中的起始查找位置
//返回值：成功返回匹配位置，否则返回-1
//注  意：Called by “bool MatchingString()”
int CPF::FindingString(const char* lpszSour, const char* lpszFind, int nStart /* = 0 */)
{
	//	ASSERT(lpszSour && lpszFind && nStart >= 0);
	if(lpszSour == NULL || lpszFind == NULL || nStart < 0)
		return -1;

	int m = (int)strlen(lpszSour);
	int n = (int)strlen(lpszFind);

	if( nStart+n > m )
		return -1;

	if(n == 0)
		return nStart;

	//KMP算法
	int* next = new int[n];

	//得到查找字符串的next数组
	{	
		n--;
		
		int j, k;
		j = 0;
		k = -1;
		next[0] = -1;
		
		while(j < n)
		{	
			if(k == -1 || lpszFind[k] == '?' || lpszFind[j] == lpszFind[k])
			{	
				j++;
				k++;
				next[j] = k;
			}
			else
			{
				k = next[k];
			}
		}
		
		n++;
	}

	int i = nStart, j = 0;
	while(i < m && j < n)
	{
		if(j == -1 || lpszFind[j] == '?' || lpszSour[i] == lpszFind[j])
		{	
			i++;
			j++;
		}
		else
		{
			j = next[j];
		}
	}

	delete []next;

	if(j >= n)
		return i-n;
	else
		return -1;
}

//功	  能：带通配符的字符串匹配
//参	  数：lpszSour是一个普通字符串；
//			  lpszMatch是一可以包含通配符的字符串；
//			  bMatchCase为0，不区分大小写，否则区分大小写。
//返  回  值：匹配，返回1；否则返回0。
//通配符意义：
//		‘*’	代表任意字符串，包括空字符串；
//		‘?’	代表任意一个字符，不能为空；
//时	  间：	2001.11.02	13:00
bool CPF::MatchingString(const char* lpszSour, const char* lpszMatch, bool bMatchCase /*  = true */)
{
	//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	if(lpszMatch[0] == 0)//Is a empty string
	{
		if(lpszSour[0] == 0)
			return true;
		else
			return false;
	}

	int i = 0, j = 0;

	//生成比较用临时源字符串'szSource'
	char* szSource =
		new char[ (j = (int)strlen(lpszSour)+1) ];

	if( bMatchCase )
	{	//memcpy(szSource, lpszSour, j);
		while(( *(szSource+i) = *(lpszSour+i++) ));
	}
	else
	{	//Lowercase 'lpszSour' to 'szSource'
		i = 0;
		while(lpszSour[i])
		{	
			if(lpszSour[i] >= 'A' && lpszSour[i] <= 'Z')
				szSource[i] = lpszSour[i] - 'A' + 'a';
			else
				szSource[i] = lpszSour[i];
			
			i++;
		}

		szSource[i] = 0;
	}

	//生成比较用临时匹配字符串'szMatcher'
	char* szMatcher = new char[strlen(lpszMatch)+1];

	//把lpszMatch里面连续的“*”并成一个“*”后复制到szMatcher中
	i = j = 0;
	while(lpszMatch[i])
	{
		szMatcher[j++] = (!bMatchCase) ?
			( (lpszMatch[i] >= 'A' && lpszMatch[i] <= 'Z') ?//Lowercase lpszMatch[i] to szMatcher[j]
			lpszMatch[i] - 'A' + 'a' :
		lpszMatch[i]
		) :
		lpszMatch[i];		 //Copy lpszMatch[i] to szMatcher[j]
		//Merge '*'
		if(lpszMatch[i] == '*')
			while(lpszMatch[++i] == '*');
		else
			i++;
	}
	szMatcher[j] = 0;

	//开始进行匹配检查

	int nMatchOffset, nSourOffset;

	bool bIsMatched = true;
	nMatchOffset = nSourOffset = 0;
	while(szMatcher[nMatchOffset])
	{
		if(szMatcher[nMatchOffset] == '*')
		{
			if(szMatcher[nMatchOffset+1] == 0)
			{	//szMatcher[nMatchOffset]是最后一个字符

				bIsMatched = true;
				break;
			}
			else
			{	//szMatcher[nMatchOffset+1]只能是'?'或普通字符

				int nSubOffset = nMatchOffset+1;

				while(szMatcher[nSubOffset])
				{	
					if(szMatcher[nSubOffset] == '*')
						break;
					nSubOffset++;
				}

				if( strlen(szSource+nSourOffset) <	size_t(nSubOffset-nMatchOffset-1) )
				{	//源字符串剩下的长度小于匹配串剩下要求长度
					bIsMatched = false; //判定不匹配
					break;			//退出
				}

				if(!szMatcher[nSubOffset])//nSubOffset is point to ender of 'szMatcher'
				{	//检查剩下部分字符是否一一匹配

					nSubOffset--;
					int nTempSourOffset = (int)strlen(szSource)-1;
					//从后向前进行匹配
					while(szMatcher[nSubOffset] != '*')
					{
						if(szMatcher[nSubOffset] == '?')
						{
							;
						}
						else
						{	
							if(szMatcher[nSubOffset] != szSource[nTempSourOffset])
							{	
								bIsMatched = false;
								break;
							}
						}

						nSubOffset--;
						nTempSourOffset--;
					}

					break;
				}
				else//szMatcher[nSubOffset] == '*'
				{	
					nSubOffset -= nMatchOffset;

					char* szTempFinder = new char[nSubOffset];

					nSubOffset--;
					memcpy(szTempFinder, szMatcher+nMatchOffset+1, nSubOffset);
					szTempFinder[nSubOffset] = 0;

					int nPos = CPF::FindingString(szSource+nSourOffset, szTempFinder, 0);
					delete []szTempFinder;

					if(nPos != -1)//在'szSource+nSourOffset'中找到szTempFinder
					{	
						nMatchOffset += nSubOffset;
						nSourOffset += (nPos+nSubOffset-1);
					}
					else
					{	
						bIsMatched = false;
						break;
					}
				}
			}
		}		//end of "if(szMatcher[nMatchOffset] == '*')"
		else if(szMatcher[nMatchOffset] == '?')
		{
			if(!szSource[nSourOffset])
			{	
				bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	//如果szMatcher[nMatchOffset]是最后一个字符，
				//且szSource[nSourOffset]不是最后一个字符
				bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
		else//szMatcher[nMatchOffset]为常规字符
		{
			if(szSource[nSourOffset] != szMatcher[nMatchOffset])
			{	
				bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	
				bIsMatched = false;
				break;
			}

			nMatchOffset++;
			nSourOffset++;
		}
	}

	delete []szSource;
	delete []szMatcher;
	return bIsMatched;
}

//功  能：多重匹配，不同匹配字符串之间用‘sep’隔开
//			如：“*.h,*.cpp”将依次匹配“*.h”和“*.cpp”
//参  数：nMatchLogic = 0, 不同匹配求或，else求与；bMatchCase, 是否大小敏感
//返回值：如果bRetReversed = 0, 匹配返回true；否则不匹配返回true
//时  间：2001.11.02  17:00
bool CPF::MultiMatching(const char* lpszSour, const char* lpszMatch, int nMatchLogic /* = 0 */, bool bRetReversed /* = 0 */, bool bMatchCase /* = true */,char sep/*=','*/)
{
	//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;

	char* szSubMatch = new char[strlen(lpszMatch)+1];
	bool bIsMatch;

	if(nMatchLogic == 0)//求或
	{	
		bIsMatch = 0;
		int i = 0;
		int j = 0;
		while(1)
		{	
			if(lpszMatch[i] != 0 && lpszMatch[i] != ',')
			{
				szSubMatch[j++] = lpszMatch[i];
			}
			else
			{	
				szSubMatch[j] = 0;
				if(j != 0)
				{
					bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
					if(bIsMatch)
						break;
				}
				j = 0;
			}
			
			if(lpszMatch[i] == 0)
				break;
			i++;
		}
	}
	else//求与
	{	
		bIsMatch = 1;
		
		int i = 0;
		int j = 0;
		
		while(1)
		{
			if(lpszMatch[i] != 0 && lpszMatch[i] != sep)
			{
				szSubMatch[j++] = lpszMatch[i];
			}
			else
			{	
				szSubMatch[j] = 0;
				
				bIsMatch = MatchingString(lpszSour, szSubMatch, bMatchCase);
				
				if(!bIsMatch)
					break;
				
				j = 0;
			}
			
			if(lpszMatch[i] == 0)
				break;
			i++;
		}
	}

	delete []szSubMatch;

	if(bRetReversed)
		return !bIsMatch;
	else
		return bIsMatch;
}

/*------------------------------------------------------------------------
Function:	const char * ESCString(const char * data,char sep, char esc_char)

Purpose:	将char类型的字符串进行转义

Parameters:	data 	- 	[in]要转义的字符串。

Returns:	转义后的char类型字符串

Author:		刘会华, 2006-11-23

notice:		目前转义字符串的最大长度为10240字节，长度大于10240部分舍弃

如果没有发生转义，则返回data,output内容没有任何修改，
如果发生了转移，否则返回output，output是修改后的内容
------------------------------------------------------------------------*/
CPF_CLASS const char * CPF::ESCString(const char * data,char * output1,size_t output_len,char sep, char esc_char)
{
	int pos_dst = 0;

	const char * text = data;

	while(*text)
	{
		if( *text == esc_char || *text == sep )
		{
			int cpylen = mymin((int)(text-data),(int)output_len -2-pos_dst);

			memcpy(output1+pos_dst,data,cpylen);

			if(pos_dst + cpylen + 2 >= (int)output_len )
			{
				*(output1 + pos_dst+cpylen) = 0;
				break;
			}
			else
			{
				*(output1 + pos_dst+cpylen) = esc_char;
				*(output1 + pos_dst+cpylen+1) = *text;

				pos_dst += cpylen+2;
			}

			data = text+1;
		}

		++text;
	}

	if( pos_dst == 0 )
	{
		return data;
	}
	else
	{
		if( pos_dst != output_len )
		{
			int cpylen = mymin((int)(text-data),(int)output_len -1-pos_dst);

			memcpy(output1+pos_dst,data,cpylen);

			output1[pos_dst+cpylen] = 0;
		}

		return output1;
	}
}

//指定时字符个数的空间
CPF_CLASS const wchar_t * CPF::ESCString(const wchar_t * data,wchar_t * output1,size_t output_len,wchar_t sep, wchar_t esc_char)
{
	int pos_dst = 0;

	const wchar_t * text = data;

	while(*text)
	{
		if( *text == esc_char || *text == sep )
		{
			int cpylen = mymin((int)(text-data),(int)output_len -2-pos_dst);

			memcpy(output1+pos_dst,data,cpylen*sizeof(wchar_t));

			if(pos_dst + cpylen + 2 >= (int)output_len )
			{
				*(output1 + pos_dst+cpylen) = 0;
				break;
			}
			else
			{
				*(output1 + pos_dst+cpylen) = esc_char;
				*(output1 + pos_dst+cpylen+1) = *text;

				pos_dst += cpylen+2;
			}

			data = text+1;
		}

		++text;
	}

	if( pos_dst == 0 )
	{
		return data;
	}
	else
	{
		if( pos_dst != output_len )
		{
			int cpylen = mymin((int)(text-data),(int)output_len -1-pos_dst);

			memcpy(output1+pos_dst,data,cpylen*sizeof(wchar_t));

			output1[pos_dst+cpylen] = 0;
		}

		return output1;
	}
}

const char * CPF::ESCItemAndSubItemString(const char * data)
{
	static char output1[10240];
	static char output2[10240];

	const char * pout = ESCStringSubItemSep(data,output1,sizeof(output1));

	return ESCStringItemSep(pout,output2,sizeof(output2));
}

const char * CPF::ESCStringItemSep(const char * data,char * output1,size_t output_len)
{
	return ESCString(data,output1,output_len,g_item_sep);
}

const char * CPF::ESCStringSubItemSep(const char * data,char * output1,size_t output_len)
{
	return ESCString(data,output1,output_len,g_subitem_sep);
}

const wchar_t * CPF::ESCItemAndSubItemString(const wchar_t * data)
{
	static wchar_t output1[10240];
	static wchar_t output2[10240];

	const wchar_t * pout = ESCStringSubItemSep(data,output1,sizeof(output1));

	return ESCStringItemSep(pout,output2,sizeof(output2));
}

const wchar_t * CPF::ESCStringItemSep(const wchar_t * data,wchar_t * output1,size_t output_len)
{
	return ESCString(data,output1,output_len,g_w_item_sep);

}
const wchar_t * CPF::ESCStringSubItemSep(const wchar_t * data,wchar_t * output1,size_t output_len)
{
	return ESCString(data,output1,output_len,g_w_subitem_sep);
}


const char* CPF::memnmem(const char *s1, const char *s2, size_t len1, size_t len2)
{
	if (len2 > len1)
		return 0;

	size_t const len = len1 - len2;
	for (size_t i = 0; i <= len; i++)
	{
		if (ACE_OS::memcmp (s1 + i, s2, len2) == 0)
		{
			// Found a match!  Return the index.
			return s1 + i;
		}
	}

	return 0;
}

const char* CPF::memnstr(const char *s1, const char *s2, size_t len1)
{
	size_t len2 = ACE_OS::strlen(s2);
	return CPF::memnmem(s1, s2, len1, len2);
}
