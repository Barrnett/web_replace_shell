//////////////////////////////////////////////////////////////////////////
//EtherealWritefile.h

#pragma once

#include "cpf/ostypedef.h"

//#ifdef OS_WINDOWS

#include "PacketIO/PacketIO.h"

#ifndef __WTAP_H__

/* Encapsulation types. Choose names that truly reflect
* what is contained in the packet trace file.
*
* WTAP_ENCAP_PER_PACKET is a value passed to "wtap_dump_open()" or
* "wtap_dump_fd_open()" to indicate that there is no single encapsulation
* type for all packets in the file; this may cause those routines to
* fail if the capture file format being written can't support that.
* It's also returned by "wtap_file_encap()" for capture files that
* don't have a single encapsulation type for all packets in the file.
*
* WTAP_ENCAP_UNKNOWN is returned by "wtap_pcap_encap_to_wtap_encap()"
* if it's handed an unknown encapsulation.
*
* WTAP_ENCAP_FDDI_BITSWAPPED is for FDDI captures on systems where the
* MAC addresses you get from the hardware are bit-swapped.  Ideally,
* the driver would tell us that, but I know of none that do, so, for
* now, we base it on the machine on which we're *reading* the
* capture, rather than on the machine on which the capture was taken
* (they're probably likely to be the same).  We assume that they're
* bit-swapped on everything except for systems running Ultrix, Alpha
* systems, and BSD/OS systems (that's what "tcpdump" does; I guess
* Digital decided to bit-swap addresses in the hardware or in the
* driver, and I guess BSDI bit-swapped them in the driver, given that
* BSD/OS generally runs on Boring Old PC's).  If we create a wiretap
* save file format, we'd use the WTAP_ENCAP values to flag the
* encapsulation of a packet, so there we'd at least be able to base
* it on the machine on which the capture was taken.
*
* WTAP_ENCAP_LINUX_ATM_CLIP is the encapsulation you get with the
* ATM on Linux code from <http://linux-atm.sourceforge.net/>;
* that code adds a DLT_ATM_CLIP DLT_ code of 19, and that
* encapsulation isn't the same as the DLT_ATM_RFC1483 encapsulation
* presumably used on some BSD systems, which we turn into
* WTAP_ENCAP_ATM_RFC1483.
*
* WTAP_ENCAP_NULL corresponds to DLT_NULL from "libpcap".  This
* corresponds to
*
*	1) PPP-over-HDLC encapsulation, at least with some versions
*	   of ISDN4BSD (but not the current ones, it appears, unless
*	   I've missed something);
*
*	2) a 4-byte header containing the AF_ address family, in
*	   the byte order of the machine that saved the capture,
*	   for the packet, as used on many BSD systems for the
*	   loopback device and some other devices, or a 4-byte header
*	   containing the AF_ address family in network byte order,
*	   as used on recent OpenBSD systems for the loopback device;
*
*	3) a 4-byte header containing 2 octets of 0 and an Ethernet
*	   type in the byte order from an Ethernet header, that being
*	   what older versions of "libpcap" on Linux turn the Ethernet
*	   header for loopback interfaces into (0.6.0 and later versions
*	   leave the Ethernet header alone and make it DLT_EN10MB). */
#define WTAP_ENCAP_PER_PACKET			-1
#define WTAP_ENCAP_UNKNOWN			0
#define WTAP_ENCAP_ETHERNET			1
#define WTAP_ENCAP_TOKEN_RING			2
#define WTAP_ENCAP_SLIP				3
#define WTAP_ENCAP_PPP				4
#define WTAP_ENCAP_FDDI				5
#define WTAP_ENCAP_FDDI_BITSWAPPED		6
#define WTAP_ENCAP_RAW_IP			7
#define WTAP_ENCAP_ARCNET			8
#define WTAP_ENCAP_ARCNET_LINUX			9
#define WTAP_ENCAP_ATM_RFC1483			10
#define WTAP_ENCAP_LINUX_ATM_CLIP		11
#define WTAP_ENCAP_LAPB				12
#define WTAP_ENCAP_ATM_PDUS			13
#define WTAP_ENCAP_ATM_PDUS_UNTRUNCATED		14
#define WTAP_ENCAP_NULL				15
#define WTAP_ENCAP_ASCEND			16
#define WTAP_ENCAP_ISDN				17
#define WTAP_ENCAP_IP_OVER_FC			18
#define WTAP_ENCAP_PPP_WITH_PHDR		19
#define WTAP_ENCAP_IEEE_802_11			20
#define WTAP_ENCAP_PRISM_HEADER			21
#define WTAP_ENCAP_IEEE_802_11_WITH_RADIO	22
#define WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP	23
#define WTAP_ENCAP_IEEE_802_11_WLAN_AVS		24
#define WTAP_ENCAP_SLL				25
#define WTAP_ENCAP_FRELAY			26
#define WTAP_ENCAP_FRELAY_WITH_PHDR		27
#define WTAP_ENCAP_CHDLC			28
#define WTAP_ENCAP_CISCO_IOS			29
#define WTAP_ENCAP_LOCALTALK			30
#define WTAP_ENCAP_OLD_PFLOG			31
#define WTAP_ENCAP_HHDLC			32
#define WTAP_ENCAP_DOCSIS			33
#define WTAP_ENCAP_COSINE			34
#define WTAP_ENCAP_WFLEET_HDLC			35
#define WTAP_ENCAP_SDLC				36
#define WTAP_ENCAP_TZSP				37
#define WTAP_ENCAP_ENC				38
#define WTAP_ENCAP_PFLOG			39
#define WTAP_ENCAP_CHDLC_WITH_PHDR		40
#define WTAP_ENCAP_BLUETOOTH_H4			41
#define WTAP_ENCAP_MTP2				42
#define WTAP_ENCAP_MTP3				43
#define WTAP_ENCAP_IRDA				44
#define WTAP_ENCAP_USER0 			45
#define WTAP_ENCAP_USER1 			46
#define WTAP_ENCAP_USER2 			47
#define WTAP_ENCAP_USER3 			48
#define WTAP_ENCAP_USER4 			49
#define WTAP_ENCAP_USER5 			50
#define WTAP_ENCAP_USER6 			51
#define WTAP_ENCAP_USER7 			52
#define WTAP_ENCAP_USER8 			53
#define WTAP_ENCAP_USER9 			54
#define WTAP_ENCAP_USER10			55
#define WTAP_ENCAP_USER11			56
#define WTAP_ENCAP_USER12			57
#define WTAP_ENCAP_USER13			58
#define WTAP_ENCAP_USER14			59
#define WTAP_ENCAP_USER15			60
#define WTAP_ENCAP_SYMANTEC			61
#define WTAP_ENCAP_APPLE_IP_OVER_IEEE1394	62
#define WTAP_ENCAP_BACNET_MS_TP			63
#define WTAP_ENCAP_RAW_ICMP			64
#define WTAP_ENCAP_RAW_ICMPV6			65
#define WTAP_ENCAP_GPRS_LLC			67

/* last WTAP_ENCAP_ value + 1 */
#define WTAP_NUM_ENCAP_TYPES			67

/* File types that can be read by wiretap.
We support writing some many of these file types, too, so we
distinguish between different versions of them. */
#define WTAP_FILE_UNKNOWN			0
#define WTAP_FILE_WTAP				1
#define WTAP_FILE_PCAP				2
#define WTAP_FILE_PCAP_SS990417			3
#define WTAP_FILE_PCAP_SS990915			4
#define WTAP_FILE_PCAP_SS991029			5
#define WTAP_FILE_PCAP_NOKIA			6
#define WTAP_FILE_PCAP_AIX			7
#define WTAP_FILE_LANALYZER			8
#define WTAP_FILE_NGSNIFFER_UNCOMPRESSED	9
#define WTAP_FILE_NGSNIFFER_COMPRESSED		10
#define WTAP_FILE_SNOOP				11
#define WTAP_FILE_SHOMITI			12
#define WTAP_FILE_IPTRACE_1_0			13
#define WTAP_FILE_IPTRACE_2_0			14
#define WTAP_FILE_NETMON_1_x			15
#define WTAP_FILE_NETMON_2_x			16
#define WTAP_FILE_NETXRAY_OLD			17
#define WTAP_FILE_NETXRAY_1_0			18
#define WTAP_FILE_NETXRAY_1_1			19
#define WTAP_FILE_NETXRAY_2_00x			20
#define WTAP_FILE_RADCOM			21
#define WTAP_FILE_ASCEND			22
#define WTAP_FILE_NETTL				23
#define WTAP_FILE_TOSHIBA			24
#define WTAP_FILE_I4BTRACE			25
#define WTAP_FILE_CSIDS				26
#define WTAP_FILE_PPPDUMP			27
#define WTAP_FILE_ETHERPEEK_V56			28
#define WTAP_FILE_ETHERPEEK_V7			29
#define WTAP_FILE_VMS				30
#define WTAP_FILE_DBS_ETHERWATCH		31
#define WTAP_FILE_VISUAL_NETWORKS		32
#define WTAP_FILE_COSINE			33
#define WTAP_FILE_5VIEWS			34
#define WTAP_FILE_ERF				35
#define WTAP_FILE_HCIDUMP			36
#define WTAP_FILE_NETWORK_INSTRUMENTS_V9	37
#define WTAP_FILE_AIROPEEK_V9			38
#define WTAP_FILE_EYESDN			39

/* last WTAP_FILE_ value + 1 */
#define WTAP_NUM_FILE_TYPES			40

/*
* Maximum packet size we'll support.
* It must be at least 65535.
*/
#define	WTAP_MAX_PACKET_SIZE			65535

#endif

class PACKETIO_CLASS CEtherealWritefile
{
public:
	CEtherealWritefile(void);
	~CEtherealWritefile(void);

	BOOL CreateFile(const char *filename, int filetype, int encap,int snaplen=2000);

	BOOL CreateSnifferEtherFile(const char *filename,int snaplen=2000)
	{
		return CreateFile(filename,WTAP_FILE_NETXRAY_2_00x,WTAP_ENCAP_ETHERNET,snaplen);
	}

	BOOL CreatePcapEtherFile(const char *filename,int snaplen=2000)
	{
		return CreateFile(filename,WTAP_FILE_PCAP,WTAP_ENCAP_ETHERNET,snaplen);
	}

	BOOL WriteData(const char * pdata,int datalen,int caplen,int sec,int ns);
	void Close();

private:
	void *		m_wd;
	int			m_encap;

};

#include "PacketIO/WriteDataStream.h"

class PACKETIO_CLASS CEtherealWriteDataStream : public IFileWriteDataStream
{
public:
	CEtherealWriteDataStream();
	virtual ~CEtherealWriteDataStream();

	//相当于init+OpenDataStream
	BOOL Open(LPCSTR szFileName,int filetype,int encap,int snaplen = 2000);

	//相当于CloseDataStream+fini
	void Close();

	BOOL init(LPCSTR szFileName,int filetype,int encap,int snaplen = 2000);
	void fini();

	//是否支持写控制．
	virtual BOOL IsSupportWriteCtrl() const
	{
		return false;
	}

	virtual IFileWriteDataStream * CloneAfterIni();

	//打开设备．这个函数应该在参数设置好之后调用
	//成功返回0,否则返回-1
	virtual int OpenDataStream();

	//关闭设备
	virtual void CloseDataStream();

	//保存数据，成功返回0,失败返回-1
	virtual int WritePacketToDevWithPacket(PACKET_LEN_TYPE type,const RECV_PACKET& packet,int index_array,int try_nums);

	virtual int GetConnectState();
	virtual bool GetDataStreamName(std::string& name);

private:
	int			m_filetype;
	int			m_encap;
	int			m_snaplen;

	CEtherealWritefile *	m_pEtherealWriteFile;

};

//#endif//#ifdef OS_WINDOWS

