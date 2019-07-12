//////////////////////////////////////////////////////////////////////////
//File_Data_Stream.h

#pragma once

#include "cpf/cpf.h"
#include <stdio.h>
#include <iosfwd>
#include <fstream>

//文件中的数据流格式，参照Data_Steam.h
class CPF_CLASS CFile_Data_Stream
{
public:
	static void SkipLenAndString(FILE * inputfile);
	static std::string ReadLenAndString(FILE * inputfile);
	static ACE_UINT8 ReadUint8(FILE * inputfile);
	static ACE_UINT16 ReadUint16(FILE * inputfile);
	static ACE_UINT32 ReadUint32(FILE * inputfile);
	static ACE_UINT64 ReadUint64(FILE * inputfile);
	static int ReadFixString(void * buffer,int len,FILE * inputfile);

public:
	static void WriteLenAndString(const std::string& data,FILE * outputfile);
	static void WriteLenAndString(const char * data,FILE * outputfile);

	static void WriteUint8(ACE_UINT8 data,FILE * outputfile);
	static void WriteUint16(ACE_UINT16 data,FILE * outputfile);
	static void WriteUint32(ACE_UINT32 data,FILE * outputfile);
	static void WriteUint64(ACE_UINT64 data,FILE * outputfile);
	static void WriteFixString(void * buffer,int len,FILE * outputfile);

public:
	//#define SEEK_SET    0
	//#define SEEK_CUR    1
	//#define SEEK_END    2
	static void Skip(int len,FILE * inputfile,int type=SEEK_CUR);

public:
	static void SkipLenAndString(std::ifstream& inputfile);
	static std::string ReadLenAndString(std::ifstream& inputfile);
	static ACE_UINT8 ReadUint8(std::ifstream& inputfile);
	static ACE_UINT16 ReadUint16(std::ifstream& inputfile);
	static ACE_UINT32 ReadUint32(std::ifstream& inputfile);
	static ACE_UINT64 ReadUint64(std::ifstream& inputfile);
	static int ReadFixString(void * buffer,int len,std::ifstream& inputfile);

public:
	static void WriteLenAndString(const std::string& data,std::ofstream& outputfile);
	static void WriteLenAndString(const char * data,std::ofstream& outputfile);

	static void WriteUint8(ACE_UINT8 data,std::ofstream& outputfile);
	static void WriteUint16(ACE_UINT16 data,std::ofstream& outputfile);
	static void WriteUint32(ACE_UINT32 data,std::ofstream& outputfile);
	static void WriteUint64(ACE_UINT64 data,std::ofstream& outputfile);
	static void WriteFixString(void * buffer,int len,std::ofstream& outputfile);

public:
	//static const _Seekdir beg = (_Seekdir)0;
	//static const _Seekdir cur = (_Seekdir)1;
	//static const _Seekdir end = (_Seekdir)2;

	static void Skip(int len,std::ifstream& inputfile,std::ios::seekdir type=std::ios::cur);
	static void Skip(int len,std::ofstream& outputfile,std::ios::seekdir type=std::ios::cur);

};

