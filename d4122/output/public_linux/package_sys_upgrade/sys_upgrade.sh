#!/bin/sh

pktdir=upgrade_tool

echo
echo ׼������ $pktdst ��װ��


if [ -d $pktdir ]; then
        rm -rf $pktdir
fi

#echo ����Ŀ¼
mkdir $pktdir

cp -R ../$2/silent_$1_unierm_client_reg_package_to_web.exe $pktdir/silent_unierm_client_reg_package_to_web.exe
cp -R ../$2/silent_$1_web_ROOT_setup.exe                   $pktdir/silent_web_ROOT_setup.exe
#cp -R ../$2/silent_web_conf.exe                        $pktdir/
#cp -R ../$2/silent_ui_component_package_to_web.exe      $pktdir/
#cp -R ../$2/silent_internet_ip_table_package_to_web.exe $pktdir/
cp -R ../$2/silent_netebs_update_package_to_web.exe     $pktdir/
cp -R ../$2/silent_probe_setup.exe                       $pktdir/
cp -R ../$2/silent_probe_component_setup.exe            $pktdir/
cp -R ../$2/silent_$1_netebs_package_to_web.exe         $pktdir/silent_netebs_package_to_web.exe
#cp -R ../$2/silent_fc_protecter_setup.exe               $pktdir/
cp -R ../$2/silent_bak_prg_config_setup.exe               $pktdir/

cp -R ../prepare_packet/httpd_64.conf                    $pktdir/httpd.conf

cp -R ../$2/env_setup.exe                                 $pktdir

cp -R ${path_bin_root}/libACE-5.8.so               $pktdir/
cp -R ${path_bin_root}/libcpf.so                   $pktdir/
cp -R ${path_bin_root}/libcpf2.so                  $pktdir/
cp -R ${path_bin_root}/libmysqlclient.so.16        $pktdir/
cp -R ${path_bin_root}/libmonitormanage.so         $pktdir/
cp -R ${path_bin_root}/libunierm_setup_dll.so      $pktdir/
cp -R ${path_bin_root}/libconfig_center.so         $pktdir/
cp -R ${path_bin_root}/upgradetool                 $pktdir/
cp -R ${path_ini_root}/${UPGRADE_TOOL_XML_SRC}         $pktdir/${UPGRADE_TOOL_XML_DST}
cp -R ${path_ini_root}/${VERSION_FILE_XML_SRC}         $pktdir/${VERSION_FILE_XML_DST} 

#cp -R ../../bin/trans_config_db_to_xml.exe  $pktdir/

# ����oem����Ϣ
# mkdir $pktdir/oem
cp -R ${path_root_linux}/package_oem/$1/oem.xml    $pktdir/


#���е�sql�ű�������������
cp -R ../prepare_db/upgrade_data_migration       $pktdir/
cp -R ../prepare_db/netems_db_setup.linux.rar    $pktdir/netems_db_setup.exe 


# ���Ƶ�����Ŀ¼
#echo ���Ƶ�����Ŀ¼


# ֻ���ư�װ��������������װ�ű�������Ŀ¼��
mv -f $pktdir	 ../$publish_setup/
cp ../prepare_packet/unierm_install.sh            ../$publish_setup/
cp ../prepare_packet/silent_prg_config_setup.exe  ../$publish_setup/

ls -l ../$publish_setup

echo ��ϲ��$pktdst ��װ���������!!!!!!!!!!!!
