#!/bin/sh

root_path=$PWD
pkt_name=$1

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
	if [ ! ${ARCH} -eq 32 ]; then
		clear
		echo 
		echo 
		echo "本脚本只适用于32位操作系统!"
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
	echo    "            PROBE安装程序                     "
	echo    "                                                      "
	echo    "                                                      "
	echo    "            1：安装MySQL数据库（只能安装一次）         "
	echo    "                                                      "
	echo    "            2：安装开发环境（只能安装一次）           "
	echo    "                                                      "
	echo    "            3：升级MySQL数据库                        "
	echo    "                                                      "
	echo    "            4：安装通用组件                           "
	echo    "                                                      "
	echo    "            5：安装PROBE程序                          "
	echo    "                                                      "
	echo    "            6：快速安装 3->5         "
	echo    "                                                      "
	echo    "            0：退出安装                               "
	echo    "                                                      "
	echo    "            可选择：1、2、3、4、5、6、0              "
	echo    "                                                  "
	echo -n "            请选择："
}

func_1()
{
	func_print_sub_title "1：安装MySQL数据库（只能安装一次）" $1
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
	./install_db.sh
	cd $root_path
	rm -rf install_mysql_db
	
	func_print_rtn "数据库升级完成，请检查升级结果"  $1
	return
}

func_2()
{
	func_print_sub_title "2：安装开发环境（只能安装一次）" $1
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
	./probe_autorun.sh
	rm -rf env_setup
	rm -rf probe_autorun.sh

	echo
	echo 检测 LAMPP 安装包程序是否存在
	if [ ! -r xampp-linux-1.7.3a.new.tar.gz ]; then
	    func_print_rtn "无法找到安装包 xampp-linux-1.7.3a.new.tar.gz" $1
		return
	fi

	echo
	echo 安装lampp服务
	if [ ! -d /opt ]; then
	        mkdir /opt
	fi
	tar zxf xampp-linux-1.7.3a.new.tar.gz -C /opt/
	cp -f httpd.conf /opt/lampp/etc/
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

	allfile=`ls -r *_Upgrade_Tool.*full.up`
	echo 正在删除配置信息

        if [ -d /unierm_data/local_machine ]; then
        /home/unierm/probe/k
        sleep 5
        rm -f -r /unierm_data/bak_grogram_config
        rm -f -r /unierm_data/fc_server_config
        rm -f -r /unierm_data/probe_buffer
        rm -f -r /unierm_data/udata
        rm -f -r /unierm_data/probe_config
        rm -f -r /udata_cap/audit_data/
        rm -f -r /udata_cap/cap_data/
        rm -f -r /udata_cap/report/
        rm -f -r /udata_cap/temp_audit_data/
        fi

        if [ -r /unierm_data/local_machine/machine_info.lic2 ]; then
        cp -r /unierm_data/local_machine/machine_info.lic2 /
        fi

        if [ -r /unierm_data/local_machine/flux_control.dat ]; then
        cp -r /unierm_data/local_machine/flux_control.dat /
        fi

        if [ -r /home/unierm_data/local_machine/machine_info.lic2 ]; then
        cp -r /home/unierm_data/local_machine/machine_info.lic2 /
        fi

	for onefile in $allfile
	do
		if [ {$pkt_name} = {$onefile} ]; then
			echo 开始安装 $onefile
			./unierm_install.sh $pkt_name

                        if [ -r /machine_info.lic2 ]; then
                        mv /machine_info.lic2 /home/unierm_data/local_machine/
                        fi

                        if [ -r /flux_control.dat ]; then
                        mv /flux_control.dat /home/unierm_data/local_machine/
                        fi

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



