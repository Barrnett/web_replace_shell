///////////////////////////////////////////////////////////////////////////
//stop_all_program.h

#include "MonitorTask.h"
#include "cpf/TinyXmlEx.h"

class CStop_all_program
{
public:
	CStop_all_program();
	~CStop_all_program();

	BOOL init();
	void close();
	
public:
	int StopOldProgram();

private:
	CTinyXmlEx		m_xml_reader;
	TiXmlNode *		m_pCurUpdateNode;



};

