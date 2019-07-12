/*
 * EZSerialAPI.c
 *
 *  Created on: 2016��7��28��
 *      Author: Administrator
 */
#include "EZInclude.h"
#include <string.h>

enum {  IDLE, HEAD, SUMSTART, LENRCVED, //�հ�״̬
        MAX_RCV_LENGTH = 100,           //��֡��󳤶�ֵ100
        MAX_RCV_BYTES = MAX_RCV_LENGTH+3, //��֡����ֽ���������֡ͷ+��λ+����+�غ�+У���
        FRAME_HEAD = 0x7E,
        FRAME_SLOT = 0xFF,
        FRAME_TRANS_First = 0x7D};
static uint8 stateRcv = IDLE;           //����״̬����ʶ
static uint8 flagTranslate = 0;
static uint8 rcvFrameBuf[MAX_RCV_BYTES] = {0};
static uint8 rFrmBufCurPoint = 0;       //�ջ���дָ��
static uint8 rFrmNeeded = 0;            //����֡����ָʾ����������ֽ�������


static void resetFSM(void);
static uint8 checkSum(void);
/*
 * ������յ��������ݺ����
 * input para:
 *              pDataIn ���յ������ַ�
 *              dataSizeIn ���յ��ַ��ֽ���
 * output para:
 *              pPayloadOut ���������֡���������ת���ľ��غ�
 *              pPayloadSizeOut ���������֡��ָʾ���غ��ֽ���
 * return:
 *              0 ��ʾ����ȷ֡�յ�����ʱpPayloadOut��pPayloadSizeOut��������
 *              ��0 ���մ������
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
                //�յ��Ĳ��ǲ�λ�ţ���������
                resetFSM();
            }
        }
        break;
        case SUMSTART: {
            if( tempByteIn > MAX_RCV_LENGTH || tempByteIn < 2 ) {
                //����Խ�ޣ���������
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
                //��Ϊ����һ֡�Ŀ�ʼ
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
                    //�յ��Ƿ�֡����������
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
                        //�յ��Ƿ�֡����������
                        resetFSM();
                        return ERR_CHECKSUM;
                    }
                }
            }
        }
        break;
        default: {
            //�յ��Ƿ�֡����������
            resetFSM();
        }
        break;
        }
    }
    return NO_FRAME;
}


/*
 * ��ذ巢�ʹ�������ǰ����
 * input para:
 *              pPayloadDataIn �����͵�ָ��
 *              payloadSizeIn ������ָ���ֽ���
 * output para:
 *              pFrameOut ��ֱ�ӷ��͵�ת����֡
 *              pFrameSizeOut ת����֡�ֽ���
 * return:
 *              0 ��ʾ֡��ȷ����
 *              ��0 ���մ������
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
