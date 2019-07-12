#ifndef _CRC_TOOLS_H_ZHUZP_2006_3_12
#define _CRC_TOOLS_H_ZHUZP_2006_3_12
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "cpf/cpf.h"

namespace CPF
{

	/********************************************************************

	PPP_FCSCheck16&PPP_FCSCheck32有关FCS的代码基于 RFC 1662 附录 C

	********************************************************************/

	/*
	* The FCS-32 generator polynomial: x**0 + x**5 + x**12 + x**16
	*/
	CPF_EXPORT
		BOOL PPP_FCSCheck16(const BYTE * pData,int nlen);

	CPF_EXPORT
		ACE_UINT16 GetFCS16(const BYTE * pData,int nlen);

	/*
	* The FCS-32 generator polynomial: x**0 + x**1 + x**2 + x**4 + x**5
	*                      + x**7 + x**8 + x**10 + x**11 + x**12 + x**16
	*                      + x**22 + x**23 + x**26 + x**32.
	*/
	CPF_EXPORT
		BOOL PPP_FCSCheck32(const BYTE * pData,int nlen);

	CPF_EXPORT
		unsigned int GetFCS32(const BYTE * pData,int nlen);

	CPF_EXPORT
		//返回校验和的网络字节序,nPartSum也是网络字节
		ACE_UINT16 ComputeCheckSum_NetOrder(const BYTE* pPktChecked, int nLenChecked,UINT nPartSum=0);


	CPF_EXPORT
		//返回校验和的主机字节序,nPartSum也是主机字节
	inline ACE_UINT16 ComputeCheckSum(const BYTE* pPktChecked, int nLenChecked,UINT nPartSum=0)
	{
		return ACE_NTOHS(ComputeCheckSum_NetOrder(pPktChecked,nLenChecked,ACE_HTONS(nPartSum)));
	}

	
	CPF_EXPORT
		//返回校验和的网络字节序
	ACE_UINT16 Calculate_trans_CheckSum_NetOrder(ACE_UINT32 dwNetOrderIPSrc,ACE_UINT32 dwNetOrderIPDst,
		BYTE type,const BYTE * pTransHeader,size_t trans_datalen);


	CPF_EXPORT
		//返回校验和的主机字节序
	inline ACE_UINT16 Calculate_trans_CheckSum(ACE_UINT32 dwNetOrderIPSrc,ACE_UINT32 dwNetOrderIPDst,
			BYTE type,const BYTE * pTransHeader,size_t trans_datalen)
	{
		return ACE_NTOHS(Calculate_trans_CheckSum_NetOrder(
			dwNetOrderIPSrc,dwNetOrderIPDst,type,pTransHeader,trans_datalen));
	}

	CPF_EXPORT
		//返回校验和的主机字节序
		inline ACE_UINT16 CalculateTCPCheckSum(ACE_UINT32 dwNetOrderIPSrc,ACE_UINT32 dwNetOrderIPDst,
		const BYTE * pTCPHeader,size_t tcp_datalen)
	{
		return CPF::Calculate_trans_CheckSum(dwNetOrderIPSrc,dwNetOrderIPDst,6,pTCPHeader,tcp_datalen);
	}


	CPF_EXPORT
		//返回校验和的网络字节序
		inline ACE_UINT16 CalculateTCPCheckSum_NetOrder(ACE_UINT32 dwNetOrderIPSrc,ACE_UINT32 dwNetOrderIPDst,
			const BYTE * pTCPHeader,size_t tcp_datalen)
	{
		return CPF::Calculate_trans_CheckSum_NetOrder(dwNetOrderIPSrc,dwNetOrderIPDst,6,pTCPHeader,tcp_datalen);
	}

	CPF_EXPORT
		//返回校验和的主机字节序
		inline ACE_UINT16 CalculateUDPCheckSum(ACE_UINT32 dwNetOrderIPSrc,ACE_UINT32 dwNetOrderIPDst,
		const BYTE * pUDPHeader,size_t udp_datalen)
	{
		return CPF::Calculate_trans_CheckSum(dwNetOrderIPSrc,dwNetOrderIPDst,17,pUDPHeader,udp_datalen);
	}

	CPF_EXPORT
		//返回校验和的网络节序
		inline ACE_UINT16 CalculateUDPCheckSum_NetOrder(ACE_UINT32 dwNetOrderIPSrc,ACE_UINT32 dwNetOrderIPDst,
			const BYTE * pUDPHeader,size_t udp_datalen)
	{
		return CPF::Calculate_trans_CheckSum_NetOrder(dwNetOrderIPSrc,dwNetOrderIPDst,17,pUDPHeader,udp_datalen);
	}


	/*
	* Generate a table of CRC-7 syndromes for x^7 * each possible input byte
	: x^7 + x^3 + 1 
	*/
	CPF_EXPORT
		void crc7_gen_syndrome_table(BYTE *btCrc7Table, UINT dwBuffLen);

	/*
	* Insert the CRC-7 into a 16-byte E.164 path trace message
	*/
	CPF_EXPORT
		void crc7_insert_check_result(BYTE *path_trace_message);

	/*
	* Check whether a 16-byte E.164 path trace message has a valid CRC-7 field
	*/
	CPF_EXPORT
		BOOL crc7_whether_OK (const BYTE *path_trace_message);

	//zero_do如果以前的校验和伪0是否继续计算

	/*incremental update IP ,TCP, UDP checksum, implemented in Assembly according
	with RFC 1624, used subtraction to update checksum*/
	CPF_EXPORT
		unsigned short csum_incremental_update3(
		unsigned short old_csum, 
		unsigned short old_field, 
		unsigned short new_field);

	/*implemented according with RFC 1071 and 1141*/
	//有可能出现计算结果为0xFFFF的情形
	CPF_EXPORT
		unsigned short csum_incremental_update1(
		unsigned short old_csum, 
		unsigned short old_field, 
		unsigned short new_field);

	/*implemented according with RFC 1624, modified the algorithm from RFC 1071 and 1141*/
	//该方法和公式1给出的方法很明显多出了两次取反操作,效率上并没有公式1的高，这个也经过了实际的测试。
	CPF_EXPORT
	 unsigned short csum_incremental_update2(
		unsigned short old_csum, 
		unsigned short old_field, 
		unsigned short new_field);

	CPF_EXPORT
	unsigned short csum_incremental_update_ip3(
		unsigned short old_csum, 
		unsigned int old_nip, 
		unsigned int new_nip);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#endif//_CRC_TOOLS_H_ZHUZP_2006_3_12

