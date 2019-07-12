#!/bin/sh

export MY_FUNC=$1
export MY_TMPDIR=$2

if [ -z $2 ]; then
export MY_TMPDIR=`mktemp -d /tmp/unierm_setup_pkg.XXXXXXXX`
fi

echo $MY_TMPDIR

# �ҵ�ѹ���ļ��ڰ��е�ƫ����������__ARCHIVE_BELOW__����һ��
ARCHIVE=`awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' "$0"`

echo pls wait for extracting to ${MY_TMPDIR}....

tail -n+$ARCHIVE $0 | tar xz -C $MY_TMPDIR

BACKUP_CDIR=`pwd` #��ס��ǰĿ¼

cd $MY_TMPDIR/setup

if [ "$MY_FUNC" == "upgrade" ];then  #��������
cd upgrade_tool
./upgradetool
elif [ "$MY_FUNC" == "extract" ];then #���н�ѹ
echo extract ok to $MY_TMPDIR
else
./prg_install_menu.sh   #���а�װ
fi
 
cd $BACKUP_CDIR

if echo $MY_TMPDIR|grep -q /tmp/unierm_setup_pkg;then
rm -rf $MY_TMPDIR
fi
 
exit 0
 
#This line "__ARCHIVE_BELOW__" must be the last line of the file
__ARCHIVE_BELOW__
