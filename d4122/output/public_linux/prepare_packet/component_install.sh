# .bashrc: executed by bash(1) for non-login shells.

# 这里只安装lampp和驱动，其他程序的安装在unierm_install.sh中

echo
echo 检测安装包程序是否存在
if [ ! -r xampp-linux-1.7.3a.new.tar.gz ]; then
        echo 无法找到安装包 xampp-linux-1.7.3a.new.tar.gz
        exit
fi

if [ ! -r iaf_packet.ko  || ! -r hb_pf_ring.ko ]; then
        echo 无法找到驱动程序 iaf_packet.ko 或 hb_pf_ring.ko 
        exit
fi


echo
echo 安装lampp服务
echo "    如果需要立刻启动web程序，请执行以下命令"
echo "    cd /opt/lampp"
echo "    /opt/lampp start"
if [ ! -d /opt ]; then
        mkdir /opt
fi

ARCH=`getconf LONG_BIT`
if [ ${ARCH} -eq 32 ]; then
	
	tar zxf xampp-linux-1.7.3a.new.tar.gz -C /opt/
	
	echo
	echo
	echo 安装iaf_packet driver
	echo "    如果需要立刻加载驱动，请执行以下命令"
        echo "    insmod /usr/lib/iaf_packet.ko"
        cp iaf_packet.ko /usr/lib


else
	
	tar zxf xampp-linux-1.8.3-4.lite.tar.gz -C /opt/
	
	echo
	echo
	echo 安装hb_pf_ring driver
	echo "    如果需要立刻加载驱动，请执行以下命令"
        echo "    insmod /usr/lib/hb_pf_ring.ko"
        cp hb_pf_ring.ko /usr/lib
fi


echo
echo 复制启动文件rc.local
oldname=rc.local.`date +%Y%m%d_%H%M`
cp -R /etc/rc.local /etc/$oldname
cp -R rc.local /etc/

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
# 如果这个文件的权限不对，则apache中的命令无法执行
chmod -w /etc/sudoers

echo
echo 安装 console口登录补丁
if [ ! -r /etc/init/tty6.conf.ori ]; then
	cp -ar /etc/init/tty6.conf /etc/init/tty6.conf.ori
fi
cp -R tty6.conf /etc/init/



echo
echo 安装程序完成。
echo
