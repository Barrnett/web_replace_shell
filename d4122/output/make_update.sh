# !/bin/sh

if [ -z $1 ];then
echo "update error: miss parameter!"
exit 1
fi

TMPDIR=$1

#rm -rf /home/ts/ts_prg/probe_bak
#/bin/mv /home/ts/ts_prg/probe /home/ts/ts_prg/probe_bak
#mkdir /home/ts/ts_prg/probe
#/bin/cp -rf ${TMPDIR}/ts/ts_prg/probe/* /home/ts/ts_prg/probe/
#mkdir /home/ts/ts_prg/probe/log
#/bin/cp -rf /home/ts/ts_prg/probe_bak/log/* /home/ts/ts_prg/probe/log/

rm -rf /home/ts_bak
/bin/mv /home/ts /home/ts_bak

mkdir /home/ts
/bin/cp -rf ${TMPDIR}/ts/version /home/ts/
/bin/cp -rf ${TMPDIR}/ts/ts_config /home/ts/
/bin/cp -rf ${TMPDIR}/ts/ts_prg /home/ts/

/bin/cp -rf /home/ts_bak/ts_config/* /home/ts/ts_config/
mkdir /home/ts/ts_prg/probe/log
/bin/cp -rf /home/ts_bak/ts_prg/probe/log/* /home/ts/ts_prg/probe/log/

chmod +x /home/ts/ts_prg/probe/testcontrol
chmod +x /home/ts/ts_prg/probe/fcserver
chmod +x /home/ts/ts_prg/probe/unierm_ctrl

if [ -f $TMPDIR/ts/snmp/snmpd ]; then
	/bin/cp -rf ${TMPDIR}/ts/snmp /home/ts/
	/bin/cp -rf /home/ts_bak/snmp/snmpd.conf /home/ts/snmp/
	chmod +x /home/ts/snmp/snmpd
else
	/bin/cp -rf /home/ts_bak/snmp /home/ts/
fi

if [ -f $TMPDIR/ts/web/SSM.war ]; then
	/usr/web/apache-tomcat-8.0.36/bin/shutdown.sh
	sleep 5
	rm -rf /usr/web/apache-tomcat-8.0.36/webapps/SSM
	rm -f /usr/web/apache-tomcat-8.0.36/webapps/SSM.war
	cp -rf $TMPDIR/ts/web/SSM.war /usr/web/apache-tomcat-8.0.36/webapps
	/usr/web/apache-tomcat-8.0.36/bin/startup.sh
	sleep 5
fi

echo 'update completed!'

sync
sync
reboot
exit



