# !/bin/sh

CUR_PATH=$PWD

MY_FUNC=$1

if [ "${MY_FUNC}" == "clean" ];then
	cd ../output/bin_64
	rm -f libACE.so
	rm -f libiconv.so
	rm -f libmysqlclient.so
	rm -f libsnmp++.so
	rm -f libpcap.so
	rm -f libldap.so
	rm -f libEtherIO.so
	cd ${CUR_PATH}
	
	cd ../output
	rm -rf bin_64/ini
	cd ${CUR_PATH}
	
	echo "clean complete!"
else
#creat so link
	cd ../output/bin_64
	rm -f libACE.so
	ln -s libACE.so.6.3.0 libACE.so
	rm -f libiconv.so
	ln -s libiconv.so.2        libiconv.so
	rm -f libmysqlclient.so
	ln -s libmysqlclient.so.16    libmysqlclient.so
	rm -f libsnmp++.so
	ln -s libsnmp++.so.2 libsnmp++.so
	rm -f libpcap.so
	ln -s libpcap.so.1       libpcap.so
	rm -f libldap.so
	ln -s libldap-2.4.so.2 libldap.so
	rm -f libEtherIO.so
	ln -s libetherio.so.1 libEtherIO.so
	cd ${CUR_PATH}

#copy ini
	cd ../output
	if [ -d bin_64/ini ];then
		rm -r bin_64/ini
	fi
	mkdir bin_64/ini
	cp ini/*.xml bin_64/ini/
	cd ${CUR_PATH}

	echo "configure complete!"
fi

exit

