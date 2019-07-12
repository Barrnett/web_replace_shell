#!/bin/sh

pktdst=env_setup.exe

echo
echo 准备制作 $pktdst 安装包

#echo 删除 env_setup 下的所有文件
if [ -d env_setup ]; then
        rm -rf env_setup
fi
if [ -r $pktdst ]; then
        rm -rf $pktdst
fi

#echo 创建目录
mkdir env_setup

#echo 拷贝文件
if [ ${ARCH} -eq 32 ]; then
	cp -R ../prepare_packet/iaf_packet.ko   env_setup
else
	cp -R ../prepare_packet/hb_pf_ring.ko   env_setup
fi
cp -R ../prepare_packet/rc.local        env_setup
cp -R ../prepare_packet/sudoers         env_setup
cp -R ../prepare_packet/sysctl.conf     env_setup
cp -R ../prepare_packet/tty6.conf       env_setup


#echo 打包 -------------------
tar zcf $pktdst env_setup  probe_autorun.sh
rm -rf env_setup

# 复制到发行目录
../copy_to_version.sh $pktdst

echo 恭喜：$pktdst 安装包制作完成!!!!!!!!!!!!



