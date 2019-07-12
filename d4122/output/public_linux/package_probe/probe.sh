#!/bin/sh

# rem
# ����linux��probe.shû��Я������������޷�����OEM����Ϣ�����Ȱ취���£�
# 	��package_web_ROOT���ʱ��ͬʱ��OEM��Ϣ���룬��ѹ��ʱ����Ӧ��oem�ŵ�probeĿ¼��
#

pktdst=silent_probe_setup.exe

echo
echo ׼������ $pktdst ���а汾 !!!!!!!!!!!!
rm probe -rf


#echo ����Ŀ¼
mkdir probe
mkdir probe/ini
mkdir probe/log
mkdir probe/local_machine
mkdir probe/pat
mkdir probe/web_pat
mkdir probe/url
mkdir probe/system

#echo ����ini�ļ�
cp -R ${path_ini_root}/pat/proto_tree.xml	probe/pat/
cp -R ${path_ini_root}/pat/*.dat 		probe/pat/
cp -R ${path_ini_root}/pat/*.ini 		probe/pat/
cp -R ${path_ini_root}/web_pat/*.dat 		probe/web_pat/
cp -R ${path_ini_root}/local_machine/*		probe/local_machine/
cp -R ${path_ini_root}/url/*.dat							probe/url/
cp -R ${path_ini_root}/ini/* 			probe/ini/
cp -R ${path_ini_root}/linux_ini/* 			probe/ini/

cp -R ${THIS_OEM_INFO_PATH}/*			probe/system/

#echo ��������dll $2 Dll --------
cp -R ${path_bin_root}/libcapdata_mgr.so	probe
cp -R ${path_bin_root}/libfilter_mgr.so		probe
cp -R ${path_bin_root}/libpacketio.so		probe
cp -R ${path_bin_root}/libcpf.so	probe
cp -R ${path_bin_root}/libcpf2.so	probe


#echo �������dll $2 Dll --------
cp -R ${path_bin_root}/libobserver_mgr.so		probe
cp -R ${path_bin_root}/libtcpip_mgr.so			probe
cp -R ${path_bin_root}/libflux_control_common.so	probe
cp -R ${path_bin_root}/libtcpip_mgr_common.so	probe
cp -R ${path_bin_root}/libflux_control.so			probe
cp -R ${path_bin_root}/libflux_control_core.so			probe
cp -R ${path_bin_root}/libflux_control_base.so			probe
cp -R ${path_bin_root}/libflux_control_nat.so			  probe
cp -R ${path_bin_root}/liblicensechecker.so		probe	
cp -R ${path_bin_root}/libfcserverfacade.so	probe
cp -R ${path_bin_root}/libconfig_center.so     probe 
cp -R ${path_bin_root}/libbypassmanager.so	probe
cp -R ${path_bin_root}/libfetchipmacinl3switch.so		probe

cp -R ${path_bin_root}/libsysreportclient.so	probe
cp -R ${path_bin_root}/libunierm_setup_db.so	probe
cp -R ${path_bin_root}/libmonitormanage.so	probe
cp -R ${path_bin_root}/libprobesysmgr.so	probe
cp -R ${path_bin_root}/libunierm_ctrl_dll.so	probe
cp -R ${path_bin_root}/libtimer_report_dll.so	probe

#echo ����Э��dll $2 Dll --------
cp -R ${path_bin_root}/libftp.so	probe
cp -R ${path_bin_root}/libtelnet.so	probe
cp -R ${path_bin_root}/libmail.so	probe

cp -R ${path_bin_root}/libhttp.so		probe
cp -R ${path_bin_root}/libhttp_webbrowse.so	probe
cp -R ${path_bin_root}/libhttp_webmail.so	probe
cp -R ${path_bin_root}/libhttp_weibo.so	probe

cp -R ${path_bin_root}/libqq.so		probe
cp -R ${path_bin_root}/libfetion.so		probe
cp -R ${path_bin_root}/libtelnet.so		probe

cp -R ${path_bin_root}/libfirewall.so	probe
cp -R ${path_bin_root}/libldap_auth.so	probe
cp -R ${path_bin_root}/libefilterio.so probe

#echo ����ִ�г��� $pktdst  --------
cp -R ${path_bin_root}/seprobe_mfc	probe
cp -R ${path_bin_root}/testcontrol		probe
cp -R ${path_bin_root}/extractmachineinfo	probe


cp -R ${path_bin_root}/fcserver	probe

cp -R ${path_ini_root}/${VERSION_FILE_XML_SRC}	probe/${VERSION_FILE_XML_DST}

cp -R ${path_bin_root}/unierm_ctrl		probe
cp -R ${path_bin_root}/timer_report_exe	probe

cp -R ${path_ini_root}/p   probe
cp -R ${path_ini_root}/k   probe


#ɾ��������CVS�ļ�
find probe -name CVS | xargs rm -rf

#echo ��� -------------------
cp -R probe_autorun_probe.sh probe_autorun.sh
tar zcf $pktdst probe probe_autorun.sh
rm -rf probe_autorun.sh
rm -rf probe


# echo ���Ƶ�����Ŀ¼
../copy_to_version.sh $pktdst


echo ��ϲ��$pktdst ���а汾 �������.!!!!!!!!!!!!
echo

