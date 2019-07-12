#!/bin/sh


#默认路径
cfg_file=/etc/xiaocao/unierm_setup_path.ini
path_prg=/xiaocao_prg
#path_db=/xiaocao_data
path_data=/xiaocao_data

#读配置文件
if [ -r ${cfg_file} ]; then
	path_prg=`sed -n '1p' ${cfg_file}`
#	path_db=`sed -n '2p' ${cfg_file}`
	path_data=`sed -n '3p' ${cfg_file}`
else
	echo ${cfg_file} not exist, use default path!
fi

cp -R probe ${path_prg}/unierm


# httpd.conf 的修改和拷贝
file_httpd=${path_prg}/unierm/httpd.conf
if [ -r ${file_httpd} ]; then
	echo "Modify  ${file_httpd}"
	s1="/udata_cap"
	d1="${path_data}"
	s2="/home/unierm_data"
	d2="${path_prg}/unierm_data"

	sed -i "s/${s1}/${d1}/g" ${file_httpd} 
	sed -i "s/${s2}/${d2}/g" ${file_httpd} 

	cp -R ${file_httpd} /opt/lampp/etc/
fi

