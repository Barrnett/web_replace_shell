# !/bin/sh

if [ -z $1 ];then
	echo "usage: ./uns.v10r5.build < install | update | extract >"
	echo " "
	exit 0
fi

MY_FUNC=$1
MY_TMPDIR="/home/tmp/"

# 找到压缩文件在包中的偏移量，即含__ARCHIVE_BELOW__的下一行
ARCHIVE=`awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' "$0"`

echo "extracting to ${MY_TMPDIR}..."
#echo $ARCHIVE

rm -rf $MY_TMPDIR
mkdir $MY_TMPDIR
tail -n+$ARCHIVE $0 | tar xz -C $MY_TMPDIR

if [ "$MY_FUNC" == "update" ];then
	echo "updating..."
	$MY_TMPDIR/ts/make_update.sh $MY_TMPDIR $2
elif [ "$MY_FUNC" == "extract" ];then
	echo "extract completed!"
elif [ "$MY_FUNC" == "install" ];then
	echo "installing..."
	$MY_TMPDIR/ts/make_install.sh $MY_TMPDIR $2
else
	echo "input command error!"
fi

rm -rf $MY_TMPDIR

exit 0

#This line "__ARCHIVE_BELOW__" must be the last line of the file
__ARCHIVE_BELOW__
