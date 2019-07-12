//////////////////////////////////////////////////////////////////////////
//LMB_serial_class.cpp

#include "stdafx.h"
#include "LMB_serial_class.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
/////////////////////////////

const struct LMB_SERIAL_CMD_INFO LMB_SCREEN_SERIAL::g_LMB_com_command_list[LMB_SERIAL_CMD_MAX]=
{
	// ���� ,LMB_SERIAL_CMD_CLEAR
	{ {0xF8, 0x01},  2},
	// ��긴λ, LMB_SERIAL_CMD_CURSOR_RETURN
	{ {0xF8, 0x02},  2},
	// д���ı�,����ͷ, LMB_SERIAL_CMD_WRITE_TEXT_HEAD
	{ {0xF8, 0x03},  2},
	// д���ı�,�����β, LMB_SERIAL_CMD_WRITE_TEXT_TAIL
	{ {0xA0},  1},
	// ���ù��λ��(��һ��:0x00~0x0F, �ڶ���:0x40~0x4F), LMB_SERIAL_CMD_SET_CURSOR
	{ {0xF8, 0x80},  2},
	// ���ù��λ�õ��ڶ�����λ, add by xumx 2016-11-15, LMB_SERIAL_CMD_SET_CURSOR_2
	{ {0xF8, 0xC0},  2},
	// ���ù���ƶ�����Ϊ����, LMB_SERIAL_CMD_CURSOR_LEFT_DIR
	{ {0xF8, 0x05},  2},
	// ���ù���ƶ�����Ϊ���ҡ�, LMB_SERIAL_CMD_CURSOR_RIGHT_DIR
	{ {0xF8, 0x06},  2},
	// ��������ƶ�һλ��һ���ַ���λ�ã�, LMB_SERIAL_CMD_CURSOR_MOVE1_LEFT
	{ {0xF8, 0x10},  2},
	// ��������ƶ�һλ��һ���ַ���λ�ã�, LMB_SERIAL_CMD_CURSOR_MOVE1_RIGHT
	{ {0xF8, 0x14},  2},
	// �����������һλ��һ���ַ���λ�ã�, LMB_SERIAL_CMD_SCREEN_MOVE1_LEFT
	{ {0xF8, 0x18},  2},
	// �������ҹ���һλ��һ���ַ���λ�ã�, LMB_SERIAL_CMD_SCREEN_MOVE1_RIGHT
	{ {0xF8, 0x1C},  2},
	// ������, LMB_SERIAL_CMD_BACKGROUND_OPEN
	{ {0xF8, 0x28},  2},
	// ����ر�, LMB_SERIAL_CMD_BACKGROUND_OFF
	{ {0xF8, 0x2C},  2},
	// ��ʾ���, LMB_SERIAL_CMD_SHOW_CURSOR
	{ {0xF8, 0x0F},  2},
	// ���ع��, LMB_SERIAL_CMD_HIDE_CURSOR
	{ {0xF8, 0x0C},  2},
};
/////////////////


LMB_SCREEN_SERIAL::LMB_SCREEN_SERIAL(const char *serial_name) // ���������ڵ�����,����:"COM1"
{
	serial_is_open = 0;
#if defined(OS_WINDOWS) && !defined(USE_ACE)
	serial_handle = NULL;
#elif defined(OS_LINUX) && !defined(USE_ACE)
	serial_handle = 0;
#elif defined(USE_ACE)
	ace_read_dev.dump();
	ace_con.dump();
#endif

	if(serial_open(serial_name) > 0){ serial_is_open = 1; }
	else{ serial_is_open = 0; }

	m_pShm = NULL;
}

LMB_SCREEN_SERIAL::LMB_SCREEN_SERIAL()
{
	serial_is_open = 0;
#if defined(OS_WINDOWS) && !defined(USE_ACE)
	serial_handle = NULL;
#elif defined(OS_LINUX) && !defined(USE_ACE)
	serial_handle = 0;
#elif defined(USE_ACE)
	ace_read_dev.dump();
	ace_con.dump();
#endif

	m_pShm = NULL;
}

LMB_SCREEN_SERIAL::~LMB_SCREEN_SERIAL()
{
	Close_serial();

	f_DeleteShareMem();
	//f_FreeShareMem();
}
///////////////

////////////////////

// ��һ������, �ɹ�����>0��ֵ��ʧ�ܷ���<0��ֵ��
#if defined(OS_WINDOWS) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::serial_open
(
	const char *serial_name,                     // ���������ڵ�����,����:"COM1"
	enum SERIAL_RATIO_TYPE ratio,          // ������
	enum SERIAL_PARITY_TYPE parity,        // ��żУ��
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
	enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
)
{
	// �رվɴ���
	if((serial_is_open == 1) && (serial_handle != NULL)){ CloseHandle(serial_handle); }
	serial_is_open = 0;
	serial_handle = 0;

	// ��LMB���Ĵ���
	serial_handle =
		CreateFile
		(
			(LPCTSTR)serial_name, // ��������
			GENERIC_READ | GENERIC_WRITE, //�������д
			0, //��ռ��ʽ
			NULL, // ��ȫ���ԣ�ȱʡֵ
			OPEN_EXISTING, // �򿪶����Ǵ���
			0, // ������ͬ�� I/O
			NULL
		);
	if (serial_handle == INVALID_HANDLE_VALUE){ serial_handle = 0; return(-1); }
	serial_is_open = 1;

	// ���ô���
	int result = GetCommState(serial_handle, &serial_param);
	if (result == false)
	{
		CloseHandle(serial_handle); // �رմ���
		serial_handle = 0;
		serial_is_open = 0;
		return(-2);
	}

	Set_serial_param(ratio, parity, ByteSize, StopBit);
	PurgeComm(serial_handle, PURGE_TXCLEAR | PURGE_RXCLEAR); // ��ջ�����

	return(1);
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::serial_open
(
	const char *serial_name,               // ���������ڵ�����,����:"/dev/ttyS0"
	enum SERIAL_RATIO_TYPE ratio,          // ������
	enum SERIAL_PARITY_TYPE parity,        // ��żУ��
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
	enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
)
{
	// �رվɴ���
	if ((serial_is_open == 1) && (serial_handle > 0)) { close(serial_handle); }
	serial_is_open = 0;
	serial_handle = 0;

	// ��LMB���Ĵ���
	serial_handle = open(serial_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_handle == -1){ serial_handle = 0; return(-1); }
	fcntl(serial_handle, F_SETFL, FNDELAY);
	serial_is_open = 1;

	// ���ô���
	tcgetattr(serial_handle, &serial_param);
	Set_serial_param(ratio, parity, ByteSize, StopBit);

	return(1);
}
#elif defined(USE_ACE)
int LMB_SCREEN_SERIAL::serial_open
(
	const char *serial_name,               // ���������ڵ�����,����:linux: "/dev/ttyS0", windows: "COM1"
	enum SERIAL_RATIO_TYPE ratio,          // ������
	enum SERIAL_PARITY_TYPE parity,        // ��żУ��
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
	enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
)
{
	// �رվɴ���
	ace_read_dev.dump();
	ace_con.dump();

	// ��LMB���Ĵ���
	int result = ace_con.connect
	(
		ace_read_dev,
		ACE_DEV_Addr(serial_name)
	);
	if (result < 0) { return(-1); }
	serial_is_open = 1;

	// ���ô���
	Set_serial_param(ratio, parity, ByteSize, StopBit);

	// ��������
	result = ace_read_dev.control
	(
		ACE_TTY_IO::SETPARAMS,
		&serial_param
	);
	if (result < 0) { serial_is_open = 0; return(-1); }

	return(1);
}
#endif
////////////////////////////////

// ���ò���, ���ڴ򿪳ɹ�ʱ�����ܵ��ô˺���.
#if defined(OS_WINDOWS) && !defined(USE_ACE)
void LMB_SCREEN_SERIAL::Set_serial_param
(
	enum SERIAL_RATIO_TYPE ratio,          // ������
	enum SERIAL_PARITY_TYPE parity,        // ��żУ��
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
	enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
)
{
	if((serial_handle == 0) || (serial_is_open != 1)){ return; }

	// ������
	switch(ratio)
	{
	case SRT_2400:
		serial_param.BaudRate = 2400;//������Ϊ 2400
	break;

	case SRT_4800:
		serial_param.BaudRate = 4800;//������Ϊ 4800
	break;

	case SRT_9600:
		serial_param.BaudRate = 9600;//������Ϊ 9600
	break;

	case SRT_19200:
		serial_param.BaudRate = 19200;
	break;

	case SRT_57600:
		serial_param.BaudRate = 57600;
	break;

	case SRT_115200:
		serial_param.BaudRate = 115200;//������Ϊ 115200
	break;

	default:
		serial_param.BaudRate = 9600;//������Ϊ 9600
	break;
	}
	
	// ����λ����
	switch(ByteSize)
	{
	case SBT_7:  // 7bit
		serial_param.ByteSize = 7;//ÿ���ֽ��� 7 λ
	break;

	case SBT_8:  // 8bit
		serial_param.ByteSize = 8;//ÿ���ֽ��� 8 λ
	break;

	default:
		serial_param.ByteSize = 8;//ÿ���ֽ��� 8 λ
	break;
	}
	
	// ��żУ��
	switch(parity)
	{
	case SPT_O: // ����У��
		serial_param.Parity = ODDPARITY;
	break;

	case SPT_E: // ż��У��
		serial_param.Parity = EVENPARITY;
	break;
	
	case SPT_N: // ����żУ��
		serial_param.Parity = NOPARITY;
	break;

	default:
		serial_param.Parity = NOPARITY;
	break;
	}
	
	// ֹͣλ
	switch(StopBit)
	{
	case SST_0:  // ֹͣλ 1
		serial_param.StopBits = ONESTOPBIT;
	break;

	case SST_1:  // ֹͣλ 1.5
		serial_param.StopBits = ONE5STOPBITS;
	break;

	case SST_2:  // ֹͣλ 2
		serial_param.StopBits = TWOSTOPBITS;
	break;

	default:
		serial_param.StopBits = TWOSTOPBITS;
	break;
	}
	
	SetCommState(serial_handle, &serial_param);
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
void LMB_SCREEN_SERIAL::Set_serial_param
(
	enum SERIAL_RATIO_TYPE ratio,          // ������
	enum SERIAL_PARITY_TYPE parity,        // ��żУ��
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
	enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
)
{
	if ((serial_handle == 0) || (serial_is_open != 1)) { return; }

	// ������
	switch (ratio)
	{
	case SRT_2400: //������Ϊ 2400
		cfsetispeed(&serial_param, B2400);
		cfsetospeed(&serial_param, B2400);
	break;

	case SRT_4800: //������Ϊ 4800
		cfsetispeed(&serial_param, B4800);
		cfsetospeed(&serial_param, B4800);
	break;

	case SRT_9600: //������Ϊ 9600
		cfsetispeed(&serial_param, B9600);
		cfsetospeed(&serial_param, B9600);
	break;

	case SRT_19200:
		cfsetispeed(&serial_param, B19200);
		cfsetospeed(&serial_param, B19200);
	break;

	case SRT_57600:
		cfsetispeed(&serial_param, B57600);
		cfsetospeed(&serial_param, B57600);
	break;

	case SRT_115200: //������Ϊ 115200
		cfsetispeed(&serial_param, B115200);
		cfsetospeed(&serial_param, B115200);
	break;

	default: //������Ϊ 9600
		cfsetispeed(&serial_param, B9600);
		cfsetospeed(&serial_param, B9600);
	break;
	}

	// ������ģʽ (CLOCAL)�ʹ������ݽ���(CREAD)����Ϊ��Ч.
	serial_param.c_cflag |= (CLOCAL | CREAD);

	// ����λ����
	serial_param.c_cflag &= ~CSIZE;
	switch (ByteSize)
	{
	case SBT_7:  // 7bit
		serial_param.c_cflag |= CS7;//ÿ���ֽ��� 7 λ
	break;

	case SBT_8:  // 8bit
		serial_param.c_cflag |= CS8;//ÿ���ֽ��� 8 λ
	break;

	default:
		serial_param.c_cflag |= CS8;//ÿ���ֽ��� 8 λ
	break;
	}

	// ��żУ��
	switch (parity)
	{
	case SPT_O: // ����У��
		serial_param.c_cflag |= PARENB;
		serial_param.c_cflag |= PARODD;
		serial_param.c_cflag &= ~CSTOPB;
	break;

	case SPT_E: // ż��У��
		serial_param.c_cflag |= PARENB;
		serial_param.c_cflag &= ~PARODD;
		serial_param.c_cflag &= ~CSTOPB;
	break;

	default:
	case SPT_N: // ����żУ��
		serial_param.c_cflag &= ~PARENB;
		serial_param.c_cflag &= ~CSTOPB;
	break;
	}

	// ֹͣλ
	switch (StopBit)
	{
	case SST_0:  // ֹͣλ 1
		serial_param.c_cflag &= ~CSTOPB;
	break;

	case SST_2:  // ֹͣλ 2
		serial_param.c_cflag |= CSTOPB;
	break;

	default:
		serial_param.c_cflag &= ~CSTOPB;
	break;
	}

	// ����������Ч
	tcsetattr(serial_handle, TCSANOW, &serial_param);
}
#elif defined(USE_ACE)
void LMB_SCREEN_SERIAL::Set_serial_param
(
	enum SERIAL_RATIO_TYPE ratio,          // ������
	enum SERIAL_PARITY_TYPE parity,        // ��żУ��
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
	enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
)
{
	if (serial_is_open != 1) { return; }

	// ������
	switch (ratio)
	{
	case SRT_2400: //������Ϊ 2400
		serial_param.baudrate = 2400;
	break;

	case SRT_4800: //������Ϊ 4800
		serial_param.baudrate = 4800;
	break;

	case SRT_9600: //������Ϊ 9600
		serial_param.baudrate = 9600;
	break;

	case SRT_19200:
		serial_param.baudrate = 19200;
	break;

	case SRT_57600:
		serial_param.baudrate = 57600;
	break;

	case SRT_115200: //������Ϊ 115200
		serial_param.baudrate = 115200;
	break;

	default: //������Ϊ 9600
		serial_param.baudrate = 9600;
	break;
	}
	
	// ��XON�ַ�������֮ǰ�����뻺��������С�ֽ�
	serial_param.xonlim = 0;
	// ��XOFF�ַ�������֮ǰ�����뻺����������ֽ�
	serial_param.xofflim = 0;
	// ����POSIX�Ǳ�׼�Ķ����ٸ������ַ�
	serial_param.readmincharacters = 0;
	// �ڴ�read����֮ǰ���ȴ���ʱ��
	//serial_param.readtimeoutmsec = 1000; // 1��
	serial_param.readtimeoutmsec = -1; // 1��

	// ��żУ��
	switch (parity)
	{
	case SPT_O: // ����У��
		serial_param.paritymode = "odd";
	break;

	case SPT_E: // ż��У��
		serial_param.paritymode = "even";
	break;

	default:
	case SPT_N: // ����żУ��
		serial_param.paritymode = "none";
	break;
	}

	// �Ƿ�֧��CTSģʽ��ȷ���Ƿ��ͨ����ѯ Clear To Send(CTS) �ߵ�״̬�������ݡ�
	serial_param.ctsenb = FALSE;
	// ʹ�ú�����RTS��0��ʾ��ʹ��RTS 1��ʾʹ��RTS 2��ʾʹ��RTS����������(win32)
	// 3��ʾ��������㹻�����ڴ�����ֽڣ�RTS�߱�ߡ�����֮��RTS��ɵ�(win32)
	serial_param.rtsenb = 0;
	// �Ƿ���������ʹ�����������
	serial_param.xinenb = FALSE;
	// �Ƿ��������ʹ�����������
	serial_param.xoutenb = FALSE;
	// �豸�Ƿ���POSIX����ģʽ
	serial_param.modem = FALSE;
	// �Ƿ�ʹ��receiver(POSIX)
	serial_param.rcvenb = TRUE;
	// Data Set Ready(DSR) �ߵ�״̬�Ƿ�������(WIN32)
	serial_param.dsrenb = FALSE;
	// Data Terminal Ready״̬�Ƿ�������
	serial_param.dtrdisable = FALSE;

	// ����λ���͡��Ϸ�ֵΪ5, 6, 7, 8��Win32��֧��4λ
	switch (ByteSize)
	{
	case SBT_7:  // 7bit
		serial_param.databits = 7;//ÿ���ֽ��� 7 λ
	break;

	case SBT_8:  // 8bit
		serial_param.databits = 8;//ÿ���ֽ��� 8 λ
	break;

	default:
		serial_param.databits = 8;//ÿ���ֽ��� 8 λ
	break;
	}

	// ֹͣλ���Ϸ�ֵΪ1��2
	switch (StopBit)
	{
	case SST_0:  // ֹͣλ 1
		serial_param.stopbits = 1;
		break;

	case SST_2:  // ֹͣλ 2
		serial_param.stopbits = 1;
	break;

	default:
		serial_param.stopbits = 1;
	break;
	}
}
#endif
///////////////////////////////////

// �رմ���
#if defined(OS_WINDOWS) && !defined(USE_ACE)
void LMB_SCREEN_SERIAL::Close_serial()
{
	if((serial_handle != 0) || (serial_is_open == 1)){ CloseHandle(serial_handle); }

	serial_handle = NULL;
	serial_is_open = 0;
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
void LMB_SCREEN_SERIAL::Close_serial()
{
	if ((serial_handle > 0) || (serial_is_open == 1)) { close(serial_handle); }

	serial_handle = 0;
	serial_is_open = 0;
}
#elif defined(USE_ACE)
void LMB_SCREEN_SERIAL::Close_serial()
{
	ace_read_dev.dump();
	ace_con.dump();
	serial_is_open = 0;
	ace_read_dev.close();  // add by xumx 2016-10-21
}
#endif
///////////////////////////////////

// �����Ƿ�򿪳ɹ�,�ɹ�����>0��ֵ,ʧ�ܷ���<0��ֵ
#if defined(OS_WINDOWS) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::Is_serial_successed()
{
	if((serial_handle != 0) || (serial_is_open == 1)){ return(1); }
	else{ return(-1); }
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::Is_serial_successed()
{
	if ((serial_handle > 0) || (serial_is_open == 1)) { return(1); }
	else { return(-1); }
}
#elif defined(USE_ACE)
int LMB_SCREEN_SERIAL::Is_serial_successed()
{
	if(serial_is_open == 1) { return(1); }
	return(-1);
}
#endif
///////////////////////////////////////



// ���ݴ�������ID��������
#if defined(OS_WINDOWS) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::API_LMB_operate_by_cmdID
(
	int serial_cmd_id,     // ����ID
	void *write_buff,      // д������
	unsigned int write_byte   // д������
)
{
	DWORD real_write_byte;
	BOOL w_result;
	/////////////

	// д������
	w_result =
	WriteFile
	(
		serial_handle, // ���ھ��
		g_LMB_com_command_list[serial_cmd_id].cmd_name, // ��������
		g_LMB_com_command_list[serial_cmd_id].cmd_name_length, // �����
		&real_write_byte, // ʵ��д���ֽ���
		NULL
	);

	if(w_result == false){ return(-1); }

	// д������
	if((write_buff == NULL) ||(write_byte == 0)){ return(1); }
	w_result =
	WriteFile
	(
		serial_handle, // ���ھ��
		write_buff, // ��������
		write_byte, // ���ݳ���
		&real_write_byte, // ʵ��д���ֽ���
		NULL
	);

	if(w_result == false){ return(-1); }

	return(1);
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::API_LMB_operate_by_cmdID
(
	int serial_cmd_id,     // ����ID
	void *write_buff,      // д������
	unsigned int write_byte   // д������
)
{
	int w_result;

	// д������
	w_result = 
	write
	(
		serial_handle,
		g_LMB_com_command_list[serial_cmd_id].cmd_name, // ��������
		g_LMB_com_command_list[serial_cmd_id].cmd_name_length // �����
	);

	if (w_result < 0) { return(-1); }

	// д������
	if ((write_buff == NULL) || (write_byte == 0)) { return(1); }
	w_result =
	write
	(
		serial_handle,
		write_buff, // ��������
		write_byte // �����
	);
	if (w_result == false) { return(-1); }

	return(1);
}
#elif defined(USE_ACE)
int LMB_SCREEN_SERIAL::API_LMB_operate_by_cmdID
(
	int serial_cmd_id,     // ����ID
	void *write_buff,      // д������
	unsigned int write_byte   // д������
)
{
	int result = 0;

	// д������
	result = (int)ace_read_dev.send
	(
		g_LMB_com_command_list[serial_cmd_id].cmd_name, // ��������
		g_LMB_com_command_list[serial_cmd_id].cmd_name_length // �����
	);
	if (result < 0) { return(-1); }

	// д������
	if ((write_buff == NULL) || (write_byte == 0)) { return(1); }

	result = (int)ace_read_dev.send
	(
		write_buff, // ��������
		write_byte  // �����
	);
	if (result < 0) { return(-1); }

	return(1);
}
#endif

// Ӧ�ò�Ĳ����ӿ�
int LMB_SCREEN_SERIAL::API_LMB_OPERATE
(
	int operate_type,          // �������ͣ��ο�enum SERIAL_OPERATE_ID
	void *write_text,          // д������
	unsigned int write_byte    // д�����ݳ���
)
{
	////if(Is_serial_successed() < 0){ return(-1); }

	int result=0;

	switch(operate_type)
	{
		case SERIAL_OPERATE_CLEAR_SCREEN: // ����
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CLEAR, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_RETURN: // ��긴λ
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_RETURN, NULL, 0);
			break;

		case SERIAL_OPERATE_WRITE_TEXT: // д���ı�
			if((write_text == NULL) || (write_byte == 0)){ return(1); }
			
			// ����ͷ
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_WRITE_TEXT_HEAD, write_text, write_byte);
			if(result < 0){ return(-1); }


			// �����β
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_WRITE_TEXT_TAIL, NULL, 0);
			if(result < 0){ return(-1); }
			break;

		case SERIAL_OPERATE_SET_CURSOR: // ���ù��λ��
			#if 0
				if((write_text == NULL) || (write_byte != 1)){ return(-1); }
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR, write_text, write_byte);
			#else
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR, NULL, 0);
			#endif
			break;

		case SERIAL_OPERATE_SET_CURSOR_2: // ���ù��λ�õ��ڶ�����λ
			#if 0
				if((write_text == NULL) || (write_byte != 1)){ return(-1); }
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR_2, write_text, write_byte);
			#else
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR_2, NULL, 0);
			#endif
			break;

		case SERIAL_OPERATE_CURSOR_LEFT_DIR: // ���ù���ƶ�����Ϊ����
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_LEFT_DIR, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_RIGHT_DIR: // ���ù���ƶ�����Ϊ���ҡ�
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_RIGHT_DIR, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_MOVE1_LEFT:  // ��������ƶ�һλ��һ���ַ���λ�ã�
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_MOVE1_LEFT, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_MOVE1_RIGHT:  // ��������ƶ�һλ��һ���ַ���λ�ã�
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_MOVE1_RIGHT, NULL, 0);
			break;

		case SERIAL_OPERATE_SCREEN_MOVE1_LEFT:  // �����������һλ��һ���ַ���λ�ã�
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SCREEN_MOVE1_LEFT, NULL, 0);
			break;

		case SERIAL_OPERATE_SCREEN_MOVE1_RIGHT:  // �������ҹ���һλ��һ���ַ���λ�ã�
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SCREEN_MOVE1_RIGHT, NULL, 0);
			break;

		case SERIAL_OPERATE_BACKGROUND_OPEN:    // ������
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_BACKGROUND_OPEN, NULL, 0);
			break;

		case SERIAL_OPERATE_BACKGROUND_OFF:    // ����ر�
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_BACKGROUND_OFF, NULL, 0);
			break;

		case SERIAL_OPERATE_SHOW_CURSOR:      // ��ʾ���
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SHOW_CURSOR, NULL, 0);
			break;

		case SERIAL_OPERATE_HIDE_CURSOR:      // ���ع��
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_HIDE_CURSOR, NULL, 0);
			break;

		default: return(-1);
	}

	if(result < 0){ return(-1); }
	return(1);
}

///////////////////////////////////////////
int LMB_SCREEN_SERIAL::f_CreateShareMem(void)
{
	if (NULL == m_pShm)
	{
		// ���������ڴ�
		key_t LMB_shm_key = 9979;
		unsigned int memory_size = sizeof(LMB_SHARE_MEM_S);

		int LMB_shmid = shmget(LMB_shm_key, memory_size, 0666|IPC_CREAT);
		if (0 > LMB_shmid)
		{
			return FALSE; 
		}


		 m_pShm = (LMB_SHARE_MEM_S*)shmat(LMB_shmid, NULL, 0);
		 if ((LMB_SHARE_MEM_S*)-1 == m_pShm)
		 {
		 	m_pShm = NULL;
		 	return FALSE;
		 }

		 memset((char*)m_pShm, 0, sizeof(LMB_SHARE_MEM_S));
	 }

	return TRUE;
}

int LMB_SCREEN_SERIAL::f_MapShareMem(void)
{
	if (NULL == m_pShm)
	{
		key_t LMB_shm_key = 9979;
		
		int LMB_shmid = shmget(LMB_shm_key, 0, 0);
		if (0 > LMB_shmid)
		{
			return FALSE; 
		}

		//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�  
		m_pShm = (LMB_SHARE_MEM_S*)shmat(LMB_shmid, NULL, 0);
		 if ((LMB_SHARE_MEM_S*)-1 == m_pShm)
		 {
		 	m_pShm = NULL;
		 	return FALSE;
		 }
	 }
	 
	return TRUE;
}

int LMB_SCREEN_SERIAL::f_DeleteShareMem(void)
{
	if (NULL != m_pShm)
	{
		if (0 > shmdt(m_pShm))
		{
			return FALSE;
		}

		m_pShm = NULL;
	}

	return TRUE;
}

int LMB_SCREEN_SERIAL::f_FreeShareMem(void)
{
	
	key_t LMB_shm_key = 9979;
	
	int LMB_shmid = shmget(LMB_shm_key, 0, 0);
	if (0 > LMB_shmid)
	{
		return FALSE; 
	}

	shmctl(LMB_shmid, IPC_RMID, NULL);
	m_pShm = NULL;

	return TRUE;
}

int LMB_SCREEN_SERIAL::f_AddOneLine(char* usr_string)
{
	if (LMB_SCREEN_MAX_LINE <= m_pShm->nTotalLineNum)
	{
		return FALSE;
	}

	char* pOffset = m_pShm->cData + LMB_SCREEN_LINE_WIDTH*(m_pShm->nTotalLineNum);

	memset(pOffset, ' ', LMB_SCREEN_LINE_WIDTH);
		
	if (usr_string)
	{
		int length = strlen(usr_string);
		if (LMB_SCREEN_LINE_WIDTH < length)
		{
			length = LMB_SCREEN_LINE_WIDTH;
		}
		
		memcpy(pOffset, usr_string, length);
	}

	m_pShm->nTotalLineNum++;

	return TRUE;
}

int LMB_SCREEN_SERIAL::f_WriteOneLine(char* usr_string, unsigned int line_num)
{
	if (line_num >= m_pShm->nTotalLineNum)
	{
		return FALSE;
	}

	char* pOffset = m_pShm->cData + LMB_SCREEN_LINE_WIDTH*line_num;

	if (usr_string)
	{
		int length = strlen(usr_string);
		if (LMB_SCREEN_LINE_WIDTH > length)
		{
			memcpy(pOffset, usr_string, length);
			memset(pOffset+length, ' ', LMB_SCREEN_LINE_WIDTH-length);
		}
		else
		{
			memcpy(pOffset, usr_string, LMB_SCREEN_LINE_WIDTH);
		}

	}

	return TRUE;
}

unsigned int LMB_SCREEN_SERIAL::f_GetCurLine()
{
	return m_pShm->nCurrentLineNum;
}

int LMB_SCREEN_SERIAL::f_ScreenTimeout(unsigned int timer)
{
	if (timer)
	{
		m_pShm->nLightTimer = timer;
	}
	else
	{
		if (m_pShm->nLightTimer)
		{
			m_pShm->nLightTimer--;
			if (0 == m_pShm->nLightTimer)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void LMB_SCREEN_SERIAL::f_ScreenMoveUp(void)
{
	if (1 <= m_pShm->nCurrentLineNum)
	{
		m_pShm->nCurrentLineNum--;
	}
}

void LMB_SCREEN_SERIAL::f_ScreenMoveDown(void)
{
	if ((m_pShm->nTotalLineNum - LMB_SCREEN_LINE_HEIGHT) > m_pShm->nCurrentLineNum)
	{
		m_pShm->nCurrentLineNum++;
	}
}

void LMB_SCREEN_SERIAL::f_ScreenPrintf(void)
{	
	char* pLine1;
	char* pLine2;
	
	pLine1 = m_pShm->cData + LMB_SCREEN_LINE_WIDTH*(m_pShm->nCurrentLineNum);
	pLine2 = pLine1 + LMB_SCREEN_LINE_WIDTH;
	
	API_LMB_OPERATE(SERIAL_OPERATE_CURSOR_RETURN, NULL, 0);  // ��һ�ж�λ��Ҫ�á���긴λ��ָ��(�ɵ�����ʹ��)
	API_LMB_OPERATE(SERIAL_OPERATE_SET_CURSOR, NULL, 0); // ������ʹ��
	API_LMB_OPERATE(SERIAL_OPERATE_WRITE_TEXT, pLine1, LMB_SCREEN_LINE_WIDTH);

	API_LMB_OPERATE(SERIAL_OPERATE_SET_CURSOR_2, NULL, 0);
	API_LMB_OPERATE(SERIAL_OPERATE_WRITE_TEXT, pLine2, LMB_SCREEN_LINE_WIDTH);
}

