//////////////////////////////////////////////////////////////////////////
//MonitorManagerConfig.cpp

#include "stdafx.h"
#include "MonitorManagerConfig.h"

#include "MonitorTaskRW.h"

CMonitorManagerConfig::CMonitorManagerConfig()
{
}

CMonitorManagerConfig::~CMonitorManagerConfig()
{
}


int CMonitorManagerConfig::open(const char * xml_filename)
{
	return m_xml_tool.open(xml_filename);
}

void CMonitorManagerConfig::close()
{

}

int CMonitorManagerConfig::GetMonitorTasks(std::vector<MONITOR_TASK> & aMonitorTask)
{
	if (!m_xml_tool.GetRootNode())
		return -1;

	TiXmlNode * service_node = m_xml_tool.GetRootNode()->FirstChildElement("MonitorService");

	if( !service_node )
		return 0;

	return CMonitorTaskRW::GetMonitorTasks(m_xml_tool,service_node,aMonitorTask);

}


int CMonitorManagerConfig::ReadProbeServiceConfigParam(PROBESERVICE_CONFIG_PARAM& config_param)
{
	if( !m_xml_tool.GetRootNode() )
		return -1;

	TiXmlNode *pNetcomNode = m_xml_tool.GetRootNode()->FirstChildElement("net_com");

	if (!pNetcomNode)
		return -1;


	TiXmlNode *pCtrlProgramNode = pNetcomNode->FirstChildElement("ctrl_program");

	if (!pCtrlProgramNode)
		return -1;

	m_xml_tool.GetValueLikeIni(pCtrlProgramNode,"log_state",config_param.bLogState);	

	return 0;
}


int CMonitorManagerConfig::SetMonitorTaskState(const char * taskname,int state)
{
	if (!m_xml_tool.GetRootNode())
		return -1;

	TiXmlNode * service_node = m_xml_tool.GetRootNode()->FirstChildElement("MonitorService");

	if( !service_node )
		return -1;

	return CMonitorTaskRW::SetMonitorTaskState(m_xml_tool,service_node,taskname,state);
}
