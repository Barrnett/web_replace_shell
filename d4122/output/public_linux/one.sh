#!/bin/sh

#win提供的文件应位于win_for_linux_setup目录下

find -name "*.sh" -print | xargs chmod +x

if [ $1 ] ; then 
	OEM=$1
fi

. ./env.sh

if [ ! -d $path_win_bin ]; then
        echo  目录 $path_win_bin 不存在，退出 
        echo
        exit
fi



start_time=`date`
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
./package_ui.sh $OEM
#./ui_component.sh
#./internet_ip.sh
cd $dirroot

cd package_ui_update
./package_ui_update.sh
cd $dirroot

cd package_unierm_client_reg
./unierm_client_reg.sh $OEM
cd $dirroot


cd package_web_ROOT
./web_one.sh $OEM
cd $dirroot

# 准备好数据库（包括mysql的数据库表，netembs的表和my.cnf)
# 准备好配置（probe_config和fcserver_config)
cd package_unierm_data
./unierm_data.sh
cd $dirroot

cd package_sys_upgrade
./sys_upgrade.sh $OEM $ver 
# ./sys_upgrade_core.sh $OEM $ver
cd $dirroot

chmod +x env_packet_copy.sh
./env_packet_copy.sh

if [ -d $ver ]; then
        rm $ver -rf
fi
        
echo start time: $start_time
echo stop time : `date`
