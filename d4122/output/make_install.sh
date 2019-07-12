# !/bin/sh

if [ -z $1 ];then
echo "install error: miss parameter!"
exit 1
fi

TMPDIR=$1

rm -rf /home/ts_bak/
rm -rf /home/ts/
mkdir /home/ts/
#tar zxvf ts_release.tar.gz -C /home/
cp -rf $TMPDIR/ts/version /home/ts/
cp -rf $TMPDIR/ts/ts_config /home/ts/
cp -rf $TMPDIR/ts/ts_prg /home/ts/
rm -rf /home/ts/ts_prg/probe/log/*
cp -rf $TMPDIR/ts/snmp /home/ts/

if [ -f "/usr/lib/hb_pf_ring.ko" ]; then
#  rmmod hb_pf_ring
  rm -f /usr/lib/hb_pf_ring.ko
  echo 'remove driver...'
fi

cp -f /home/ts/ts_prg/probe/hb_pf_ring.ko /usr/lib/
chmod +x /usr/lib/hb_pf_ring.ko

#insmod /usr/lib/hb_pf_ring.ko
if grep -Fxq 'insmod /usr/lib/hb_pf_ring.ko' /etc/rc.d/rc.local 
then
  echo 'check driver ok!'
else
  sed -i '$a\insmod /usr/lib/hb_pf_ring.ko' /etc/rc.d/rc.local
  echo 'install driver ok!'
fi

if grep -Fxq '/home/ts/ts_prg/probe/unierm_ctrl &' /etc/rc.d/rc.local 
then
  echo 'check startup ok!'
else
  sed -i '$a\sleep 10s\n/home/ts/ts_prg/probe/unierm_ctrl &' /etc/rc.d/rc.local
  echo 'install startup ok!'
fi

if grep -Fxq '/home/ts/snmp/snmpd -c /home/ts/snmp/snmpd.conf' /etc/rc.d/rc.local 
then
  echo 'check snmp ok!'
else
  sed -i '$a\sleep 10s\n/home/ts/snmp/snmpd -c /home/ts/snmp/snmpd.conf' /etc/rc.d/rc.local
  echo 'install snmp ok!'
fi

if grep -Fxq '/usr/web/apache-tomcat-8.0.36/bin/startup.sh' /etc/rc.d/rc.local 
then
  echo 'check webserver ok!'
else
  sed -i '$a\sleep 10s\nexport JAVA_HOME=/usr/java/jdk1.7.0_79\n/usr/web/apache-tomcat-8.0.36/bin/startup.sh' /etc/rc.d/rc.local
  if grep -Fq '<Context path=' /usr/web/apache-tomcat-8.0.36/conf/server.xml
  then
    sed -i '/<Context path=/d' /usr/web/apache-tomcat-8.0.36/conf/server.xml
  fi
  sed -i '/unpackWARs=/a\        <Context path=\"\" docBase=\"/usr/web/apache-tomcat-8.0.36/webapps/SSM\" debug=\"0\" reloadable=\"true\" />' /usr/web/apache-tomcat-8.0.36/conf/server.xml
  echo 'install webserver ok!'
fi

if [ ! -z $2 ]; then
  NEXT_IP=$2
  if grep -Fq '<LOCAL_MANAGEMENT_CONFIG>' /home/ts/ts_config/probe_config/local_config.xml
  then
    sed -i '/<LOCAL_MANAGEMENT_CONFIG>/,/<\/LOCAL_MANAGEMENT_CONFIG>/{/<IPADDR>/d}' /home/ts/ts_config/probe_config/local_config.xml
  fi
  sed -i "/<INTERFACE>eth2<\/INTERFACE>/a\        <IPADDR>${NEXT_IP}</IPADDR>" /home/ts/ts_config/probe_config/local_config.xml
  echo "install management ip: ${NEXT_IP}"
fi


chmod +x /home/ts/ts_prg/probe/testcontrol
chmod +x /home/ts/ts_prg/probe/fcserver
chmod +x /home/ts/ts_prg/probe/unierm_ctrl
chmod +x /home/ts/snmp/snmpd

chmod +x /usr/web/apache-tomcat-8.0.36/bin/shutdown.sh
chmod +x /usr/web/apache-tomcat-8.0.36/bin/startup.sh
/usr/web/apache-tomcat-8.0.36/bin/shutdown.sh
sleep 5
rm -rf /usr/web/apache-tomcat-8.0.36/webapps/SSM
rm -f /usr/web/apache-tomcat-8.0.36/webapps/SSM.war
cp -rf $TMPDIR/ts/web/SSM.war /usr/web/apache-tomcat-8.0.36/webapps
/usr/web/apache-tomcat-8.0.36/bin/startup.sh
sleep 5

echo 'install completed!'

sync
sync
reboot
exit
