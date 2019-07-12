#!/bin/sh

#call probe_component security_probe bin release probe auto > ./probe_component_build.lo

pktdst=silent_probe_component_setup.exe


echo
echo 准备制作 $pktdst 安装包

rm probe -rf

mkdir probe

#echo 拷贝公用dll --------

cp -R ${path_bin_root}/libACE-5.8.so	 	probe
cp -R ${path_bin_root}/libpcap.so.1*		probe
cp -R ${path_bin_root}/libldap-2.4.so.2*	probe
cp -R ${path_bin_root}/libiconv.so.2		probe
cp -R ${path_bin_root}/libmysqlclient.so.16	probe
cp -R ${path_bin_root}/libsnmp++.so.2		probe

if [ ${ARCH} -eq 32 ]; then
	cp -R ${path_bin_root}/libEtherIO.so		probe
else
	cp -R ${path_bin_root}/libetherio.so.1		probe
	cp -R ${path_bin_root}/libpfring.so		probe
	cp -R ${path_bin_root}/e1000e.ko		probe
fi


cp -R ../prepare_packet/iaf_packet.ko		probe
cp -R ${path_bin_root}/bpwd_drv.ko		probe

cp -R ${path_ini_root}/ipt1.dat	probe
cp -R ${path_ini_root}/ipt2.dat	probe

cp -R ${path_bin_root}/bypass_test_shell		probe


#echo 打包 -------------------
cp -R probe_autorun_component.sh 	probe_autorun.sh
tar zcf $pktdst probe probe_autorun.sh
rm -rf probe_autorun.sh
rm -rf probe

# 复制到发行目录
../copy_to_version.sh $pktdst

echo 恭喜：$pktdst 发行库 制作完成.!!!!!!!!!!!!
echo

