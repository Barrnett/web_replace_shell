/*
 * EZInclude.h
 *
 *  Created on: 2016��7��28��
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
extern int32 ifHaveFrame(const uint8* pDataIn, const uint16 dataSizeIn, uint8* pPayloadOut, uint16* pPayloadSizeOut);

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
extern int32 makeFrame(const uint8* pPayloadDataIn, const uint16 payloadSizeIn, uint8* pFrameOut, uint16* pFrameSizeOut);

#if defined (__cplusplus)
}
#endif
#endif /* EZINCLUDE_H_ */
