//////////////////////////////////////////////////////////////////////////
//LMB_serial_class.h

#if !defined(__LMB_SERIAL_CLASS_HEAD__)
#define __LMB_SERIAL_CLASS_HEAD__
//////////////////////////////////////////////////////////////////////////////////

#include "cpf/ostypedef.h"

#if !defined(USE_ACE)
	#define USE_ACE
#endif

#if defined(OS_WINDOWS) && !defined(USE_ACE)
	#include <windows.h>
#elif defined(OS_LINUX) && !defined(USE_ACE)
	#include <stdio.h>
	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h> /* File control definitions */
	#include <errno.h>
	#include <termios.h>
#elif defined(USE_ACE)
	#include "ace/DEV_Addr.h"
	#include "ace/DEV_Connector.h"
	#include "ace/TTY_IO.h"

	// 共享内存需要的头文件
	#include "ace/Malloc_T.h"
	#include "ace/OS.h"
	#include "ace/Null_Mutex.h"
	#include "ace/MMAP_Memory_Pool.h"
	#include "ace/Shared_Memory_Pool.h"
	#include "ace/Global_Macros.h"
	
#else
	#error "unknown system!"
#endif
/////////////////

#define LMB_SCREEN_MAX_LINE			100//预开辟行数
#define LMB_SCREEN_LINE_WIDTH			16 // 一行显示16个字
#define LMB_SCREEN_LINE_HEIGHT		2 // 一屏显示2行

// 串口波特率的几种类型
enum SERIAL_RATIO_TYPE
{
	SRT_2400,
	SRT_4800,
	SRT_9600,
	SRT_19200,
	SRT_57600,
	SRT_115200,
	///////

	SRT_UNKNOWN
};

// 串口数据位的类型
enum SERIAL_BYTESIZE_TYPE
{
	SBT_7,  // 7bit
	SBT_8,  // 8bit
	////

	SBT_UNKNOWN
};

// 串口奇偶校验类型
enum SERIAL_PARITY_TYPE
{
	SPT_O, // 奇数校验
	SPT_E, // 偶数校验
	SPT_N, // 无奇偶校验
	////

	SPT_UNKNOWN
};

// 停止位
enum SERIAL_STOP_TYPE
{
	SST_0,  // 停止位 1
	SST_1,  // 停止位 1.5
	SST_2,  // 停止位 2
	///////

	SST_UNKNOWN
};
//////////////////////

// 应用层操作代号
enum SERIAL_OPERATE_ID
{
	SERIAL_OPERATE_CLEAR_SCREEN,         // 清屏操作
	SERIAL_OPERATE_CURSOR_RETURN,        // 光标复位
	SERIAL_OPERATE_WRITE_TEXT,           // 写入文本
	SERIAL_OPERATE_SET_CURSOR,           // 设置光标位置(第一行:0x00~0x0F, 第二行:0x40~0x4F)
	SERIAL_OPERATE_SET_CURSOR_2,       // 设置光标位置到第二行首位
	SERIAL_OPERATE_CURSOR_LEFT_DIR,      // 设置光标移动方向为“左”
	SERIAL_OPERATE_CURSOR_RIGHT_DIR,     // 设置光标移动方向为“右”
	SERIAL_OPERATE_CURSOR_MOVE1_LEFT,    // 光标向左移动一位（一个字符的位置）
	SERIAL_OPERATE_CURSOR_MOVE1_RIGHT,   // 光标向右移动一位（一个字符的位置）
	SERIAL_OPERATE_SCREEN_MOVE1_LEFT,    // 画面向左滚动一位（一个字符的位置）
	SERIAL_OPERATE_SCREEN_MOVE1_RIGHT,   // 画面向右滚动一位（一个字符的位置）
	SERIAL_OPERATE_BACKGROUND_OPEN,      // 背光亮
	SERIAL_OPERATE_BACKGROUND_OFF,       // 背光关闭
	SERIAL_OPERATE_SHOW_CURSOR,          // 显示光标
	SERIAL_OPERATE_HIDE_CURSOR,          // 隐藏光标
	////////////

	SERIAL_OPERATE_MAX
};
////////////////////////

// 串口命令代号
enum LMB_SERIAL_COMMAND_ID
{
	LMB_SERIAL_CMD_CLEAR,                // 清屏
	LMB_SERIAL_CMD_CURSOR_RETURN,        // 光标复位
	LMB_SERIAL_CMD_WRITE_TEXT_HEAD,      // 写入文本,命令头
	LMB_SERIAL_CMD_WRITE_TEXT_TAIL,      // 写入文本,命令结尾
	LMB_SERIAL_CMD_SET_CURSOR,           // 设置光标位置(第一行:0x00~0x0F, 第二行:0x40~0x4F)
	LMB_SERIAL_CMD_SET_CURSOR_2,       // 设置光标位置到第二行首位, add by xumx 2016-11-15
	LMB_SERIAL_CMD_CURSOR_LEFT_DIR,      // 设置光标移动方向为“左”
	LMB_SERIAL_CMD_CURSOR_RIGHT_DIR,     // 设置光标移动方向为“右”
	LMB_SERIAL_CMD_CURSOR_MOVE1_LEFT,    // 光标向左移动一位（一个字符的位置）
	LMB_SERIAL_CMD_CURSOR_MOVE1_RIGHT,   // 光标向右移动一位（一个字符的位置）
	LMB_SERIAL_CMD_SCREEN_MOVE1_LEFT,    // 画面向左滚动一位（一个字符的位置）
	LMB_SERIAL_CMD_SCREEN_MOVE1_RIGHT,   // 画面向右滚动一位（一个字符的位置）
	LMB_SERIAL_CMD_BACKGROUND_OPEN,      // 背光亮
	LMB_SERIAL_CMD_BACKGROUND_OFF,       // 背光关闭
	LMB_SERIAL_CMD_SHOW_CURSOR,          // 显示光标
	LMB_SERIAL_CMD_HIDE_CURSOR,          // 隐藏光标
	/////////

	LMB_SERIAL_CMD_MAX
};

#define LMB_KEYCODE_LEFT	0xDF
#define LMB_KEYCODE_UP		0xFB
#define LMB_KEYCODE_RIGHT	0xF7
#define LMB_KEYCODE_DOWN	0xEF

#pragma pack(1)
// 串口命令
struct LMB_SERIAL_CMD_INFO
{
	// 命令内容(最多2个字节)
	unsigned char cmd_name[2];
	// 命令内容长度(单位：字节)
	unsigned char cmd_name_length;
};
#pragma pack()
///////////////////////////////

#pragma pack(1)
//共享内存数据结构
typedef struct LMB_SHARE_MEM
{
	unsigned int nReserve;
	unsigned int nLightTimer;//黑屏倒计时
	unsigned int nTotalLineNum;
	unsigned int nCurrentLineNum;
	char cData[LMB_SCREEN_LINE_WIDTH * LMB_SCREEN_MAX_LINE];
}LMB_SHARE_MEM_S;
#pragma pack()
///////////////////////////////

// LMB 屏的串口操作类
class LMB_SCREEN_SERIAL
{
public:
	// 构造函数
	LMB_SCREEN_SERIAL(const char *serial_name); // 参数：串口的名字,例如:"COM1"
	LMB_SCREEN_SERIAL();

	// 析构函数
	~LMB_SCREEN_SERIAL();

public:
	// 串口操作句柄
	#if defined(OS_WINDOWS) && !defined(USE_ACE)
		HANDLE serial_handle;
	#elif defined(OS_LINUX) && !defined(USE_ACE)
		int serial_handle;
	#elif defined(USE_ACE)
		ACE_TTY_IO ace_read_dev;
		ACE_DEV_Connector ace_con;
	#endif

	// 配置参数
	#if defined(OS_WINDOWS) && !defined(USE_ACE)
		DCB serial_param;
	#elif defined(OS_LINUX) && !defined(USE_ACE)
		struct termios serial_param;
	#elif defined(USE_ACE)
		ACE_TTY_IO::Serial_Params serial_param;
	#endif

	// 串口状态,0:无串口连接 1:有串口连接
	int serial_is_open;

	const static struct LMB_SERIAL_CMD_INFO g_LMB_com_command_list[LMB_SERIAL_CMD_MAX];
	
private:
	LMB_SHARE_MEM_S* m_pShm;//共享内存指针

public:

	// 打开一个串口, 成功返回>0的值，失败返回<0的值。
	int serial_open
	(
		const char *serial_name,                     // 参数：串口的名字,例如:"COM1"
		enum SERIAL_RATIO_TYPE ratio=SRT_9600,          // 波特率
		enum SERIAL_PARITY_TYPE parity=SPT_N,        // 奇偶校验
		enum SERIAL_BYTESIZE_TYPE ByteSize=SBT_8,    // 数据位
		enum SERIAL_STOP_TYPE StopBit =SST_2         // 停止位
	);


	// 设置参数, 串口打开成功时，才能调用此函数.
	void Set_serial_param
	(
		enum SERIAL_RATIO_TYPE ratio,          // 波特率
		enum SERIAL_PARITY_TYPE parity,        // 奇偶校验
		enum SERIAL_BYTESIZE_TYPE ByteSize,    // 数据位
		enum SERIAL_STOP_TYPE StopBit          // 停止位
	);

	// 关闭串口
	void Close_serial();

	// 串口是否打开成功,成功返回>0的值,失败返回<0的值
	int Is_serial_successed();

	// 应用层的操作接口
	int API_LMB_OPERATE
	(
		int operate_type,          // 操作类型，参考enum SERIAL_OPERATE_ID
		void *write_text,          // 写入内容
		unsigned int write_byte    // 写入内容长度
	);


	int f_CreateShareMem(void);
	int f_MapShareMem(void);
	int f_DeleteShareMem(void);
	int f_FreeShareMem(void);
	int f_AddOneLine(char * usr_string);
	int f_WriteOneLine(char * usr_string, unsigned int line_num);
	unsigned int f_GetCurLine();
	int f_ScreenTimeout(unsigned int timer);
	void f_ScreenMoveUp(void);
	void f_ScreenMoveDown(void);
	void f_ScreenPrintf(void);

private:
	// 根据串口命令ID操作串口
	int API_LMB_operate_by_cmdID
	(
		int serial_cmd_id,     // 命令ID
		void *write_buff,      // 写入内容
		unsigned int write_byte   // 写入内容
	);

};

//////////////////////////////////////////////////////////////////////////////////
#endif
