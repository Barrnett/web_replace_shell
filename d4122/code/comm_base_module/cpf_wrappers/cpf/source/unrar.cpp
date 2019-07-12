//////////////////////////////////////////////////////////////////////////
//unrar.cpp

#include "cpf/unrar.h"

#ifdef OS_WINDOWS
//{

#ifdef _UNICODE
#define _ArcName ArcNameW
#define STR_RARProcessFile TEXT("RARProcessFileW")
#else
#define _ArcName ArcName
#define STR_RARProcessFile TEXT("RARProcessFile")
#endif

HRESULT RARX(PTSTR ptzSrcFile, PTSTR ptzDstDir)
{
	struct RAROpenArchiveDataEx od = {0};
	od._ArcName = ptzSrcFile;
	od.CmtBufSize = 16384;
	od.CmtBuf = (PSTR) new char[od.CmtBufSize];
	if (od.CmtBuf == NULL)
	{
		return E_OUTOFMEMORY;
	}

	int nrtn = 0;

	od.OpenMode = RAR_OM_EXTRACT;
	HANDLE hArc = RAROpenArchiveEx(&od);
	if (od.OpenResult == S_OK)
	{
		struct RARHeaderData hd = {0};
		while ((od.OpenResult = RARReadHeader(hArc, &hd)) == S_OK)
		{
			od.OpenResult = RARProcessFile(hArc, RAR_EXTRACT, ptzDstDir, NULL);
			if (od.OpenResult != S_OK)
			{
				nrtn = -1;
				break;
			}
		}
	}
	else
	{
		nrtn = -1;
	}

	RARCloseArchive(hArc);
	free(od.CmtBuf);
	return nrtn;
}

HRESULT RARGetCommon(PTSTR ptzSrcFile, std::string& strCommon)
{
	struct RAROpenArchiveDataEx od = {0};
	od._ArcName = ptzSrcFile;
	od.CmtBufSize = 64*1024;
	od.CmtBuf = (PSTR) new char[od.CmtBufSize];
	if (od.CmtBuf == NULL)
	{
		return E_OUTOFMEMORY;
	}

	int nrtn = 0;

	od.OpenMode = RAR_OM_EXTRACT;
	HANDLE hArc = RAROpenArchiveEx(&od);
	if (od.OpenResult == S_OK)
	{
		strCommon = od.CmtBuf;
	}
	else
	{
		nrtn = -1;
	}

	RARCloseArchive(hArc);
	free(od.CmtBuf);
	return nrtn;
}

//}

#else

int RARX(char* ptzSrcFile, char* ptzDstDir)
{
	char strCmd[1024];
	char* pTemp = NULL;
	
	sprintf(strCmd, "rar x -y %s %s/ | grep \"All OK\"", ptzSrcFile, ptzDstDir);

	FILE *fpTemp = NULL;
	if (NULL == (fpTemp = popen(strCmd, "r")))
	{
		printf("Ω‚—πÀı ß∞‹£∫popen¥ÌŒÛ[%s]\n", strCmd);
		return -1;
	}
	
	pTemp = fgets(strCmd, 512, fpTemp);
	pclose(fpTemp);

	if (!pTemp)
	{
		return -1;
	}

	if ( 0 == strncmp(pTemp, "All OK", 6))
	{
		return 0;
	}

	return -1;
}

#endif // OS_WINDOWS
