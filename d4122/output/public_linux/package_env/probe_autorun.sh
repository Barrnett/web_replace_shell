#!/bin/sh

cur_path=$PWD

path_prg=$1
if [ -z $1 ]; then
	path_prg=/xiaocao_prg
fi

path_db=$2
if [ -z $2 ]; then
	path_db=/xiaocao_data
fi

path_data=$3
if [ -z $3 ]; then
	path_data=/xiaocao_data
fi

cd env_setup

echo
ARCH=`getconf LONG_BIT`
if [ ${ARCH} -eq 32 ]; then
	echo ��װiaf_packet����
	#echo "    �����Ҫ���̼�����������ִ����������"
	#echo "    insmod /usr/lib/iaf_packet.ko"
	cp iaf_packet.ko /usr/lib
else
	echo ��װhb_pf_ring driver
	#echo "    �����Ҫ���̼�����������ִ����������"
	#echo "    insmod /usr/lib/hb_pf_ring.ko"
	cp hb_pf_ring.ko /usr/lib
fi


echo
echo ���������ļ�rc.local

rc_local_backup_path="/etc/rc.d/rc.local.backup"
if [ ! -d ${rc_local_backup_path} ]; then
    mkdir ${rc_local_backup_path}
fi

rc_local_file="/etc/rc.d/rc.local"
oldname=rc.local.`date +%Y%m%d_%H%M`
cp -R ${rc_local_file} ${rc_local_backup_path}/$oldname
cp -R rc.local ${rc_local_file}

if [ -r ${rc_local_file} ]; then
		s1="/xiaocao_prg/unierm/probe"
		d1="${path_prg}/unierm/probe"

		if [ "$s1" != "$d1" ];then
		    sed -i "s:${s1}:${d1}:g" ${rc_local_file}
		fi
               		
fi

# ��������ķ���ǽ
iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport 61777 -j ACCEPT
iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport 61666 -j ACCEPT
iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport 83 -j ACCEPT

iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 61777 -j ACCEPT
iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 61666 -j ACCEPT
iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 83 -j ACCEPT

/etc/rc.d/init.d/iptables save
service iptables restart

# ���¼��䲻��Ҫ����Ϊ�Ѿ���/etc/rc.d/rc.local����������
# MY_UNIERM_LD_PATH="export LD_LIBRARY_PATH=${path_prg}/unierm/probe/:\$LD_LIBRARY_PATH"
# MY_UNIERM_PATH="export PATH=${path_prg}/unierm/probe/:\$PATH"
# sed -i '$a'"${MY_UNIERM_LD_PATH}"' ' /etc/profile
# sed -i '$a'"${MY_UNIERM_PATH}"' ' /etc/profile


echo
echo ��װ Socket TIME_WAIT ����
if [ ! -r /etc/sysctl.conf.ori ]; then
        cp -R /etc/sysctl.conf /etc/sysctl.conf.ori
fi
cp -R sysctl.conf /etc/

echo
echo ��װ apache Ȩ�޲���
chmod +w /etc/sudoers
if [ ! -r /etc/sudoers.ori ]; then
	cp -R /etc/sudoers /etc/sudoers.ori
fi
cp -R sudoers /etc/

if [ -r /etc/sudoers ]; then
	 	s1="/xiaocao_prg/unierm/probe"
		d1="${path_prg}/unierm/probe"

		if [ "$s1" != "$d1" ];then
		    sed -i "s:${s1}:${d1}:g" /etc/sudoers
		fi
fi

# �������ļ���Ȩ�޲��ԣ���apache�е������޷�ִ��
chmod -w /etc/sudoers

echo
echo ��װ console�ڵ�¼����
if [ ! -r /etc/init/tty6.conf.ori ]; then
	cp -ar /etc/init/tty6.conf /etc/init/tty6.conf.ori
fi
cp -R tty6.conf /etc/init/

cd $cur_path

echo
echo ��װ������ɡ�
echo
