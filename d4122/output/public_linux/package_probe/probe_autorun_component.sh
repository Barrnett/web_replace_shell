#!/bin/sh


#默认路径
cfg_file=/etc/xiaocao/unierm_setup_path.ini
path_prg=/xiaocao_prg

#读配置文件
if [ -r ${cfg_file} ]; then
	path_prg=`sed -n '1p' ${cfg_file}`
else
	echo ${cfg_file} not exist, use default path!
fi

cp -R probe ${path_prg}/unierm


cd ${path_prg}/unierm/probe

cp iaf_packet.ko /usr/lib

# 做链接
#ln -s libACE.so         libACE-5.8.so
#ln -s libiconv.so       libiconv.so.2
#ln -s libwiretap.so    libwiretap.so.0
#ln -s libwsutil.so       libwsutil.so.0
#ln -s libwireshark.so libwireshark.so.0
#ln -s libpcap.so        libpcap.so.1
#ln -s libmysqlclient.so  libmysqlclient.so.16

