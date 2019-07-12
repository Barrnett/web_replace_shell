#!/bin/sh

pktdst=env_setup.exe

echo
echo ׼������ $pktdst ��װ��

#echo ɾ�� env_setup �µ������ļ�
if [ -d env_setup ]; then
        rm -rf env_setup
fi
if [ -r $pktdst ]; then
        rm -rf $pktdst
fi

#echo ����Ŀ¼
mkdir env_setup

#echo �����ļ�
if [ ${ARCH} -eq 32 ]; then
	cp -R ../prepare_packet/iaf_packet.ko   env_setup
else
	cp -R ../prepare_packet/hb_pf_ring.ko   env_setup
fi
cp -R ../prepare_packet/rc.local        env_setup
cp -R ../prepare_packet/sudoers         env_setup
cp -R ../prepare_packet/sysctl.conf     env_setup
cp -R ../prepare_packet/tty6.conf       env_setup


#echo ��� -------------------
tar zcf $pktdst env_setup  probe_autorun.sh
rm -rf env_setup

# ���Ƶ�����Ŀ¼
../copy_to_version.sh $pktdst

echo ��ϲ��$pktdst ��װ���������!!!!!!!!!!!!



