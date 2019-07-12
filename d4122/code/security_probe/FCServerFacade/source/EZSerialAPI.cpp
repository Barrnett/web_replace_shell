/*
 * EZSerialAPI.c
 *
 *  Created on: 2016年7月28日
 *      Author: Administrator
 */
#include "EZInclude.h"
#include <string.h>

enum {  IDLE, HEAD, SUMSTART, LENRCVED, //收包状态
        MAX_RCV_LENGTH = 100,           //单帧最大长度值100
        MAX_RCV_BYTES = MAX_RCV_LENGTH+3, //单帧最大字节数，包括帧头+槽位+长度+载荷+校验和
        FRAME_HEAD = 0x7E,
        FRAME_SLOT = 0xFF,
        FRAME_TRANS_First = 0x7D};
static uint8 stateRcv = IDLE;           //接收状态机标识
static uint8 flagTranslate = 0;
static uint8 rcvFrameBuf[MAX_RCV_BYTES] = {0};
static uint8 rFrmBufCurPoint = 0;       //收缓存写指针
static uint8 rFrmNeeded = 0;            //按照帧长度指示，还差多少字节收完整


static void resetFSM(void);
static uint8 checkSum(void);
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
int32 ifHaveFrame(const uint8* pDataIn, const uint16 dataSizeIn, uint8* pPayloadOut, uint16* pPayloadSizeOut) {
    int32 i;
    uint8 tempByteIn;
    if( pDataIn == 0 || pPayloadOut == 0 || pPayloadSizeOut == 0 ) {
        return ERR_NULL_POINTER;
    }
    for (i = 0; i < dataSizeIn; ++i) {
        tempByteIn = pDataIn[i];
        switch( stateRcv ) {
        case IDLE: {
            if( tempByteIn == FRAME_HEAD ) {
                rFrmBufCurPoint = 0;
                rcvFrameBuf[rFrmBufCurPoint++] = FRAME_HEAD;
                stateRcv = HEAD;
            }
        }
        break;
        case HEAD: {
            if( tempByteIn == FRAME_SLOT ) {
                rcvFrameBuf[rFrmBufCurPoint++] = FRAME_SLOT;
                stateRcv = SUMSTART;

            }
            else {
                //收到的不是槽位号，则丢弃数据
                resetFSM();
            }
        }
        break;
        case SUMSTART: {
            if( tempByteIn > MAX_RCV_LENGTH || tempByteIn < 2 ) {
                //包长越限，则丢弃数据
                resetFSM();
                return ERR_TOO_MANY_BYTES;
            }
            else {
                rcvFrameBuf[rFrmBufCurPoint++] = tempByteIn;
                rFrmNeeded = tempByteIn;
                stateRcv = LENRCVED;
            }

        }
        break;
        case LENRCVED: {
            if( tempByteIn == FRAME_HEAD ) {
                //认为是新一帧的开始
                rFrmBufCurPoint = 0;
                rcvFrameBuf[rFrmBufCurPoint++] = FRAME_HEAD;
                stateRcv = HEAD;

            }
            else if( flagTranslate != 0 ) {
                flagTranslate = 0;
                if( tempByteIn == 0x5D ) {
                    rcvFrameBuf[rFrmBufCurPoint++] = 0x7D;
                    goto rcv_a_byte;
                }
                else if( tempByteIn == 0x5E ) {
                    rcvFrameBuf[rFrmBufCurPoint++] = 0x7E;
                    goto rcv_a_byte;
                }
                else {
                    //收到非法帧，丢弃数据
                    resetFSM();
                }

            }
            else if( tempByteIn == FRAME_TRANS_First ) {
                flagTranslate = 1;
            }
            else {
                rcvFrameBuf[rFrmBufCurPoint++] = tempByteIn;
rcv_a_byte:
                if( --rFrmNeeded == 0 ) {
                    if( checkSum() == FRAME_RCEIVED ) {
                        *pPayloadSizeOut = rcvFrameBuf[2] -2;
                        memcpy(pPayloadOut, &rcvFrameBuf[3], *pPayloadSizeOut);
                        return FRAME_RCEIVED;
                    }
                    else {
                        //收到非法帧，丢弃数据
                        resetFSM();
                        return ERR_CHECKSUM;
                    }
                }
            }
        }
        break;
        default: {
            //收到非法帧，丢弃数据
            resetFSM();
        }
        break;
        }
    }
    return NO_FRAME;
}


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
int32 makeFrame(const uint8* pPayloadDataIn, const uint16 payloadSizeIn, uint8* pFrameOut, uint16* pFrameSizeOut) {
    if( pPayloadDataIn == 0 || pFrameOut == 0 || pFrameSizeOut == 0 ) {
        return ERR_NULL_POINTER;
    }
    if( payloadSizeIn > MAX_RCV_LENGTH ) {
        return ERR_TOO_MANY_BYTES;
    }
    int32 i, j;
    uint8 sumPlus, sumXor;
    pFrameOut[0] = FRAME_HEAD;
    pFrameOut[1] = FRAME_SLOT;
    sumPlus = FRAME_SLOT;
    sumXor = FRAME_SLOT;
    pFrameOut[2] = payloadSizeIn + 2;

    sumPlus += pFrameOut[2];
    sumXor ^= pFrameOut[2];
    j = 3;
    for (i = 0; i < payloadSizeIn; ++i) {
        sumPlus += pPayloadDataIn[i];
        sumXor ^= pPayloadDataIn[i];
        if( pPayloadDataIn[i] != FRAME_HEAD && pPayloadDataIn[i] != FRAME_TRANS_First ) {
            pFrameOut[j++] = pPayloadDataIn[i];
        }
        else {
            pFrameOut[j++] = 0x7D;
            pFrameOut[j++] = pPayloadDataIn[i] - 0x20;
        }
    }
    pFrameOut[j++] = sumPlus;
    pFrameOut[j++] = sumXor;
    *pFrameSizeOut = j;

    return 0;
}


void resetFSM(void) {
    rFrmBufCurPoint = 0;
    stateRcv = IDLE;
    flagTranslate = 0;
    rFrmNeeded = 0;
}

uint8 checkSum(void) {
    uint16 checkBytes = rcvFrameBuf[2];
    uint8* checkData = &rcvFrameBuf[1];
    uint8 sumPlus = 0;
    uint8 sumXor = 0;
    int32 i = 0;
    for (i = 0; i < checkBytes; ++i) {
        sumPlus += checkData[i];
        sumXor ^= checkData[i];
    }
    if( sumPlus != checkData[checkBytes] || sumXor != checkData[checkBytes+1] ) {
        return ERR_CHECKSUM;
    }
    return FRAME_RCEIVED;
}
