//////////////////////////////////////////////////////////////////////////
//flux_tools.cpp

#include "cpf/flux_tools.h"
#include "cpf/other_tools.h"

FLUX_UNIT_TYPE CPF::GetFluxUnitType(ACE_UINT64 bps_data)
{
	if( bps_data >= CPF_FLUX_Tbsp )
		return FLUX_UNIT_TBPS;

	if( bps_data >= CPF_FLUX_Gbsp )
		return FLUX_UNIT_GBPS;

	if( bps_data >= CPF_FLUX_Mbsp )
		return FLUX_UNIT_MBPS;

	if( bps_data >= CPF_FLUX_Kbps )
		return FLUX_UNIT_KBPS;

	return FLUX_UNIT_BPS;
}


inline ACE_UINT64 CPF::GetShowFlux_int(ACE_UINT64 bps_data,FLUX_UNIT_TYPE type)
{
	switch(type)
	{
	case FLUX_UNIT_BPS:
		return bps_data;
		break;

	case FLUX_UNIT_KBPS:
		return bps_data/CPF_FLUX_Kbps;
		break;

	case FLUX_UNIT_MBPS:
		return bps_data/CPF_FLUX_Mbsp;
		break;

	case FLUX_UNIT_GBPS:
		return bps_data/CPF_FLUX_Gbsp;
		break;

	case FLUX_UNIT_TBPS:
		return bps_data/CPF_FLUX_Tbsp;
		break;
	}

	return 0;
}

double CPF::GetShowFlux_double(ACE_UINT64 bps_data,FLUX_UNIT_TYPE type)
{
	switch(type)
	{
	case FLUX_UNIT_BPS:
		return ((double)bps_data);
		break;

	case FLUX_UNIT_KBPS:
		return ((double)bps_data)/CPF_FLUX_Kbps;
		break;

	case FLUX_UNIT_MBPS:
		return ((double)bps_data)/CPF_FLUX_Mbsp;
		break;

	case FLUX_UNIT_GBPS:
		return ((double)bps_data)/CPF_FLUX_Gbsp;
		break;

	case FLUX_UNIT_TBPS:
		return ((double)bps_data)/CPF_FLUX_Tbsp;
		break;
	}

	return 0;
}

inline ACE_UINT64 CPF::GetShowFlux_int(ACE_UINT64 bps_data,OUT FLUX_UNIT_TYPE* type)
{
	FLUX_UNIT_TYPE temptype = GetFluxUnitType(bps_data);

	if( type )
		*type = temptype;

	return GetShowFlux_int(bps_data,temptype);
}

double CPF::GetShowFlux_double(ACE_UINT64 bps_data,OUT FLUX_UNIT_TYPE* type)
{
	FLUX_UNIT_TYPE temptype = GetFluxUnitType(bps_data);

	if( type )
		*type = temptype;


	return GetShowFlux_double(bps_data,temptype);
}


ACE_UINT64 CPF::GetFluxInfo(ACE_UINT64 cur_bytes,ACE_UINT64 old_bytes,ACE_UINT32 msec,int showtype,std::string * pstrinfo)
{
	ACE_UINT64 ubitSpeed = 0;

	if( msec != 0 )
	{
		ubitSpeed = (cur_bytes - old_bytes)*8*1000/msec;
	}
	else
	{
		ubitSpeed = 0;
	}

	if( pstrinfo )
	{
		*pstrinfo = GetFluxInfo(ubitSpeed,showtype);
	}

	return ubitSpeed;
}

std::string CPF::GetFluxInfo_2(ACE_UINT64 bps_data,int showtype,OUT FLUX_UNIT_TYPE* type)
{
	char buf[256] = {0};

	FLUX_UNIT_TYPE unit_type = FLUX_UNIT_BPS;

	double dSpeed = GetShowFlux_double(bps_data,&unit_type);

	if( type )
	{
		*type = unit_type;
	}

	sprintf(buf,"%.2f%s",dSpeed,GetFluxUnitTitle(unit_type,showtype).c_str());

	return std::string(buf);
}

std::string CPF::GetFluxInfo(ACE_UINT64 bps_data,int showtype,OUT FLUX_UNIT_TYPE* type)
{
	char buf[256] = {0};

	FLUX_UNIT_TYPE unit_type = FLUX_UNIT_BPS;

	double dSpeed = GetShowFlux_double(bps_data,&unit_type);

	if( type )
	{
		*type = unit_type;
	}

	if( FLUX_UNIT_TBPS == unit_type 
		|| FLUX_UNIT_GBPS == unit_type )
	{
		sprintf(buf,"%.2f%s",dSpeed,GetFluxUnitTitle(unit_type,showtype).c_str());
	}
	else if( FLUX_UNIT_MBPS == unit_type )
	{
		sprintf(buf,"%.1f%s",dSpeed,GetFluxUnitTitle(unit_type,showtype).c_str());
	}
	else
	{
		sprintf(buf,"%u%s",(unsigned int)dSpeed,GetFluxUnitTitle(unit_type,showtype).c_str());
	}

	return std::string(buf);
}

ACE_UINT32 CPF::GetPktSpeed(ACE_UINT64 cur_pkts,ACE_UINT64 old_pkts,ACE_UINT32 msec,int showtype,std::string * pstrinfo)
{
	ACE_UINT32 uPktSpeed = 0;

	if( msec != 0 )
	{
		uPktSpeed = (ACE_UINT32)((cur_pkts - old_pkts)*1000/msec);
	}
	else
	{
		uPktSpeed = 0;
	}

	if( pstrinfo )
	{
		*pstrinfo = CPF::GetFriendNumString(uPktSpeed);

		if( showtype == 0 )
		{
			*pstrinfo += "pps";
		}
	}

	return uPktSpeed;
}


std::string CPF::GetFluxUnitTitle(FLUX_UNIT_TYPE nFluxUnitType,int showtype)
{
	std::string strCaption;

	if(showtype == 0)
	{
		switch(nFluxUnitType)
		{
		case FLUX_UNIT_BPS:
			strCaption = "bps";
			break;

		case FLUX_UNIT_KBPS:
			strCaption = "Kbps";
			break;

		case FLUX_UNIT_MBPS:
			strCaption = "Mbps";
			break;
		case FLUX_UNIT_GBPS:
			strCaption = "Gbps";
			break;
		case FLUX_UNIT_TBPS:
			strCaption = "Tbps";
			break;

		default:
			ACE_ASSERT(FALSE);
			strCaption = "bps";
			break;
		}
	}
	else if(showtype == 1)
	{
		switch(nFluxUnitType)
		{
		case FLUX_UNIT_BPS:
			strCaption = "";
			break;

		case FLUX_UNIT_KBPS:
			strCaption = "K";
			break;

		case FLUX_UNIT_MBPS:
			strCaption = "M";
			break;
		case FLUX_UNIT_GBPS:
			strCaption = "G";
			break;
		case FLUX_UNIT_TBPS:
			strCaption = "T";
			break;

		default:
			ACE_ASSERT(FALSE);
			strCaption = "";
			break;
		}
	}
	else if(showtype == 2)
	{
		switch(nFluxUnitType)
		{
		case FLUX_UNIT_BPS:
			strCaption = "B";
			break;

		case FLUX_UNIT_KBPS:
			strCaption = "KB";
			break;

		case FLUX_UNIT_MBPS:
			strCaption = "MB";
			break;
		case FLUX_UNIT_GBPS:
			strCaption = "GB";
			break;
		case FLUX_UNIT_TBPS:
			strCaption = "TB";
			break;

		default:
			ACE_ASSERT(FALSE);
			strCaption = "";
			break;
		}
	}
	else
	{		
		ACE_ASSERT(FALSE);

		return GetFluxUnitTitle(nFluxUnitType,1);
	}

	return strCaption;

}
