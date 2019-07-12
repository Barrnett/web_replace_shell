# .bashrc: executed by bash(1) for non-login shells.

# ����ֻ��װlampp����������������İ�װ��unierm_install.sh��

echo
echo ��ⰲװ�������Ƿ����
if [ ! -r xampp-linux-1.7.3a.new.tar.gz ]; then
        echo �޷��ҵ���װ�� xampp-linux-1.7.3a.new.tar.gz
        exit
fi

if [ ! -r iaf_packet.ko  || ! -r hb_pf_ring.ko ]; then
        echo �޷��ҵ��������� iaf_packet.ko �� hb_pf_ring.ko 
        exit
fi


echo
echo ��װlampp����
echo "    �����Ҫ��������web������ִ����������"
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
	echo ��װiaf_packet driver
	echo "    �����Ҫ���̼�����������ִ����������"
        echo "    insmod /usr/lib/iaf_packet.ko"
        cp iaf_packet.ko /usr/lib


else
	
	tar zxf xampp-linux-1.8.3-4.lite.tar.gz -C /opt/
	
	echo
	echo
	echo ��װhb_pf_ring driver
	echo "    �����Ҫ���̼�����������ִ����������"
        echo "    insmod /usr/lib/hb_pf_ring.ko"
        cp hb_pf_ring.ko /usr/lib
fi


echo
echo ���������ļ�rc.local
oldname=rc.local.`date +%Y%m%d_%H%M`
cp -R /etc/rc.local /etc/$oldname
cp -R rc.local /etc/

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
# �������ļ���Ȩ�޲��ԣ���apache�е������޷�ִ��
chmod -w /etc/sudoers

echo
echo ��װ console�ڵ�¼����
if [ ! -r /etc/init/tty6.conf.ori ]; then
	cp -ar /etc/init/tty6.conf /etc/init/tty6.conf.ori
fi
cp -R tty6.conf /etc/init/



echo
echo ��װ������ɡ�
echo
