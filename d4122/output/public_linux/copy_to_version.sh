#!/bin/sh


#echo ���Ƶ�����Ŀ¼
if [ ! -d ../$ver ]; then
        mkdir ../$ver
fi
mv $1 ../$ver/
ls -l ../$ver | grep $1
