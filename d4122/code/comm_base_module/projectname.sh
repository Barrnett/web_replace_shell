#!/bin/sh

#取当前全路径名
DIR=`pwd`

#只保留最有一个目录名称
WANT=`echo $DIR | sed "s/^.*\///"`

#去掉名称中的_VersionModule
#WANT=`echo $WANT | sed 's/_VersionModule*$//'`

#去掉名称中的 _VERSIONMODULE
#WANT=`echo $WANT | sed 's/_VERSIONMODULE*$//'`

#转换成小写字符
WANT=`echo $WANT | tr A-Z a-z`

echo $WANT


