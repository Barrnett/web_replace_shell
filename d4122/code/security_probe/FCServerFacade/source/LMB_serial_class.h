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

	// �����ڴ���Ҫ��ͷ�ļ�
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

#define LMB_SCREEN_MAX_LINE			100//Ԥ��������
#define LMB_SCREEN_LINE_WIDTH			16 // һ����ʾ16����
#define LMB_SCREEN_LINE_HEIGHT		2 // һ����ʾ2��

// ���ڲ����ʵļ�������
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

// ��������λ������
enum SERIAL_BYTESIZE_TYPE
{
	SBT_7,  // 7bit
	SBT_8,  // 8bit
	////

	SBT_UNKNOWN
};

// ������żУ������
enum SERIAL_PARITY_TYPE
{
	SPT_O, // ����У��
	SPT_E, // ż��У��
	SPT_N, // ����żУ��
	////

	SPT_UNKNOWN
};

// ֹͣλ
enum SERIAL_STOP_TYPE
{
	SST_0,  // ֹͣλ 1
	SST_1,  // ֹͣλ 1.5
	SST_2,  // ֹͣλ 2
	///////

	SST_UNKNOWN
};
//////////////////////

// Ӧ�ò��������
enum SERIAL_OPERATE_ID
{
	SERIAL_OPERATE_CLEAR_SCREEN,         // ��������
	SERIAL_OPERATE_CURSOR_RETURN,        // ��긴λ
	SERIAL_OPERATE_WRITE_TEXT,           // д���ı�
	SERIAL_OPERATE_SET_CURSOR,           // ���ù��λ��(��һ��:0x00~0x0F, �ڶ���:0x40~0x4F)
	SERIAL_OPERATE_SET_CURSOR_2,       // ���ù��λ�õ��ڶ�����λ
	SERIAL_OPERATE_CURSOR_LEFT_DIR,      // ���ù���ƶ�����Ϊ����
	SERIAL_OPERATE_CURSOR_RIGHT_DIR,     // ���ù���ƶ�����Ϊ���ҡ�
	SERIAL_OPERATE_CURSOR_MOVE1_LEFT,    // ��������ƶ�һλ��һ���ַ���λ�ã�
	SERIAL_OPERATE_CURSOR_MOVE1_RIGHT,   // ��������ƶ�һλ��һ���ַ���λ�ã�
	SERIAL_OPERATE_SCREEN_MOVE1_LEFT,    // �����������һλ��һ���ַ���λ�ã�
	SERIAL_OPERATE_SCREEN_MOVE1_RIGHT,   // �������ҹ���һλ��һ���ַ���λ�ã�
	SERIAL_OPERATE_BACKGROUND_OPEN,      // ������
	SERIAL_OPERATE_BACKGROUND_OFF,       // ����ر�
	SERIAL_OPERATE_SHOW_CURSOR,          // ��ʾ���
	SERIAL_OPERATE_HIDE_CURSOR,          // ���ع��
	////////////

	SERIAL_OPERATE_MAX
};
////////////////////////

// �����������
enum LMB_SERIAL_COMMAND_ID
{
	LMB_SERIAL_CMD_CLEAR,                // ����
	LMB_SERIAL_CMD_CURSOR_RETURN,        // ��긴λ
	LMB_SERIAL_CMD_WRITE_TEXT_HEAD,      // д���ı�,����ͷ
	LMB_SERIAL_CMD_WRITE_TEXT_TAIL,      // д���ı�,�����β
	LMB_SERIAL_CMD_SET_CURSOR,           // ���ù��λ��(��һ��:0x00~0x0F, �ڶ���:0x40~0x4F)
	LMB_SERIAL_CMD_SET_CURSOR_2,       // ���ù��λ�õ��ڶ�����λ, add by xumx 2016-11-15
	LMB_SERIAL_CMD_CURSOR_LEFT_DIR,      // ���ù���ƶ�����Ϊ����
	LMB_SERIAL_CMD_CURSOR_RIGHT_DIR,     // ���ù���ƶ�����Ϊ���ҡ�
	LMB_SERIAL_CMD_CURSOR_MOVE1_LEFT,    // ��������ƶ�һλ��һ���ַ���λ�ã�
	LMB_SERIAL_CMD_CURSOR_MOVE1_RIGHT,   // ��������ƶ�һλ��һ���ַ���λ�ã�
	LMB_SERIAL_CMD_SCREEN_MOVE1_LEFT,    // �����������һλ��һ���ַ���λ�ã�
	LMB_SERIAL_CMD_SCREEN_MOVE1_RIGHT,   // �������ҹ���һλ��һ���ַ���λ�ã�
	LMB_SERIAL_CMD_BACKGROUND_OPEN,      // ������
	LMB_SERIAL_CMD_BACKGROUND_OFF,       // ����ر�
	LMB_SERIAL_CMD_SHOW_CURSOR,          // ��ʾ���
	LMB_SERIAL_CMD_HIDE_CURSOR,          // ���ع��
	/////////

	LMB_SERIAL_CMD_MAX
};

#define LMB_KEYCODE_LEFT	0xDF
#define LMB_KEYCODE_UP		0xFB
#define LMB_KEYCODE_RIGHT	0xF7
#define LMB_KEYCODE_DOWN	0xEF

#pragma pack(1)
// ��������
struct LMB_SERIAL_CMD_INFO
{
	// ��������(���2���ֽ�)
	unsigned char cmd_name[2];
	// �������ݳ���(��λ���ֽ�)
	unsigned char cmd_name_length;
};
#pragma pack()
///////////////////////////////

#pragma pack(1)
//�����ڴ����ݽṹ
typedef struct LMB_SHARE_MEM
{
	unsigned int nReserve;
	unsigned int nLightTimer;//��������ʱ
	unsigned int nTotalLineNum;
	unsigned int nCurrentLineNum;
	char cData[LMB_SCREEN_LINE_WIDTH * LMB_SCREEN_MAX_LINE];
}LMB_SHARE_MEM_S;
#pragma pack()
///////////////////////////////

// LMB ���Ĵ��ڲ�����
class LMB_SCREEN_SERIAL
{
public:
	// ���캯��
	LMB_SCREEN_SERIAL(const char *serial_name); // ���������ڵ�����,����:"COM1"
	LMB_SCREEN_SERIAL();

	// ��������
	~LMB_SCREEN_SERIAL();

public:
	// ���ڲ������
	#if defined(OS_WINDOWS) && !defined(USE_ACE)
		HANDLE serial_handle;
	#elif defined(OS_LINUX) && !defined(USE_ACE)
		int serial_handle;
	#elif defined(USE_ACE)
		ACE_TTY_IO ace_read_dev;
		ACE_DEV_Connector ace_con;
	#endif

	// ���ò���
	#if defined(OS_WINDOWS) && !defined(USE_ACE)
		DCB serial_param;
	#elif defined(OS_LINUX) && !defined(USE_ACE)
		struct termios serial_param;
	#elif defined(USE_ACE)
		ACE_TTY_IO::Serial_Params serial_param;
	#endif

	// ����״̬,0:�޴������� 1:�д�������
	int serial_is_open;

	const static struct LMB_SERIAL_CMD_INFO g_LMB_com_command_list[LMB_SERIAL_CMD_MAX];
	
private:
	LMB_SHARE_MEM_S* m_pShm;//�����ڴ�ָ��

public:

	// ��һ������, �ɹ�����>0��ֵ��ʧ�ܷ���<0��ֵ��
	int serial_open
	(
		const char *serial_name,                     // ���������ڵ�����,����:"COM1"
		enum SERIAL_RATIO_TYPE ratio=SRT_9600,          // ������
		enum SERIAL_PARITY_TYPE parity=SPT_N,        // ��żУ��
		enum SERIAL_BYTESIZE_TYPE ByteSize=SBT_8,    // ����λ
		enum SERIAL_STOP_TYPE StopBit =SST_2         // ֹͣλ
	);


	// ���ò���, ���ڴ򿪳ɹ�ʱ�����ܵ��ô˺���.
	void Set_serial_param
	(
		enum SERIAL_RATIO_TYPE ratio,          // ������
		enum SERIAL_PARITY_TYPE parity,        // ��żУ��
		enum SERIAL_BYTESIZE_TYPE ByteSize,    // ����λ
		enum SERIAL_STOP_TYPE StopBit          // ֹͣλ
	);

	// �رմ���
	void Close_serial();

	// �����Ƿ�򿪳ɹ�,�ɹ�����>0��ֵ,ʧ�ܷ���<0��ֵ
	int Is_serial_successed();

	// Ӧ�ò�Ĳ����ӿ�
	int API_LMB_OPERATE
	(
		int operate_type,          // �������ͣ��ο�enum SERIAL_OPERATE_ID
		void *write_text,          // д������
		unsigned int write_byte    // д�����ݳ���
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
	// ���ݴ�������ID��������
	int API_LMB_operate_by_cmdID
	(
		int serial_cmd_id,     // ����ID
		void *write_buff,      // д������
		unsigned int write_byte   // д������
	);

};

//////////////////////////////////////////////////////////////////////////////////
#endif
