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
	// 清屏 ,LMB_SERIAL_CMD_CLEAR
	{ {0xF8, 0x01},  2},
	// 光标复位, LMB_SERIAL_CMD_CURSOR_RETURN
	{ {0xF8, 0x02},  2},
	// 写入文本,命令头, LMB_SERIAL_CMD_WRITE_TEXT_HEAD
	{ {0xF8, 0x03},  2},
	// 写入文本,命令结尾, LMB_SERIAL_CMD_WRITE_TEXT_TAIL
	{ {0xA0},  1},
	// 设置光标位置(第一行:0x00~0x0F, 第二行:0x40~0x4F), LMB_SERIAL_CMD_SET_CURSOR
	{ {0xF8, 0x80},  2},
	// 设置光标位置到第二行首位, add by xumx 2016-11-15, LMB_SERIAL_CMD_SET_CURSOR_2
	{ {0xF8, 0xC0},  2},
	// 设置光标移动方向为“左”, LMB_SERIAL_CMD_CURSOR_LEFT_DIR
	{ {0xF8, 0x05},  2},
	// 设置光标移动方向为“右”, LMB_SERIAL_CMD_CURSOR_RIGHT_DIR
	{ {0xF8, 0x06},  2},
	// 光标向左移动一位（一个字符的位置）, LMB_SERIAL_CMD_CURSOR_MOVE1_LEFT
	{ {0xF8, 0x10},  2},
	// 光标向右移动一位（一个字符的位置）, LMB_SERIAL_CMD_CURSOR_MOVE1_RIGHT
	{ {0xF8, 0x14},  2},
	// 画面向左滚动一位（一个字符的位置）, LMB_SERIAL_CMD_SCREEN_MOVE1_LEFT
	{ {0xF8, 0x18},  2},
	// 画面向右滚动一位（一个字符的位置）, LMB_SERIAL_CMD_SCREEN_MOVE1_RIGHT
	{ {0xF8, 0x1C},  2},
	// 背光亮, LMB_SERIAL_CMD_BACKGROUND_OPEN
	{ {0xF8, 0x28},  2},
	// 背光关闭, LMB_SERIAL_CMD_BACKGROUND_OFF
	{ {0xF8, 0x2C},  2},
	// 显示光标, LMB_SERIAL_CMD_SHOW_CURSOR
	{ {0xF8, 0x0F},  2},
	// 隐藏光标, LMB_SERIAL_CMD_HIDE_CURSOR
	{ {0xF8, 0x0C},  2},
};
/////////////////


LMB_SCREEN_SERIAL::LMB_SCREEN_SERIAL(const char *serial_name) // 参数：串口的名字,例如:"COM1"
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

// 打开一个串口, 成功返回>0的值，失败返回<0的值。
#if defined(OS_WINDOWS) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::serial_open
(
	const char *serial_name,                     // 参数：串口的名字,例如:"COM1"
	enum SERIAL_RATIO_TYPE ratio,          // 波特率
	enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
	enum SERIAL_STOP_TYPE StopBit          // 停止位
)
{
	// 关闭旧串口
	if((serial_is_open == 1) && (serial_handle != NULL)){ CloseHandle(serial_handle); }
	serial_is_open = 0;
	serial_handle = 0;

	// 打开LMB屏的串口
	serial_handle =
		CreateFile
		(
			(LPCTSTR)serial_name, // 串口名称
			GENERIC_READ | GENERIC_WRITE, //允许读和写
			0, //独占方式
			NULL, // 安全属性，缺省值
			OPEN_EXISTING, // 打开而不是创建
			0, // 必须是同步 I/O
			NULL
		);
	if (serial_handle == INVALID_HANDLE_VALUE){ serial_handle = 0; return(-1); }
	serial_is_open = 1;

	// 配置串口
	int result = GetCommState(serial_handle, &serial_param);
	if (result == false)
	{
		CloseHandle(serial_handle); // 关闭串口
		serial_handle = 0;
		serial_is_open = 0;
		return(-2);
	}

	Set_serial_param(ratio, parity, ByteSize, StopBit);
	PurgeComm(serial_handle, PURGE_TXCLEAR | PURGE_RXCLEAR); // 清空缓冲区

	return(1);
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::serial_open
(
	const char *serial_name,               // 参数：串口的名字,例如:"/dev/ttyS0"
	enum SERIAL_RATIO_TYPE ratio,          // 波特率
	enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
	enum SERIAL_STOP_TYPE StopBit          // 停止位
)
{
	// 关闭旧串口
	if ((serial_is_open == 1) && (serial_handle > 0)) { close(serial_handle); }
	serial_is_open = 0;
	serial_handle = 0;

	// 打开LMB屏的串口
	serial_handle = open(serial_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_handle == -1){ serial_handle = 0; return(-1); }
	fcntl(serial_handle, F_SETFL, FNDELAY);
	serial_is_open = 1;

	// 配置串口
	tcgetattr(serial_handle, &serial_param);
	Set_serial_param(ratio, parity, ByteSize, StopBit);

	return(1);
}
#elif defined(USE_ACE)
int LMB_SCREEN_SERIAL::serial_open
(
	const char *serial_name,               // 参数：串口的名字,例如:linux: "/dev/ttyS0", windows: "COM1"
	enum SERIAL_RATIO_TYPE ratio,          // 波特率
	enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
	enum SERIAL_STOP_TYPE StopBit          // 停止位
)
{
	// 关闭旧串口
	ace_read_dev.dump();
	ace_con.dump();

	// 打开LMB屏的串口
	int result = ace_con.connect
	(
		ace_read_dev,
		ACE_DEV_Addr(serial_name)
	);
	if (result < 0) { return(-1); }
	serial_is_open = 1;

	// 配置串口
	Set_serial_param(ratio, parity, ByteSize, StopBit);

	// 载入配置
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

// 设置参数, 串口打开成功时，才能调用此函数.
#if defined(OS_WINDOWS) && !defined(USE_ACE)
void LMB_SCREEN_SERIAL::Set_serial_param
(
	enum SERIAL_RATIO_TYPE ratio,          // 波特率
	enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
	enum SERIAL_STOP_TYPE StopBit          // 停止位
)
{
	if((serial_handle == 0) || (serial_is_open != 1)){ return; }

	// 波特率
	switch(ratio)
	{
	case SRT_2400:
		serial_param.BaudRate = 2400;//波特率为 2400
	break;

	case SRT_4800:
		serial_param.BaudRate = 4800;//波特率为 4800
	break;

	case SRT_9600:
		serial_param.BaudRate = 9600;//波特率为 9600
	break;

	case SRT_19200:
		serial_param.BaudRate = 19200;
	break;

	case SRT_57600:
		serial_param.BaudRate = 57600;
	break;

	case SRT_115200:
		serial_param.BaudRate = 115200;//波特率为 115200
	break;

	default:
		serial_param.BaudRate = 9600;//波特率为 9600
	break;
	}
	
	// 数据位类型
	switch(ByteSize)
	{
	case SBT_7:  // 7bit
		serial_param.ByteSize = 7;//每个字节有 7 位
	break;

	case SBT_8:  // 8bit
		serial_param.ByteSize = 8;//每个字节有 8 位
	break;

	default:
		serial_param.ByteSize = 8;//每个字节有 8 位
	break;
	}
	
	// 奇偶校验
	switch(parity)
	{
	case SPT_O: // 奇数校验
		serial_param.Parity = ODDPARITY;
	break;

	case SPT_E: // 偶数校验
		serial_param.Parity = EVENPARITY;
	break;
	
	case SPT_N: // 无奇偶校验
		serial_param.Parity = NOPARITY;
	break;

	default:
		serial_param.Parity = NOPARITY;
	break;
	}
	
	// 停止位
	switch(StopBit)
	{
	case SST_0:  // 停止位 1
		serial_param.StopBits = ONESTOPBIT;
	break;

	case SST_1:  // 停止位 1.5
		serial_param.StopBits = ONE5STOPBITS;
	break;

	case SST_2:  // 停止位 2
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
	enum SERIAL_RATIO_TYPE ratio,          // 波特率
	enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
	enum SERIAL_STOP_TYPE StopBit          // 停止位
)
{
	if ((serial_handle == 0) || (serial_is_open != 1)) { return; }

	// 波特率
	switch (ratio)
	{
	case SRT_2400: //波特率为 2400
		cfsetispeed(&serial_param, B2400);
		cfsetospeed(&serial_param, B2400);
	break;

	case SRT_4800: //波特率为 4800
		cfsetispeed(&serial_param, B4800);
		cfsetospeed(&serial_param, B4800);
	break;

	case SRT_9600: //波特率为 9600
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

	case SRT_115200: //波特率为 115200
		cfsetispeed(&serial_param, B115200);
		cfsetospeed(&serial_param, B115200);
	break;

	default: //波特率为 9600
		cfsetispeed(&serial_param, B9600);
		cfsetospeed(&serial_param, B9600);
	break;
	}

	// 将本地模式 (CLOCAL)和串行数据接收(CREAD)设置为有效.
	serial_param.c_cflag |= (CLOCAL | CREAD);

	// 数据位类型
	serial_param.c_cflag &= ~CSIZE;
	switch (ByteSize)
	{
	case SBT_7:  // 7bit
		serial_param.c_cflag |= CS7;//每个字节有 7 位
	break;

	case SBT_8:  // 8bit
		serial_param.c_cflag |= CS8;//每个字节有 8 位
	break;

	default:
		serial_param.c_cflag |= CS8;//每个字节有 8 位
	break;
	}

	// 奇偶校验
	switch (parity)
	{
	case SPT_O: // 奇数校验
		serial_param.c_cflag |= PARENB;
		serial_param.c_cflag |= PARODD;
		serial_param.c_cflag &= ~CSTOPB;
	break;

	case SPT_E: // 偶数校验
		serial_param.c_cflag |= PARENB;
		serial_param.c_cflag &= ~PARODD;
		serial_param.c_cflag &= ~CSTOPB;
	break;

	default:
	case SPT_N: // 无奇偶校验
		serial_param.c_cflag &= ~PARENB;
		serial_param.c_cflag &= ~CSTOPB;
	break;
	}

	// 停止位
	switch (StopBit)
	{
	case SST_0:  // 停止位 1
		serial_param.c_cflag &= ~CSTOPB;
	break;

	case SST_2:  // 停止位 2
		serial_param.c_cflag |= CSTOPB;
	break;

	default:
		serial_param.c_cflag &= ~CSTOPB;
	break;
	}

	// 设置立即生效
	tcsetattr(serial_handle, TCSANOW, &serial_param);
}
#elif defined(USE_ACE)
void LMB_SCREEN_SERIAL::Set_serial_param
(
	enum SERIAL_RATIO_TYPE ratio,          // 波特率
	enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
	enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
	enum SERIAL_STOP_TYPE StopBit          // 停止位
)
{
	if (serial_is_open != 1) { return; }

	// 波特率
	switch (ratio)
	{
	case SRT_2400: //波特率为 2400
		serial_param.baudrate = 2400;
	break;

	case SRT_4800: //波特率为 4800
		serial_param.baudrate = 4800;
	break;

	case SRT_9600: //波特率为 9600
		serial_param.baudrate = 9600;
	break;

	case SRT_19200:
		serial_param.baudrate = 19200;
	break;

	case SRT_57600:
		serial_param.baudrate = 57600;
	break;

	case SRT_115200: //波特率为 115200
		serial_param.baudrate = 115200;
	break;

	default: //波特率为 9600
		serial_param.baudrate = 9600;
	break;
	}
	
	// 在XON字符被发送之前，输入缓冲区的最小字节
	serial_param.xonlim = 0;
	// 在XOFF字符被发送之前，输入缓冲区的最大字节
	serial_param.xofflim = 0;
	// 对于POSIX非标准的读最少个数的字符
	serial_param.readmincharacters = 0;
	// 在从read返回之前，等待的时间
	//serial_param.readtimeoutmsec = 1000; // 1秒
	serial_param.readtimeoutmsec = -1; // 1秒

	// 奇偶校验
	switch (parity)
	{
	case SPT_O: // 奇数校验
		serial_param.paritymode = "odd";
	break;

	case SPT_E: // 偶数校验
		serial_param.paritymode = "even";
	break;

	default:
	case SPT_N: // 无奇偶校验
		serial_param.paritymode = "none";
	break;
	}

	// 是否支持CTS模式。确定是否可通过查询 Clear To Send(CTS) 线的状态发送数据。
	serial_param.ctsenb = FALSE;
	// 使用和设置RTS。0标示不使用RTS 1标示使用RTS 2标示使用RTS流控制握手(win32)
	// 3标示如果有了足够的用于传输的字节，RTS线变高。传输之后RTS变成低(win32)
	serial_param.rtsenb = 0;
	// 是否在输入上使用软件流控制
	serial_param.xinenb = FALSE;
	// 是否在输出上使用软件流控制
	serial_param.xoutenb = FALSE;
	// 设备是否是POSIX调制模式
	serial_param.modem = FALSE;
	// 是否使用receiver(POSIX)
	serial_param.rcvenb = TRUE;
	// Data Set Ready(DSR) 线的状态是否起作用(WIN32)
	serial_param.dsrenb = FALSE;
	// Data Terminal Ready状态是否起作用
	serial_param.dtrdisable = FALSE;

	// 数据位类型。合法值为5, 6, 7, 8。Win32上支持4位
	switch (ByteSize)
	{
	case SBT_7:  // 7bit
		serial_param.databits = 7;//每个字节有 7 位
	break;

	case SBT_8:  // 8bit
		serial_param.databits = 8;//每个字节有 8 位
	break;

	default:
		serial_param.databits = 8;//每个字节有 8 位
	break;
	}

	// 停止位。合法值为1和2
	switch (StopBit)
	{
	case SST_0:  // 停止位 1
		serial_param.stopbits = 1;
		break;

	case SST_2:  // 停止位 2
		serial_param.stopbits = 1;
	break;

	default:
		serial_param.stopbits = 1;
	break;
	}
}
#endif
///////////////////////////////////

// 关闭串口
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

// 串口是否打开成功,成功返回>0的值,失败返回<0的值
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



// 根据串口命令ID操作串口
#if defined(OS_WINDOWS) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::API_LMB_operate_by_cmdID
(
	int serial_cmd_id,     // 命令ID
	void *write_buff,      // 写入内容
	unsigned int write_byte   // 写入内容
)
{
	DWORD real_write_byte;
	BOOL w_result;
	/////////////

	// 写入命令
	w_result =
	WriteFile
	(
		serial_handle, // 串口句柄
		g_LMB_com_command_list[serial_cmd_id].cmd_name, // 串口命令
		g_LMB_com_command_list[serial_cmd_id].cmd_name_length, // 命令长度
		&real_write_byte, // 实际写入字节数
		NULL
	);

	if(w_result == false){ return(-1); }

	// 写入数据
	if((write_buff == NULL) ||(write_byte == 0)){ return(1); }
	w_result =
	WriteFile
	(
		serial_handle, // 串口句柄
		write_buff, // 串口数据
		write_byte, // 数据长度
		&real_write_byte, // 实际写入字节数
		NULL
	);

	if(w_result == false){ return(-1); }

	return(1);
}
#elif defined(OS_LINUX) && !defined(USE_ACE)
int LMB_SCREEN_SERIAL::API_LMB_operate_by_cmdID
(
	int serial_cmd_id,     // 命令ID
	void *write_buff,      // 写入内容
	unsigned int write_byte   // 写入内容
)
{
	int w_result;

	// 写入命令
	w_result = 
	write
	(
		serial_handle,
		g_LMB_com_command_list[serial_cmd_id].cmd_name, // 串口命令
		g_LMB_com_command_list[serial_cmd_id].cmd_name_length // 命令长度
	);

	if (w_result < 0) { return(-1); }

	// 写入数据
	if ((write_buff == NULL) || (write_byte == 0)) { return(1); }
	w_result =
	write
	(
		serial_handle,
		write_buff, // 串口命令
		write_byte // 命令长度
	);
	if (w_result == false) { return(-1); }

	return(1);
}
#elif defined(USE_ACE)
int LMB_SCREEN_SERIAL::API_LMB_operate_by_cmdID
(
	int serial_cmd_id,     // 命令ID
	void *write_buff,      // 写入内容
	unsigned int write_byte   // 写入内容
)
{
	int result = 0;

	// 写入命令
	result = (int)ace_read_dev.send
	(
		g_LMB_com_command_list[serial_cmd_id].cmd_name, // 串口命令
		g_LMB_com_command_list[serial_cmd_id].cmd_name_length // 命令长度
	);
	if (result < 0) { return(-1); }

	// 写入数据
	if ((write_buff == NULL) || (write_byte == 0)) { return(1); }

	result = (int)ace_read_dev.send
	(
		write_buff, // 串口命令
		write_byte  // 命令长度
	);
	if (result < 0) { return(-1); }

	return(1);
}
#endif

// 应用层的操作接口
int LMB_SCREEN_SERIAL::API_LMB_OPERATE
(
	int operate_type,          // 操作类型，参考enum SERIAL_OPERATE_ID
	void *write_text,          // 写入内容
	unsigned int write_byte    // 写入内容长度
)
{
	////if(Is_serial_successed() < 0){ return(-1); }

	int result=0;

	switch(operate_type)
	{
		case SERIAL_OPERATE_CLEAR_SCREEN: // 清屏
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CLEAR, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_RETURN: // 光标复位
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_RETURN, NULL, 0);
			break;

		case SERIAL_OPERATE_WRITE_TEXT: // 写入文本
			if((write_text == NULL) || (write_byte == 0)){ return(1); }
			
			// 命令头
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_WRITE_TEXT_HEAD, write_text, write_byte);
			if(result < 0){ return(-1); }


			// 命令结尾
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_WRITE_TEXT_TAIL, NULL, 0);
			if(result < 0){ return(-1); }
			break;

		case SERIAL_OPERATE_SET_CURSOR: // 设置光标位置
			#if 0
				if((write_text == NULL) || (write_byte != 1)){ return(-1); }
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR, write_text, write_byte);
			#else
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR, NULL, 0);
			#endif
			break;

		case SERIAL_OPERATE_SET_CURSOR_2: // 设置光标位置到第二行首位
			#if 0
				if((write_text == NULL) || (write_byte != 1)){ return(-1); }
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR_2, write_text, write_byte);
			#else
				result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SET_CURSOR_2, NULL, 0);
			#endif
			break;

		case SERIAL_OPERATE_CURSOR_LEFT_DIR: // 设置光标移动方向为“左”
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_LEFT_DIR, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_RIGHT_DIR: // 设置光标移动方向为“右”
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_RIGHT_DIR, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_MOVE1_LEFT:  // 光标向左移动一位（一个字符的位置）
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_MOVE1_LEFT, NULL, 0);
			break;

		case SERIAL_OPERATE_CURSOR_MOVE1_RIGHT:  // 光标向右移动一位（一个字符的位置）
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_CURSOR_MOVE1_RIGHT, NULL, 0);
			break;

		case SERIAL_OPERATE_SCREEN_MOVE1_LEFT:  // 画面向左滚动一位（一个字符的位置）
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SCREEN_MOVE1_LEFT, NULL, 0);
			break;

		case SERIAL_OPERATE_SCREEN_MOVE1_RIGHT:  // 画面向右滚动一位（一个字符的位置）
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SCREEN_MOVE1_RIGHT, NULL, 0);
			break;

		case SERIAL_OPERATE_BACKGROUND_OPEN:    // 背光亮
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_BACKGROUND_OPEN, NULL, 0);
			break;

		case SERIAL_OPERATE_BACKGROUND_OFF:    // 背光关闭
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_BACKGROUND_OFF, NULL, 0);
			break;

		case SERIAL_OPERATE_SHOW_CURSOR:      // 显示光标
			result = API_LMB_operate_by_cmdID(LMB_SERIAL_CMD_SHOW_CURSOR, NULL, 0);
			break;

		case SERIAL_OPERATE_HIDE_CURSOR:      // 隐藏光标
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
		// 创建共享内存
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

		//将共享内存连接到当前进程的地址空间  
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
	
	API_LMB_OPERATE(SERIAL_OPERATE_CURSOR_RETURN, NULL, 0);  // 第一行定位需要用“光标复位”指令(旧的驱动使用)
	API_LMB_OPERATE(SERIAL_OPERATE_SET_CURSOR, NULL, 0); // 新驱动使用
	API_LMB_OPERATE(SERIAL_OPERATE_WRITE_TEXT, pLine1, LMB_SCREEN_LINE_WIDTH);

	API_LMB_OPERATE(SERIAL_OPERATE_SET_CURSOR_2, NULL, 0);
	API_LMB_OPERATE(SERIAL_OPERATE_WRITE_TEXT, pLine2, LMB_SCREEN_LINE_WIDTH);
}

