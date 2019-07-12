# !/bin/sh

rm -rf ts/
mkdir ts
mkdir ts/version ts/ts_config ts/ts_config/probe_config ts/ts_prg ts/ts_prg/probe ts/snmp ts/web
if [ -f ../../../common/d4122/version.xml ]; then
	LAST=`awk '/BUILD/' ../../../common/d4122/version.xml | sed 's/<BUILD>//g' | sed 's/<\/BUILD>//g'`
	NEW=`echo $LAST | awk '{print $1+1}'`
	sed -i "s/<BUILD>.*<\/BUILD>/<BUILD>${NEW}<\/BUILD>/" ../../../common/d4122/version.xml
	cp ../../../common/d4122/version.xml ts/version/
else
	echo 'Error: version.xml is missing'
fi

cp -rf bin_64/* ts/ts_prg/probe/
rm -rf ts/ts_prg/probe/log
cp ../../../common/d4122/ts_config/*.xml ts/ts_config/probe_config/

##snmp package
if [ -f ../../../snmp/uns/snmpd ]; then
	cp ../../../snmp/uns/snmpd ts/snmp/
	cp ../../../snmp/uns/snmpd.conf ts/snmp/
fi

##web package
if [ -f ../../../web/SSM.war ]; then
	cp -f ../../../web/SSM.war ts/web/
fi

##
chmod +x *.sh
cp -f make_install.sh ts/
cp -f make_update.sh ts/

##save default configuration
cp -rf ts/ts_config ts/ts_prg/probe/

##make package
TARGET="uns.v10r5.d4122.build"
rm -f ts_release.tar.gz
tar zcvf ts_release.tar.gz ts
cat vsetup.sh ts_release.tar.gz > ${TARGET}${NEW}
chmod +x ${TARGET}${NEW}

echo 'make package completed!'

exit
