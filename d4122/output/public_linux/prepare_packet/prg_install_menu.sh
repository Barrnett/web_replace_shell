#!/bin/sh

ROOT_PATH=$PWD

#ARCH=`getconf LONG_BIT`

#默认路径

PATH_PRG=/xiaocao_prg
PATH_DB=/xiaocao_data
PATH_DATA=/xiaocao_data
#读配置文件
cfg_file=unierm_setup_path.ini
if [ -r ${cfg_file} ]; then
	PATH_PRG=`sed -n '1p' ${cfg_file}`
	PATH_DB=`sed -n '2p' ${cfg_file}`
	PATH_DATA=`sed -n '3p' ${cfg_file}`
fi

# 只删除以前的程序，不删除数据库的数据
func_remove_oldver_prg()
{
	cd $ROOT_PATH

	func_print_sub_title "Uninstall last version" $1

	# read old ini file info
	FILE_INI=unierm_setup_path.ini
	OLD_PATH_INI=/etc/xiaocao/${FILE_INI}
	
	if [ ! -r ${OLD_PATH_INI} ]; then
		func_print_rtn "Can't find last version in your system!" $1
		return;
	fi
	
	# get program installed path
	OLD_PATH_PRG=`sed -n '1p' ${OLD_PATH_INI}`
	OLD_PATH_DB=`sed -n '2p' ${OLD_PATH_INI}`
	OLD_PATH_DATA=`sed -n '3p' ${OLD_PATH_INI}`
	
	# stop running process
	${OLD_PATH_PRG}/unierm/probe/k
	sleep 5

	# backup lic file
	
	# delete old program
	
	#不能直接删除 ${OLD_PATH_PRG}和${OLD_PATH_DATA，防止这几个目录在一起，会数据库表也误删除了
	
	rm -rf ${OLD_PATH_PRG}/unierm
	rm -rf ${OLD_PATH_PRG}/unierm_data
	
	rm -rf ${OLD_PATH_DATA}/udata_cap
	
	func_print_rtn "unstall old prg finished"  $1
}

# 删除以前的程序，还要删除数据库的数据
func_remove_oldver_prg_and_db()
{
	cd $ROOT_PATH

	func_print_sub_title "Uninstall last version" $1

	# read old ini file info
	FILE_INI=unierm_setup_path.ini
	OLD_PATH_INI=/etc/xiaocao/${FILE_INI}
	
	if [ ! -r ${OLD_PATH_INI} ]; then
		func_print_rtn "Can't find last version in your system!" $1
		return;
	fi
	
	# get program installed path
	OLD_PATH_PRG=`sed -n '1p' ${OLD_PATH_INI}`
	OLD_PATH_DB=`sed -n '2p' ${OLD_PATH_INI}`
	OLD_PATH_DATA=`sed -n '3p' ${OLD_PATH_INI}`
	
	# stop running process
	${OLD_PATH_PRG}/unierm/probe/k
	sleep 5

	# backup lic file
	
	
	# stop db
	service mysql stop
	
	# delete old program
	rm -rf ${OLD_PATH_PRG}
	rm -rf ${OLD_PATH_DB}
	rm -rf ${OLD_PATH_DATA}

	rm -rf ${OLD_PATH_INI}
	
	func_print_rtn "unstall old prg and db data finished"  $1
}

func_update_db()
{
	func_print_sub_title "Update MySQL DB" $1
	cd $ROOT_PATH
	
	if [ ! -r install_mysql_db.tgz ]; then
		func_print_rtn "升级包不存在，需要文件install_mysql_db.tgz，请检查安装目录是否完整"  $1
		return
	fi

	tar zxf install_mysql_db.tgz
	
	cd install_mysql_db
	./install_db.sh ${PATH_DB}
	cd $ROOT_PATH
	rm -rf install_mysql_db
	
	func_print_rtn "数据库升级完成，请检查升级结果"  $1
	return
}


func_install_probe()
{
	func_print_sub_title "安装PROBE程序"
	
	if [ ! -r ${PKT_NAME} ]; then
		func_print_rtn "PROBE安装包不存在，需要文件${PKT_NAME}，请检查安装目录是否完整"  $1
		return
	fi

	func_remove_oldver_prg $1
	
	cd $ROOT_PATH

	#复制配置文件到etc下
	if [ ! -d  /etc/xiaocao ]; then
		mkdir /etc/xiaocao
	fi
	
	cp -r ${cfg_file} /etc/xiaocao 
	

	echo 开始安装
	./unierm_install.sh ${PATH_PRG} ${PATH_DB} ${PATH_DATA}
	
	 /opt/lampp/lampp stop
	 
	echo 开始修改httpd.conf 和rc.local等配置
	tar zxf env_setup.exe
	./probe_autorun.sh ${PATH_PRG} ${PATH_DB} ${PATH_DATA}
	rm -rf env_setup
	rm -rf probe_autorun.sh

	/opt/lampp/lampp start
	 
	func_print_rtn "PROBE程序安装完成，请检查安装结果"
	return
}

func_upgrade_probe()
{
	func_print_sub_title "正在启动升级程序"
	
	if [ ! -r ${PKT_NAME} ]; then
		func_print_rtn "升级包不存在，需要文件${PKT_NAME}，请检查安装目录是否完整"  $1
		return
	fi
	
	cd upgrade_tool
  ./upgradetool
	 
	func_print_rtn "已经启动升级程序.."
	return
}

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


function_print_screen()
{
	cd $ROOT_PATH
	clear
	echo
	echo
	echo    "================================================================="
	echo    "                                                  "
	echo    "                                                  "
	echo    "            PROBE_PRG INSTALL                             "
	echo    "                                                      "
	echo    "                                                      "
	echo    "            1: uninstall old probe_prg"
	echo    "                                                      "
	echo    "            2: modify db_cfg and install db_table       "
	echo    "                                                      "
	echo    "            3: install probe main program                          "
	echo    "                                                      "
	echo    "            4: install all of above         				"
	echo    "                                                      "
	echo    "            5: upgrade probe_prg(if have old version)   "
	echo    "                                                      "
	echo    "            0: exist                               "
	echo    "                                                      "
	echo    "            option: 1, 2, 3, 4, 5, 0              "
	echo    "                                                  "
	echo -n "            input: "
}

func_quick_install()
{
	func_print_sub_title "快速安装"
	cd $ROOT_PATH

	func_remove_oldver_prg_and_db "quick"
	func_update_db "quick"
	func_install_probe "quick"

	func_print_rtn "快速安装完成，请检查安装结果"
	return
}



function_main()
{

	function_print_screen;
	
	read choose

	case $choose in
		1)
		func_remove_oldver_prg_and_db
		function_main
		;;
		
		2)
		func_update_db
		function_main
		;;
		
		3)
		func_install_probe
		function_main
		;;
		
		4)
		func_quick_install
		function_main
		;;
		
		5)
		func_upgrade_probe
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



