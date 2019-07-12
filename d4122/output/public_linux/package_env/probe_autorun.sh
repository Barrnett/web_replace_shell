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
	echo 安装iaf_packet驱动
	#echo "    如果需要立刻加载驱动，请执行以下命令"
	#echo "    insmod /usr/lib/iaf_packet.ko"
	cp iaf_packet.ko /usr/lib
else
	echo 安装hb_pf_ring driver
	#echo "    如果需要立刻加载驱动，请执行以下命令"
	#echo "    insmod /usr/lib/hb_pf_ring.ko"
	cp hb_pf_ring.ko /usr/lib
fi


echo
echo 复制启动文件rc.local

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

# 本机自身的防火墙
iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport 61777 -j ACCEPT
iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport 61666 -j ACCEPT
iptables -D INPUT -m state --state NEW -m tcp -p tcp --dport 83 -j ACCEPT

iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 61777 -j ACCEPT
iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 61666 -j ACCEPT
iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 83 -j ACCEPT

/etc/rc.d/init.d/iptables save
service iptables restart

# 以下几句不需要，因为已经在/etc/rc.d/rc.local里面设置了
# MY_UNIERM_LD_PATH="export LD_LIBRARY_PATH=${path_prg}/unierm/probe/:\$LD_LIBRARY_PATH"
# MY_UNIERM_PATH="export PATH=${path_prg}/unierm/probe/:\$PATH"
# sed -i '$a'"${MY_UNIERM_LD_PATH}"' ' /etc/profile
# sed -i '$a'"${MY_UNIERM_PATH}"' ' /etc/profile


echo
echo 安装 Socket TIME_WAIT 补丁
if [ ! -r /etc/sysctl.conf.ori ]; then
        cp -R /etc/sysctl.conf /etc/sysctl.conf.ori
fi
cp -R sysctl.conf /etc/

echo
echo 安装 apache 权限补丁
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

# 如果这个文件的权限不对，则apache中的命令无法执行
chmod -w /etc/sudoers

echo
echo 安装 console口登录补丁
if [ ! -r /etc/init/tty6.conf.ori ]; then
	cp -ar /etc/init/tty6.conf /etc/init/tty6.conf.ori
fi
cp -R tty6.conf /etc/init/

cd $cur_path

echo
echo 安装程序完成。
echo
