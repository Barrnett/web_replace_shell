//////////////////////////////////////////////////////////////////////////
//File_Data_Stream.cpp

#include "cpf/File_Data_Stream.h"


void CFile_Data_Stream::SkipLenAndString(FILE * inputfile)
{
	int len = 0;

	fread(&len,4,1,inputfile);

//#define SEEK_SET    0
//#define SEEK_CUR    1
//#define SEEK_END    2

	fseek(inputfile,len,SEEK_CUR);

	return;
}

std::string CFile_Data_Stream::ReadLenAndString(FILE * inputfile)
{
	std::string data;

	int len = 0;

	fread(&len,4,1,inputfile);

	if( len >= 65536 )
	{
		return data;
	}

	char * buf = new char[len];

	if( !buf )
	{
		return data;
	}

	fread(buf,1,len,inputfile);

	if( len == 1 )
		data.clear();
	else
		data.assign(buf,len-1);

	delete []buf;

	return data;
}

ACE_UINT8 CFile_Data_Stream::ReadUint8(FILE * inputfile)
{
	ACE_UINT8 data;
	fread(&data,sizeof(data),1,inputfile);
	return data;
}

ACE_UINT16 CFile_Data_Stream::ReadUint16(FILE * inputfile)
{
	ACE_UINT16 data;
	fread(&data,sizeof(data),1,inputfile);
	return data;
}

ACE_UINT32 CFile_Data_Stream::ReadUint32(FILE * inputfile)
{
	ACE_UINT32 data;
	fread(&data,sizeof(data),1,inputfile);
	return data;
}

ACE_UINT64 CFile_Data_Stream::ReadUint64(FILE * inputfile)
{
	ACE_UINT64 data;
	fread(&data,sizeof(data),1,inputfile);
	return data;
}

int CFile_Data_Stream::ReadFixString(void * buffer,int len,FILE * inputfile)
{
	return (int)fread(buffer,1,len,inputfile);
}

void CFile_Data_Stream::Skip(int len,FILE * inputfile,int type)
{
	fseek(inputfile,len,type);
}

//////////////////////////////////////////////////////////////////////////

void CFile_Data_Stream::SkipLenAndString(std::ifstream& inputfile)
{
	int len = 0;

	inputfile.read((char *)&len,4);

	//static const _Seekdir beg = (_Seekdir)0;
	//static const _Seekdir cur = (_Seekdir)1;
	//static const _Seekdir end = (_Seekdir)2;

	inputfile.seekg(len,std::ios::cur);

	return;
}

std::string CFile_Data_Stream::ReadLenAndString(std::ifstream& inputfile)
{
	std::string data;

	int len = 0;

	inputfile.read((char *)&len,4);

	if( len >= 65536 )
	{
		return data;
	}

	char * buf = new char[len];

	if( !buf )
	{
		return data;
	}

	inputfile.read(buf,len);

	if( len == 1 )
		data.clear();
	else
		data.assign(buf,len-1);

	delete []buf;

	return data;
}

ACE_UINT8 CFile_Data_Stream::ReadUint8(std::ifstream& inputfile)
{
	ACE_UINT8 data = 0;
	inputfile.read((char *)&data,sizeof(data));
	return data;
}
ACE_UINT16 CFile_Data_Stream::ReadUint16(std::ifstream& inputfile)
{
	ACE_UINT16 data = 0;
	inputfile.read((char *)&data,sizeof(data));
	return data;
}
ACE_UINT32 CFile_Data_Stream::ReadUint32(std::ifstream& inputfile)
{
	ACE_UINT32 data = 0;
	inputfile.read((char *)&data,sizeof(data));
	return data;
}
ACE_UINT64 CFile_Data_Stream::ReadUint64(std::ifstream& inputfile)
{
	ACE_UINT64 data = 0;
	inputfile.read((char *)&data,sizeof(data));
	return data;
}

int CFile_Data_Stream::ReadFixString(void * buffer,int len,std::ifstream& inputfile)
{
	inputfile.read((char *)buffer,len);

	return len;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CFile_Data_Stream::WriteLenAndString(const std::string& data, FILE * outputfile)
{
	int len = (int)data.size()+1;

	fwrite(&len,4,1,outputfile);
	fwrite(data.c_str(),1,len,outputfile);

	return;
}

void CFile_Data_Stream::WriteLenAndString(const char * data,FILE * outputfile)
{
	if( data )
	{
		int len = (int)strlen(data)+1;

		fwrite(&len,4,1,outputfile);
		fwrite(data,1,len,outputfile);
	}
	else
	{
		WriteUint32(1,outputfile);
		WriteUint8(0,outputfile);
	}
}


void CFile_Data_Stream::WriteUint8(ACE_UINT8 data,FILE * outputfile)
{
	fwrite(&data,sizeof(data),1,outputfile);
}

void CFile_Data_Stream::WriteUint16(ACE_UINT16 data,FILE * outputfile)
{
	fwrite(&data,sizeof(data),1,outputfile);
}
void CFile_Data_Stream::WriteUint32(ACE_UINT32 data,FILE * outputfile)
{
	fwrite(&data,sizeof(data),1,outputfile);
}
void CFile_Data_Stream::WriteUint64(ACE_UINT64 data,FILE * outputfile)
{
	fwrite(&data,sizeof(data),1,outputfile);
}

void CFile_Data_Stream::WriteFixString(void * buffer,int len,FILE * outputfile)
{
	fwrite(buffer,1,len,outputfile);
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void CFile_Data_Stream::WriteLenAndString(const std::string& data, std::ofstream& outputfile)
{
	int len = (int)data.size()+1;

	outputfile.write((const char *)&len,4);

	outputfile.write(data.c_str(),len);

	return;
}

void CFile_Data_Stream::WriteLenAndString(const char * data,std::ofstream& outputfile)
{
	if( data )
	{
		int len = (int)strlen(data)+1;

		outputfile.write((const char *)&len,4);

		outputfile.write(data,len);
	}
	else
	{
		WriteUint32(1,outputfile);
		WriteUint8(0,outputfile);
	}
}

void CFile_Data_Stream::WriteUint8(ACE_UINT8 data,std::ofstream& outputfile)
{
	outputfile.write((const char *)&data,sizeof(data));
}
void CFile_Data_Stream::WriteUint16(ACE_UINT16 data,std::ofstream& outputfile)
{
	outputfile.write((const char *)&data,sizeof(data));
}
void CFile_Data_Stream::WriteUint32(ACE_UINT32 data,std::ofstream& outputfile)
{
	outputfile.write((const char *)&data,sizeof(data));
}
void CFile_Data_Stream::WriteUint64(ACE_UINT64 data,std::ofstream& outputfile)
{
	outputfile.write((const char *)&data,sizeof(data));
}

void CFile_Data_Stream::WriteFixString(void * buffer,int len,std::ofstream& outputfile)
{
	outputfile.write((const char *)buffer,len);
}


void CFile_Data_Stream::Skip(int len,std::ifstream& inputfile,std::ios::seekdir type)
{
	inputfile.seekg(len,type);
}

void CFile_Data_Stream::Skip(int len,std::ofstream& outputfile,std::ios::seekdir type)
{
	ACE_ASSERT(FALSE);

	return;
}

