/*
 * EZInclude.h
 *
 *  Created on: 2016年7月28日
 *      Author: Administrator
 */


#ifndef EZINCLUDE_H_
#define EZINCLUDE_H_

#if defined (__cplusplus)
extern "C"
{
#endif

typedef unsigned char  uint8;
typedef signed   char  int8;
typedef unsigned short uint16;
typedef signed   short int16;
typedef unsigned int   uint32;
typedef signed   int   int32;

#define NO_FRAME             1
#define FRAME_RCEIVED        0
#define ERR_NULL_POINTER    -1
#define ERR_TOO_MANY_BYTES  -2
#define ERR_CHECKSUM        -3


/*
 * 监控盘收到串口数据后调用
 * input para:
 *              pDataIn 接收到的新字符
 *              dataSizeIn 接收到字符字节数
 * output para:
 *              pPayloadOut 如果有完整帧，则输出其转译后的静载荷
 *              pPayloadSizeOut 如果有完整帧，指示静载荷字节数
 * return:
 *              0 表示有正确帧收到，此时pPayloadOut和pPayloadSizeOut才有意义
 *              非0 参照错误代码
 */
extern int32 ifHaveFrame(const uint8* pDataIn, const uint16 dataSizeIn, uint8* pPayloadOut, uint16* pPayloadSizeOut);

/*
 * 监控板发送串口数据前调用
 * input para:
 *              pPayloadDataIn 待发送的指令
 *              payloadSizeIn 待发送指令字节数
 * output para:
 *              pFrameOut 客直接发送的转译后的帧
 *              pFrameSizeOut 转译后的帧字节数
 * return:
 *              0 表示帧正确生成
 *              非0 参照错误代码
 */
extern int32 makeFrame(const uint8* pPayloadDataIn, const uint16 payloadSizeIn, uint8* pFrameOut, uint16* pFrameSizeOut);

#if defined (__cplusplus)
}
#endif
#endif /* EZINCLUDE_H_ */
