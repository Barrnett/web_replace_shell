#!/bin/sh


#echo 复制到发行目录
if [ ! -d ../$ver ]; then
        mkdir ../$ver
fi
mv $1 ../$ver/
ls -l ../$ver | grep $1
