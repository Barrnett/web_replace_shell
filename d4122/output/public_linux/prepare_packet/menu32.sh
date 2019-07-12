#!/bin/sh

root_path=$PWD
pkt_name=$1

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
func_check_os()
{
	ARCH=`getconf LONG_BIT`
	if [ ! ${ARCH} -eq 32 ]; then
		clear
		echo 
		echo 
		echo "���ű�ֻ������32λ����ϵͳ!"
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
	echo    "            PROBE��װ����                     "
	echo    "                                                      "
	echo    "                                                      "
	echo    "            1����װMySQL���ݿ⣨ֻ�ܰ�װһ�Σ�         "
	echo    "                                                      "
	echo    "            2����װ����������ֻ�ܰ�װһ�Σ�           "
	echo    "                                                      "
	echo    "            3������MySQL���ݿ�                        "
	echo    "                                                      "
	echo    "            4����װͨ�����                           "
	echo    "                                                      "
	echo    "            5����װPROBE����                          "
	echo    "                                                      "
	echo    "            6�����ٰ�װ 3->5         "
	echo    "                                                      "
	echo    "            0���˳���װ                               "
	echo    "                                                      "
	echo    "            ��ѡ��1��2��3��4��5��6��0              "
	echo    "                                                  "
	echo -n "            ��ѡ��"
}

func_1()
{
	func_print_sub_title "1����װMySQL���ݿ⣨ֻ�ܰ�װһ�Σ�" $1
	cd $root_path
	
	if [ -r /usr/bin/mysql ]; then
		func_print_rtn "��ǰϵͳ�Ѿ���װ�� MySQL �ˡ�" $1
		return
	fi

	if [ ! -d mysql_server ]; then
		func_print_rtn "MySQL ��װ�������ڣ���ҪĿ¼mysql_server�����鰲װĿ¼�Ƿ�����"  $1
		return
	fi

	cd mysql_server
	./install_mysql.sh
	
	func_print_rtn "MySQL ��װ��ɣ����鰲װ���"  $1
	return
}

func_3()
{
	func_print_sub_title "3������MySQL���ݿ�" $1
	cd $root_path
	
	if [ ! -r install_mysql_db.tgz ]; then
		func_print_rtn "�����������ڣ���Ҫ�ļ�install_mysql_db.tgz�����鰲װĿ¼�Ƿ�����"  $1
		return
	fi

	tar zxf install_mysql_db.tgz
	
	cd install_mysql_db
	./install_db.sh
	cd $root_path
	rm -rf install_mysql_db
	
	func_print_rtn "���ݿ�������ɣ������������"  $1
	return
}

func_2()
{
	func_print_sub_title "2����װ����������ֻ�ܰ�װһ�Σ�" $1
	cd $root_path
	
	if [ -r /usr/bin/g++ ]; then
		func_print_rtn "��ǰϵͳ�Ѿ���װ�����������ˡ�"  $1
		return
	fi

	if [ ! -d build_essential ]; then
		func_print_rtn "����������װ�������ڣ���ҪĿ¼build_essential�����鰲װĿ¼�Ƿ�����"  $1
		return
	fi

	cd build_essential
	./install_build.sh

	func_print_rtn "����������װ��ɣ����鰲װ���"  $1
	return
}


func_4()
{
	func_print_sub_title "4����װͨ�����" $1
	cd $root_path

	tar zxf env_setup.exe
	./probe_autorun.sh
	rm -rf env_setup
	rm -rf probe_autorun.sh

	echo
	echo ��� LAMPP ��װ�������Ƿ����
	if [ ! -r xampp-linux-1.7.3a.new.tar.gz ]; then
	    func_print_rtn "�޷��ҵ���װ�� xampp-linux-1.7.3a.new.tar.gz" $1
		return
	fi

	echo
	echo ��װlampp����
	if [ ! -d /opt ]; then
	        mkdir /opt
	fi
	tar zxf xampp-linux-1.7.3a.new.tar.gz -C /opt/
	cp -f httpd.conf /opt/lampp/etc/
	func_print_rtn "ͨ�������װ��ɣ����鰲װ���"  $1
	return
}

func_6()
{
	func_print_sub_title "���ٰ�װ 3->5"
	cd $root_path

	func_3 "quick"
	func_4 "quick"
	func_5 "quick"

	func_print_rtn "���ٰ�װ��ɣ����鰲װ���"
	return
}

func_5()
{
	func_print_sub_title "5����װPROBE����"
	cd $root_path

	allfile=`ls -r *_Upgrade_Tool.*full.up`
	echo ����ɾ��������Ϣ

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
			echo ��ʼ��װ $onefile
			./unierm_install.sh $pkt_name

                        if [ -r /machine_info.lic2 ]; then
                        mv /machine_info.lic2 /home/unierm_data/local_machine/
                        fi

                        if [ -r /flux_control.dat ]; then
                        mv /flux_control.dat /home/unierm_data/local_machine/
                        fi

			func_print_rtn "PROBE����װ��ɣ����鰲װ���"
			return
		fi
	done

	func_print_rtn "PROBE��װ��$pkt_name�����ڣ����鰲װ���Ƿ�����"
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



