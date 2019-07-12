#!/bin/sh
# $1 总体的压缩包名字
# $2 安装包目地路径

path_dst=$1
if [ -z $1 ]; then
	path_dst=/xiaocao_prg
fi

path_prg=${path_dst}

path_db=$2
if [ -z $2 ]; then
	path_db=/xiaocao_data
fi

path_data=$3
if [ -z $3 ]; then
	path_data=/xiaocao_data
fi

#程序的安装目录在 ${path_dst}/unierm
#程序的配置目录在 ${path_dst}/unierm_data

cp -R silent_prg_config_setup.exe upgrade_tool
cd upgrade_tool


if [ ! -d ${path_dst}/unierm ]; then
	mkdir -p ${path_dst}/unierm
fi

if [ ! -d ${path_dst}/unierm_data ]; then
	mkdir -p ${path_dst}/unierm_data
fi
	
echo
echo 安装probe和component
tar zxf silent_probe_component_setup.exe -C ${path_dst}/unierm
tar zxf silent_probe_setup.exe -C ${path_dst}/unierm
chmod -R 777 ${path_dst}

echo
echo 安装运行时配置
tar zxf silent_prg_config_setup.exe 
./probe_autorun.sh ${path_dst}
tar zxf silent_bak_prg_config_setup.exe 
./probe_autorun.sh ${path_dst}

echo
echo 修改http的配置

#先复制过去，再修改httpd的内容
# httpd.conf 的修改和拷贝
orig_file_httpd=httpd.conf
dst_file_httpd=/opt/lampp/etc/httpd.conf
if [ -r ${orig_file_httpd} ]; then
  cp -R ${orig_file_httpd} ${dst_file_httpd}
	echo "Modify  ${dst_file_httpd}"
	s1="\"/xiaocao_data/udata_cap\""
	d1="\"${path_data}/udata_cap\""
	s2="\"/xiaocao_prg/unierm_data\""
	d2="\"${path_prg}/unierm_data\""

	if [ "$s1" != "$d1" ];then
	    sed -i "s:${s1}:${d1}:g" ${dst_file_httpd}
	fi

  if [ "$s2" != "$d2" ];then
	    sed -i "s:${s2}:${d2}:g" ${dst_file_httpd} 
	fi	
fi

echo
echo 安装客户端的下载包
#tar zxf silent_internet_ip_table_package_to_web.exe
#./probe_autorun.sh
#tar zxf silent_ui_component_package_to_web.exe
#./probe_autorun.sh
tar zxf silent_netebs_package_to_web.exe
./probe_autorun.sh


echo
echo 安装客户端的升级包
tar zxf silent_netebs_update_package_to_web.exe
./probe_autorun.sh


echo
echo 安装客户端的注册包
tar zxf silent_unierm_client_reg_package_to_web.exe
./probe_autorun.sh

echo
echo 安装web
if [ ! -d /opt ]; then
	mkdir /opt
fi
#tar zxf xampp-linux-1.7.3a.new.tar.gz -C /opt/
tar zxf silent_web_ROOT_setup.exe 
./probe_autorun.sh ${path_dst}


# 安装之后，删除解压缩产生的目录
if [ ! -z $1 ]; then
	cd -
#	rm -rf upgrade_tool
fi

echo
echo 安装程序完成。
echo
