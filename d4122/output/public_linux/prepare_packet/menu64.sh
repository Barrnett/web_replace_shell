#!/bin/sh

root_path=$PWD
pkt_name=$1

ARCH=`getconf LONG_BIT`

#默认路径
path_prg=/xiaocao_prg
path_db=/xiaocao_data
path_data=/xiaocao_data
#读配置文件
cfg_file=unierm_setup_path.ini
if [ -r ${cfg_file} ]; then
	path_prg=`sed -n '1p' ${cfg_file}`
	path_db=`sed -n '2p' ${cfg_file}`
	path_data=`sed -n '3p' ${cfg_file}`
fi

mkdir -p ${path_data}/udata_cap

func_print_rtn()
{
	echo -n	"\n            "
	echo $1 
	# 如果参数2不存在，则menu调用，给出提示；存在，则quick调用，不等待输入
	if [ -f $2 ]; then
		echo	"\n            按回车键返回"
		read temp
	fi
}
func_print_sub_title()
{
	# 如果参数2不存在，则menu调用，给出提示；存在，则quick调用，不清除屏幕
	if [ -f $2 ]; then
		clear
	fi
	echo -n "\n\n            "
	echo $1
	echo    "\n\n"
}
func_check_os()
{
	ARCH=`getconf LONG_BIT`
	if [ ! ${ARCH} -eq 64 ]; then
		clear
		echo 
		echo 
		echo "本脚本只适用于64位操作系统!"
		echo 
		echo 
		exit
	fi
}
function_print_screen()
{
	cd $root_path
	clear
	echo
	echo
	echo    "================================================================="
	echo    "                                                  "
	echo    "                                                  "
	echo    "            PROBE安装程序                             "
	echo    "                                                      "
	echo    "                                                      "
	
	if [ ! ${ARCH} -eq 64 ]; then

	echo    "            1：安装MySQL数据库（只能安装一次，64位不要安装）"
	echo    "                                                      "
	echo    "            2：安装开发环境（只能安装一次 64位不要安装）           "
	echo    "                                                      "	
	
	fi
	
	echo    "            3：升级MySQL数据库                        "
	echo    "                                                      "
	echo    "            4：安装通用组件                           "
	echo    "                                                      "
	echo    "            5：安装PROBE程序                          "
	echo    "                                                      "
	echo    "            6：快速安装 3->5         								"
	echo    "                                                      "
	echo    "            0：退出安装                               "
	echo    "                                                      "
	
	if [ ! ${ARCH} -eq 64 ]; then
	
	echo    "            可选择：1、2、3、4、5、6、0              "
	
	else
	
	echo    "            可选择：3、4、5、6、0              "
	
	fi
	
	echo    "                                                  "
	echo -n "            请选择："
}

func_1()
{
	func_print_sub_title "1：安装MySQL数据库（只能安装一次，64位不要安装）" $1
	cd $root_path
	
	if [ -r /usr/bin/mysql ]; then
		func_print_rtn "当前系统已经安装过 MySQL 了。" $1
		return
	fi

	if [ ! -d mysql_server ]; then
		func_print_rtn "MySQL 安装包不存在，需要目录mysql_server，请检查安装目录是否完整"  $1
		return
	fi

	cd mysql_server
	./install_mysql.sh
	
	func_print_rtn "MySQL 安装完成，请检查安装结果"  $1
	return
}

func_3()
{
	func_print_sub_title "3：升级MySQL数据库" $1
	cd $root_path
	
	if [ ! -r install_mysql_db.tgz ]; then
		func_print_rtn "升级包不存在，需要文件install_mysql_db.tgz，请检查安装目录是否完整"  $1
		return
	fi

	tar zxf install_mysql_db.tgz
	
	cd install_mysql_db
	./install_db.sh ${path_db}
	cd $root_path
	rm -rf install_mysql_db
	
	func_print_rtn "数据库升级完成，请检查升级结果"  $1
	return
}

func_2()
{
	func_print_sub_title "2：安装开发环境（只能安装一次，64位不要安装）" $1
	cd $root_path
	
	if [ -r /usr/bin/g++ ]; then
		func_print_rtn "当前系统已经安装过开发环境了。"  $1
		return
	fi

	if [ ! -d build_essential ]; then
		func_print_rtn "开发环境安装包不存在，需要目录build_essential，请检查安装目录是否完整"  $1
		return
	fi

	cd build_essential
	./install_build.sh

	func_print_rtn "开发环境安装完成，请检查安装结果"  $1
	return
}


func_4()
{
	func_print_sub_title "4：安装通用组件" $1
	cd $root_path

	tar zxf env_setup.exe
	./probe_autorun.sh ${path_prg}
	rm -rf env_setup
	rm -rf probe_autorun.sh

	echo
	echo 检测 LAMPP 安装包程序是否存在
	file_xampp=xampp-linux-1.7.3a.new.tar.gz
	if [ ${ARCH} -eq 64 ]; then
		file_xampp=xampp-linux-1.8.3-4.lite.tar.gz
	fi
	
	if [ ! -r ${file_xampp} ]; then
	    func_print_rtn "无法找到安装包 ${file_xampp}" $1
		return
	fi

	echo
	echo 安装lampp服务
	if [ ! -d /opt ]; then
	        mkdir /opt
	fi
	tar zxf ${file_xampp} -C /opt/
	
	#修改httpd的内容，然后再复制过去
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

	func_print_rtn "通用组件安装完成，请检查安装结果"  $1
	return
}

func_6()
{
	func_print_sub_title "快速安装 3->5"
	cd $root_path

	func_3 "quick"
	func_4 "quick"
	func_5 "quick"

	func_print_rtn "快速安装完成，请检查安装结果"
	return
}

func_5()
{
	func_print_sub_title "5：安装PROBE程序"
	cd $root_path

	#复制配置文件到etc下
	mkdir -p /etc/xiaocao
	cp -r ${cfg_file} /etc/xiaocao 
	
	allfile=`ls -r *_Upgrade_Tool.*full.up`
	echo 正在删除配置信息

        if [ -d ${path_prg}/unierm_data/local_machine ]; then
        ${path_prg}/unierm/probe/k
        sleep 5
        rm -f -r ${path_prg}/unierm/*
        rm -f -r ${path_prg}/unierm_data/*
        rm -f -r ${path_db}/unierm_data/udata
        rm -f -r ${path_data}/udata_cap/*
        fi

       	for onefile in $allfile
	do
		if [ {$pkt_name} = {$onefile} ]; then
			echo 开始安装 $onefile
			./unierm_install.sh $pkt_name ${path_prg}

                        func_print_rtn "PROBE程序安装完成，请检查安装结果"
			return
		fi
	done

	func_print_rtn "PROBE安装包$pkt_name不存在，请检查安装包是否完整"
	return
}

function_main()
{
	func_check_os;

	function_print_screen;
	
	read choose

	case $choose in
		1)
		func_1
		function_main
		;;
		
		2)
		func_2
		function_main
		;;
		
		3)
		func_3
		function_main
		;;
		
		4)
		func_4
		function_main
		;;
		
		5)
		func_5
		function_main
		;;

		6)
		func_6
		function_main
		;;

		0|q|x)
		echo "\n\n"
		exit
		;;
		
		
		*)
		function_main
		;;
	esac

}

find -name "*.sh" -print | xargs chmod +x

function_main



