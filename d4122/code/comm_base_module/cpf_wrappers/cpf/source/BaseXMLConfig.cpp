//////////////////////////////////////////////////////////////////////////
//BaseXMLConfig.cpp

#include "cpf/BaseXMLConfig.h"

CBaseXMLConfig::CBaseXMLConfig()
{
}

CBaseXMLConfig::~CBaseXMLConfig()
{
}

BOOL CBaseXMLConfig::LoadFromMem(const char * buf,int length)
{
	close();

	if( buf == NULL || length == 0 )
	{
		return true;
	}

	CTinyXmlEx xml_reader;

	if( xml_reader.open(buf,length) != 0 )
	{
		return false;
	}

	if( !xml_reader.GetRootNode() )
	{
		return false;
	}

	return LoadConfig(xml_reader);
}

BOOL CBaseXMLConfig::LoadFromFile(const char * xml_file_name)
{
	close();

	m_strCurOpenFilename = xml_file_name;

	CTinyXmlEx xml_reader;

	if( xml_reader.open_for_readonly(xml_file_name) != 0 )
		return false;

	return LoadConfig(xml_reader);
}

BOOL CBaseXMLConfig::LoadFromCryDataFile(const char * data_file_name)
{
	CCPFOctets buf_oct;

	if( !LoadCryDataFile(data_file_name,buf_oct) )
		return false;

	LoadFromMem((const char *)buf_oct.GetData(),buf_oct.GetDataLength());

	return true;
}


BOOL CBaseXMLConfig::SaveToFile()
{
	return SaveToFile(m_strCurOpenFilename.c_str());
}

BOOL CBaseXMLConfig::SaveToFile(const char * xml_file_name)
{
	CTinyXmlEx xml_writter;

	if( 0 != xml_writter.open_ex(xml_file_name,CTinyXmlEx::OPEN_MODE_CREATE_TRUNC) )
		return false;

	SaveConfig(xml_writter);

	xml_writter.savefile();
	xml_writter.close();

	return true;
}

BOOL CBaseXMLConfig::LoadCryDataFile(const char * dat_filename, CCPFOctets& outOctets)
{
	// reading in binary mode so that tinyxml can normalize the EOL
	FILE* file = fopen( dat_filename, "rb" );	

	if( !file )
	{
		return false;
	}

	// Get the file size, so we can pre-allocate the string. HUGE speed impact.
	long length = 0;
	fseek( file, 0, SEEK_END );
	length = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Strange case, but good to handle up front.
	if ( length <= 0 )
	{
		return false;
	}

	char* buf = new char[ length+1 ];
	buf[0] = 0;

	if ( fread( buf, length, 1, file ) != 1 ) 
	{
		delete [] buf;
		return false;
	}

	{
		buf[0]-=19;
		buf[1]+=37;

		for(int i = 2; i < length; ++i)
		{
			if( i % 2 == 0 )
				buf[i] ^= buf[1];
			else
				buf[i] ^= buf[0];
		}
	}

	buf[length] = 0;

	outOctets.Reset();
	outOctets.Append((const BYTE*)(buf+2), length-1);

	delete []buf;

	fclose(file);

	return true;
}
