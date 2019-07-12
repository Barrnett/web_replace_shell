//////////////////////////////////////////////////////////////////////////
//CommonNBTDecode.cpp

#include "cpf/CommonNBTDecode.h"

CCommonNBTDecode::CCommonNBTDecode(void)
{
}

CCommonNBTDecode::~CCommonNBTDecode(void)
{
}

//pPacket是nbt的数据头指针， dwPacketLen是输入数据的长度。
//作为输出，是跳过头部后的数据指针，dwPacketLen是去掉头部后的数据长度。
BOOL  CCommonNBTDecode::NBOnUDDecode_Header(const BYTE *& pPacket, DWORD& dwPacketLen, 
								 NetBios_ONUDP_HEADER& header_info)
{
	memset(&header_info,0x00,sizeof(header_info));

	if( dwPacketLen < 12 )
	{
		return FALSE;
	}

	const BYTE * pData = pPacket;

	header_info.m_NAME_TRN_ID = ACE_NTOHS(*(WORD *)pData);
	pData += sizeof(WORD);

	header_info.m_rsp = (((*pData)&0x80)>>7);

	header_info.m_OPCODE = (((*pData)>>3)&0x0f);

	header_info.m_NM_FLAGS = (*pData)<<5;
	pData++;

	header_info.m_NM_FLAGS += (*pData)>>3;
	header_info.m_NM_FLAGS = header_info.m_NM_FLAGS & 0xfe;
	header_info.m_RCODE = (*pData)&0x0f;
	pData++;

	header_info.m_QDCOUNT = ACE_NTOHS(*(WORD *)pData);
	pData += sizeof(WORD);

	header_info.m_ANCOUNT = ACE_NTOHS(*(WORD *)pData);	
	pData += sizeof(WORD);

	header_info.m_NSCOUNT = ACE_NTOHS(*(WORD *)pData);
	pData += sizeof(WORD);

	header_info.m_ARCOUNT = ACE_NTOHS(*(WORD *)pData);
	pData += sizeof(WORD);

	if( pData - pPacket != 12 )
	{
		return false;
	}

	dwPacketLen = dwPacketLen - (int)(pData - pPacket);

	pPacket = pData;

	return true;
}

/*
1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         NAME_TRN_ID           |0|  0x5  |0|0|1|0|0 0|B|  0x0  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          0x0001               |           0x0000              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          0x0000               |           0x0001              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
/                         QUESTION_NAME                         /
/                                                               /
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           NB (0x0020)         |        IN (0x0001)            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
/                            RR_NAME                            /
/                                                               /
|                                                               |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           NB (0x0020)         |         IN (0x0001)           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                              TTL                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           0x0006              |          NB_FLAGS             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          NB_ADDRESS                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
BOOL CCommonNBTDecode::NBOnUDPDecode_Register_Req(const BYTE * pNbtData,
											  const BYTE * pRegisterInfoPacket,
											  DWORD dwegisterInfoPacketLen,
											  const NetBios_ONUDP_HEADER& header_info,
											  NetBios_ONUDP_RegisterInfo& register_info)
{
	memset(&register_info,0x00,sizeof(register_info));

	if( header_info.m_rsp != 0 )
	{
		return false;
	}

	if( !(header_info.m_QDCOUNT == 0x0001 
		&& header_info.m_ANCOUNT == 0x00 
		&& header_info.m_NSCOUNT == 0x00
		&& header_info.m_ARCOUNT == 0x01) )
	{
		return false;
	}

	int decodenamelen = TransferEncodeNameIntoNbName(
		pNbtData,pRegisterInfoPacket,
		register_info.m_decodeName,
		sizeof(register_info.m_decodeName),
		register_info.m_nNameType);

	if( decodenamelen < 0 )
	{
		return false;
	}

	pRegisterInfoPacket += decodenamelen;

	register_info.m_QuestionType = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);
	register_info.m_QuestionClass = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);

	if( register_info.m_QuestionClass != 0x0001 
		|| register_info.m_QuestionType != 0x0020 )
	{
		return false;
	}

	{
		int decodenamelen = TransferEncodeNameIntoNbName(
			pNbtData,pRegisterInfoPacket,
			register_info.m_RRName,
			sizeof(register_info.m_RRName),
			register_info.m_nNameType);

		if( decodenamelen < 0 )
		{
			return false;
		}

#ifdef _DEBUG
		if( header_info.m_OPCODE == 0x05 )
		{
			ACE_ASSERT(strcmp(register_info.m_RRName,register_info.m_decodeName)==0);
		}
#endif

		pRegisterInfoPacket += decodenamelen;
	}


	register_info.m_RRType = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);
	register_info.m_RRClass = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);

	if( register_info.m_RRClass != 0x0001 
		|| register_info.m_RRType != 0x0020 )
	{
		return false;
	}

	register_info.m_RRTTL = ACE_NTOHL(*(DWORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(DWORD);

	register_info.m_RDLength = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);

	if( register_info.m_RDLength != 6 )
	{
		return false;
	}

	register_info.m_RNodeFlags = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);

	register_info.m_RNodeAddress = (*(DWORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(DWORD);

	register_info.m_nodeType = ((register_info.m_RNodeFlags >> 13)&0x03);

	register_info.m_nGroupOrUnique = (register_info.m_RNodeFlags & 0x8000);

	if( !IsComputerOrDomainName(register_info.m_nGroupOrUnique,
		register_info.m_nNameType,
		register_info.m_nComputerOrDomainName) )
	{
		return false;
	}

	return true;

}

BOOL CCommonNBTDecode::NBOnUDPDecode_Refresh_Req(const BYTE * pNbtData,
								  const BYTE * pRegisterInfoPacket,
								  DWORD dwegisterInfoPacketLen,
								  const NetBios_ONUDP_HEADER& header_info,
								  NetBios_ONUDP_RefreshInfo& refresh_info)
{
	return CCommonNBTDecode::NBOnUDPDecode_Register_Req(pNbtData,pRegisterInfoPacket,
		dwegisterInfoPacketLen,header_info,refresh_info);
}

BOOL CCommonNBTDecode::NBOnUDPDecode_Release_Req(const BYTE * pNbtData,
									  const BYTE * pRegisterInfoPacket,
									  DWORD dwegisterInfoPacketLen,
									  const NetBios_ONUDP_HEADER& header_info,
									  NetBios_ONUDP_ReleaseInfo& release_info)
{
	return CCommonNBTDecode::NBOnUDPDecode_Register_Req(pNbtData,pRegisterInfoPacket,
		dwegisterInfoPacketLen,header_info,release_info);

}

BOOL CCommonNBTDecode::NBOnUDPDecode_Qurey_Positive_Response(const BYTE * pNbtData,
										 const BYTE * pRegisterInfoPacket,
										 DWORD dwegisterInfoPacketLen,
										 const NetBios_ONUDP_HEADER& header_info,
										 NetBios_ONUDP_QueryResopnse_Info& QueryResponseInfo)
{
	QueryResponseInfo.m_RRType = 0;
	QueryResponseInfo.m_RRClass = 0;
	QueryResponseInfo.m_RRTTL = 0;
	QueryResponseInfo.m_RDLength = 0;
	QueryResponseInfo.m_RRName[0] = 0;
	QueryResponseInfo.m_vtIPAddr.clear();

	if( header_info.m_rsp != 1 )
	{
		return false;
	}

	if( !(header_info.m_QDCOUNT == 0x00 
		&& header_info.m_ANCOUNT == 0x01 
		&& header_info.m_NSCOUNT == 0x00
		&& header_info.m_ARCOUNT == 0x00) )
	{
		return false;
	}

	int decodenamelen = TransferEncodeNameIntoNbName(
		pNbtData,pRegisterInfoPacket,
		QueryResponseInfo.m_RRName,
		sizeof(QueryResponseInfo.m_RRName),
		QueryResponseInfo.m_nNameType);

	if( decodenamelen < 0 )
	{
		return false;
	}

	const BYTE * pOrgRegisterInfoPacket = pRegisterInfoPacket;

	pRegisterInfoPacket += decodenamelen;

	QueryResponseInfo.m_RRType = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);
	QueryResponseInfo.m_RRClass = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);

	if( QueryResponseInfo.m_RRClass != 0x0001 
		|| QueryResponseInfo.m_RRType != 0x0020 )
	{
		return false;
	}

	QueryResponseInfo.m_RRTTL = ACE_NTOHL(*(DWORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(DWORD);

	QueryResponseInfo.m_RDLength = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
	pRegisterInfoPacket += sizeof(WORD);

	//还剩下的包长度
	int addr_len = dwegisterInfoPacketLen - (int)(pRegisterInfoPacket - pOrgRegisterInfoPacket);

	int len = 0;

	while( len + 6 <= QueryResponseInfo.m_RDLength && len + 6 <= addr_len )
	{
		WORD RNodeFlags = ACE_NTOHS(*(WORD *)pRegisterInfoPacket);
		pRegisterInfoPacket += sizeof(WORD);

		DWORD RNodeAddress = (*(DWORD *)pRegisterInfoPacket);
		pRegisterInfoPacket += sizeof(DWORD);

		len += 6;

		QueryResponseInfo.m_vtIPAddr.push_back(std::make_pair(RNodeFlags,RNodeAddress));
	}
	
	return true;

}

int CCommonNBTDecode::TransferEncodeNameIntoNbName(const BYTE * pNbtData,
													const BYTE * pEncodeName,
													char * nbbuffer, 
													int nbName_buf_len, 
													BYTE& type)
{	
	if( ((*pEncodeName)&0xc0) == 0xc0 )
	{
		ACE_UINT16 name_offset = NBUF_TO_WORD(pEncodeName);

		name_offset &= 0x3fff;

		TransferEncodeNameIntoNbName(pNbtData,pNbtData+name_offset,nbbuffer,nbName_buf_len,type);

		return 2;
	}

	int encode_namelen = (int)(*(BYTE *)pEncodeName);

	if( encode_namelen > 32 )
	{
		return -1;
	}

	pEncodeName++;

	char * nbName = nbbuffer;

	int len = 0;

	while( 0 != *(BYTE *)pEncodeName
		&& len < encode_namelen
		&& nbName - nbbuffer <  nbName_buf_len )
	{
		BYTE b1 = *(BYTE *)pEncodeName - 'A';
		pEncodeName++;

		BYTE b2 = *(BYTE *)pEncodeName - 'A';
		pEncodeName++;

		len += 2;

		*nbName = (b1<<4)|b2;

		if( ' ' == *nbName )
		{
			*nbName = 0;
		}

		nbName++;
	}

	if( nbName > nbbuffer )
	{
		type = *(BYTE *)(nbName-1);
		*(BYTE *)(nbName-1) = 0;
	}

	return encode_namelen + 2;
}


BOOL CCommonNBTDecode::IsComputerOrDomainName(int GroupOrUnique,BYTE type,
											  int& nComputerOrDomainName)
{

	if( GroupOrUnique == 0)//unique
	{
		switch(type)
		{
		case 0x00:
		case 0x03:
		case 0x06:
		case 0x1f:
		case 0x20:
		case 0x21:
		case 0xbe:
		case 0xbf:
			nComputerOrDomainName = 1;//computername
			break;

		case 0x1b:
		case 0x1d:
			nComputerOrDomainName = 0;//domainname
			break;

		default:
			return false;
		}
	}
	else//group
	{
		switch(type)
		{
		case 0x00:
		case 0x1c:
		case 0x1e:
			nComputerOrDomainName = 0;//domainname
			break;

		default:
			return false;
		}
	}


	return true;
}

/*
NetBIOS Names
Microsoft networking components, such as Windows NT Workstation and Windows NT Server services, allow the first 15 characters of a NetBIOS name to be specified by the user or administrator, but reserve the 16th character of the NetBIOS name (00-FF hex) to indicate a resource type. Following are some examples of NetBIOS names used by Microsoft components:

/////////////////Unique Names////////////////////////
\\computer_name[00h]
Registered by the Workstation Service on the WINS Client

\\computer_name[03h]
Registered by the Messenger Service on the WINS Client

\\computer_name[06h]
Registered by the Remote Access Service (RAS), when started on a RAS Server.

\\computer_name[1Fh]
Registered by the Network Dynamic Data Exchange (NetDDE) services, will only appear if the NetDDE services are started on the computer. By default under Windows NT 3.51, the NetDDE services are not automatically started.

\\computer_name[20h]
Registered by the Server Service on the WINS Client.

\\computer_name[21h]
Registered by the RAS Client Service, when started on a RAS Client.

\\computer_name[BEh]
Registered by the Network Monitoring Agent Service—will only appear if the service is started on the computer. If the computer name is not a full 15 characters, the name will be padded with plus (+) symbols.

\\computer_name[BFh]
Registered by the Network Monitoring Utility (included with Microsoft Systems Management Server). If the computer name is not a full 15 characters, the name will be padded with plus (+) symbols.

\\username[03h]
User names for the currently logged on users are registered in the WINS database. The user name is registered by the Server component so that the user can receive any "net send" commands sent to their user name. If more than one user is logged on with the same user name, only the first computer at which a user logged on with the user name will register the name.

\\domain_name[1Bh]
Registered by the Windows NT Server primary domain controller (PDC) that is running as the Domain Master Browser and is used to allow remote browsing of domains. When a WINS server is queried for this name, a WINS server returns the IP address of the computer that registered this name.

\\domain_name[1Dh]
Registered only by the Master Browser, of which there can be only one for each subnet. This name is used by the Backup Browsers to communicate with the Master Browser to retrieve the list of available servers from the Master Browser.

WINS servers always return a positive registration response for domain_name[1D], even though the WINS server does not "register" this name in its database. Therefore, when a WINS server is queried for the domain_name[1D], the WINS server returns a negative response, which will cause the client to broadcast to resolve the name.

/////////////////Group Names////////////////////////
\\domain_name[00h]
Registered by the Workstation Service so that it can receive browser broadcasts from LAN Manager-based computers.

\\domain_name[1Ch]
Registered for use by the domain controllers within the domain and can contain up to 25 IP addresses. One IP address will be that of the primary domain controller (PDC) and the other 24 will be the IP addresses of backup domain controllers (BDCs).

\\domain_name[1Eh]
Registered for browsing purposes and used by the browsers to elect a Master Browser (this is how a statically mapped group name registers itself). When a WINS server receives a name query for a name ending with [1E], the WINS server will always return the network broadcast address for the requesting client's local network.

\\--__MSBROWSE__[01h]
Registered by the Master Browser for each subnet. When a WINS server receives a name query for this name, the WINS server will always return the network broadcast address for the requesting client's local network. 

*/

/*

void CCommonUDPDecode::NBOnUDPProtocolDecode(const BYTE *pData, WORD wLen, NetBios_ONUDP_DECODE_INFO& nbUDPinfo)
{
nbUDPinfo.m_dwError = 1;

nbUDPinfo.m_NAME_TRN_ID = *(WORD *)pData;
pData += sizeof(WORD);
nbUDPinfo.m_OPCODE = (*pData)&0xf8;
nbUDPinfo.m_NM_FLAGS = (*pData)<<5;
pData++;
nbUDPinfo.m_NM_FLAGS += (*pData)>>3;
nbUDPinfo.m_NM_FLAGS = nbUDPinfo.m_NM_FLAGS & 0xfe;
nbUDPinfo.m_RCODE = (*pData)&0x0f;
pData++;
nbUDPinfo.m_QDCOUNT = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_ANCOUNT = ACE_NTOHS(*(WORD *)pData);	
pData += sizeof(WORD);
nbUDPinfo.m_NSCOUNT = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_ARCOUNT = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);

size_t decodenamelen = strlen((const char *)pData)+1;

if( decodenamelen > 256 )
return;

memcpy(nbUDPinfo.m_encodeName, pData, decodenamelen );
pData += decodenamelen;

nbUDPinfo.m_QuestionType = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_QuestionClass = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_RRName = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_RRType = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_RRClass = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_RRTTL = ACE_NTOHL(*(DWORD *)pData);
pData += sizeof(DWORD);
nbUDPinfo.m_RDLength = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_RNodeFlags = ACE_NTOHS(*(WORD *)pData);
pData += sizeof(WORD);
nbUDPinfo.m_RNodeAddress = ACE_NTOHL(*(DWORD *)pData);

nbUDPinfo.m_nGroupOrUnique = (nbUDPinfo.m_RNodeFlags & 0x8000);

BYTE type;

if( TransferEncodeNameIntoNbName(nbUDPinfo.m_encodeName, 
nbUDPinfo.m_decodeName, sizeof(nbUDPinfo.m_decodeName),&type) == FALSE )
{		
return;
}

if( nbUDPinfo.m_nGroupOrUnique == 0)//unique
{
switch(type)
{
case 0x00:
case 0x06:
case 0x1f:
case 0x20:
case 0x21:
case 0xbe:
case 0xbf:
nbUDPinfo.m_nComputerOfDomainName = 1;//computername
break;

case 0x1b:
case 0x1d:
nbUDPinfo.m_nComputerOfDomainName = 0;//domainname
break;

default:
return;
}
}
else//group
{
switch(type)
{
case 0x00:
case 0x1c:
case 0x1e:
nbUDPinfo.m_nComputerOfDomainName = 0;//domainname
break;

default:
return;
}
}

nbUDPinfo.m_dwError = 0;

return;

}


//return value:0 error ,1 ok
int CCommonUDPDecode::TransferEncodeNameIntoNbName(const char * pEncodeName, char * nbbuffer, 
int nbName_buf_len, BYTE * type)
{	
int encode_namelen = (int)(*(BYTE *)pEncodeName);

if( encode_namelen > 32 )
return 0;

pEncodeName++;

char * nbName = nbbuffer;

int len = 0;

while( 0 != *(BYTE *)pEncodeName
&& len < encode_namelen
&& nbName - nbbuffer <  nbName_buf_len )
{
BYTE b1 = *(BYTE *)pEncodeName - 'A';
pEncodeName++;

BYTE b2 = *(BYTE *)pEncodeName - 'A';
pEncodeName++;

len += 2;

*nbName = (b1<<4)|b2;

if( ' ' == *nbName )
{
*nbName = 0;
break;
}

nbName++;
}

if( nbName > nbbuffer )
{
*type = *(BYTE *)(nbName-1);
*(BYTE *)(nbName-1) = 0;
}

return 1;
}
*/



/*	//rfc1001,rfc1002
typedef struct
{
WORD m_NAME_TRN_ID;
BYTE m_rsp;//0表示qurey，1表示rsp
BYTE m_OPCODE;//0 = query，5 = registration，6 = release，7 = WACK，8 = refresh
BYTE m_NM_FLAGS;
BYTE m_RCODE;
WORD m_QDCOUNT;
WORD m_ANCOUNT;
WORD m_NSCOUNT;
WORD m_ARCOUNT;

char m_encodeName[256];
WORD m_QuestionType;
WORD m_QuestionClass;

WORD m_RRName;
WORD m_RRType;
WORD m_RRClass;
DWORD m_RRTTL;
WORD  m_RDLength;
WORD  m_RNodeFlags;
DWORD m_RNodeAddress;	

char  m_decodeName[16];
int	  m_nGroupOrUnique;//0=unique,1=group	
int	  m_nComputerOfDomainName;//0=domain,1=computer

DWORD	m_dwError;//0表示没有错误

}NetBios_ONUDP_DECODE_INFO;

*/

