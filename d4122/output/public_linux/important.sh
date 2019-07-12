#!/bin/sh

#win�ṩ���ļ�Ӧλ��win_for_linux_setupĿ¼��

find -name "*.sh" -print | xargs chmod +x

. ./env.sh
if [ ! -d $path_win_bin ]; then
        echo  Ŀ¼ $path_win_bin �����ڣ��˳� 
        echo
        exit
fi

date
dirroot=$PWD

cd package_env
./env_setup.sh
cd $dirroot

#cd package_fc_protecter
#./fc_protecter.sh
#cd $dirroot

cd package_probe
./probe.sh
./probe_component.sh
cd $dirroot

cd package_ui
./package_ui_important.sh
#./ui_component.sh
#./internet_ip.sh
cd $dirroot

cd package_ui_update
./package_ui_update.sh
cd $dirroot

cd package_unierm_client_reg
./unierm_client_reg_important.sh
cd $dirroot


cd package_web_ROOT
./web_important.sh
cd $dirroot

cd package_unierm_data
./unierm_data.sh
cd $dirroot

cd package_sys_upgrade
./sys_upgrade_important.sh
cd $dirroot

./env_packet_copy.sh

date

