#!/bin/sh

#ȡ��ǰȫ·����
DIR=`pwd`

#ֻ��������һ��Ŀ¼����
WANT=`echo $DIR | sed "s/^.*\///"`

#ȥ�������е�_VersionModule
#WANT=`echo $WANT | sed 's/_VersionModule*$//'`

#ȥ�������е� _VERSIONMODULE
#WANT=`echo $WANT | sed 's/_VERSIONMODULE*$//'`

#ת����Сд�ַ�
WANT=`echo $WANT | tr A-Z a-z`

echo $WANT


