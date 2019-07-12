#!/bin/sh

ROOT_PATH=$PWD

#ARCH=`getconf LONG_BIT`

#Ĭ��·��

PATH_PRG=/xiaocao_prg
PATH_DB=/xiaocao_data
PATH_DATA=/xiaocao_data
#�������ļ�
cfg_file=unierm_setup_path.ini
if [ -r ${cfg_file} ]; then
	PATH_PRG=`sed -n '1p' ${cfg_file}`
	PATH_DB=`sed -n '2p' ${cfg_file}`
	PATH_DATA=`sed -n '3p' ${cfg_file}`
fi

# ֻɾ����ǰ�ĳ��򣬲�ɾ�����ݿ������
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
	
	#����ֱ��ɾ�� ${OLD_PATH_PRG}��${OLD_PATH_DATA����ֹ�⼸��Ŀ¼��һ�𣬻����ݿ��Ҳ��ɾ����
	
	rm -rf ${OLD_PATH_PRG}/unierm
	rm -rf ${OLD_PATH_PRG}/unierm_data
	
	rm -rf ${OLD_PATH_DATA}/udata_cap
	
	func_print_rtn "unstall old prg finished"  $1
}

# ɾ����ǰ�ĳ��򣬻�Ҫɾ�����ݿ������
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
		func_print_rtn "�����������ڣ���Ҫ�ļ�install_mysql_db.tgz�����鰲װĿ¼�Ƿ�����"  $1
		return
	fi

	tar zxf install_mysql_db.tgz
	
	cd install_mysql_db
	./install_db.sh ${PATH_DB}
	cd $ROOT_PATH
	rm -rf install_mysql_db
	
	func_print_rtn "���ݿ�������ɣ������������"  $1
	return
}


func_install_probe()
{
	func_print_sub_title "��װPROBE����"
	
	if [ ! -r ${PKT_NAME} ]; then
		func_print_rtn "PROBE��װ�������ڣ���Ҫ�ļ�${PKT_NAME}�����鰲װĿ¼�Ƿ�����"  $1
		return
	fi

	func_remove_oldver_prg $1
	
	cd $ROOT_PATH

	#���������ļ���etc��
	if [ ! -d  /etc/xiaocao ]; then
		mkdir /etc/xiaocao
	fi
	
	cp -r ${cfg_file} /etc/xiaocao 
	

	echo ��ʼ��װ
	./unierm_install.sh ${PATH_PRG} ${PATH_DB} ${PATH_DATA}
	
	 /opt/lampp/lampp stop
	 
	echo ��ʼ�޸�httpd.conf ��rc.local������
	tar zxf env_setup.exe
	./probe_autorun.sh ${PATH_PRG} ${PATH_DB} ${PATH_DATA}
	rm -rf env_setup
	rm -rf probe_autorun.sh

	/opt/lampp/lampp start
	 
	func_print_rtn "PROBE����װ��ɣ����鰲װ���"
	return
}

func_upgrade_probe()
{
	func_print_sub_title "����������������"
	
	if [ ! -r ${PKT_NAME} ]; then
		func_print_rtn "�����������ڣ���Ҫ�ļ�${PKT_NAME}�����鰲װĿ¼�Ƿ�����"  $1
		return
	fi
	
	cd upgrade_tool
  ./upgradetool
	 
	func_print_rtn "�Ѿ�������������.."
	return
}

func_print_rtn()
{
	echo -n	"\n            "
	echo $1 
	# �������2�����ڣ���menu���ã�������ʾ�����ڣ���quick���ã����ȴ�����
	if [ -f $2 ]; then
		echo	"\n            ���س�������"
		read temp
	fi
}
func_print_sub_title()
{
	# �������2�����ڣ���menu���ã�������ʾ�����ڣ���quick���ã��������Ļ
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
	func_print_sub_title "���ٰ�װ"
	cd $ROOT_PATH

	func_remove_oldver_prg_and_db "quick"
	func_update_db "quick"
	func_install_probe "quick"

	func_print_rtn "���ٰ�װ��ɣ����鰲װ���"
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



