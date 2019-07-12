///////////////////////////////////////////////////////////////////////////
//restart_all_program.h

#include "MonitorTask.h"
#include "cpf/TinyXmlEx.h"

class Crestart_all_program
{
public:
	Crestart_all_program();
	~Crestart_all_program();

	BOOL init();
	void close();
	
public:
	int StopOldProgram();
	int StartProgram();

private:
	CTinyXmlEx		m_xml_reader;
	TiXmlNode *		m_pCurUpdateNode;


private:
	int StartOneProgram(const char * prg_name);

private:
	int RunProcess(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);
	int RunBat(const char * szAppName, const char *szCmd, const char *szWorkingDirectory);


};

