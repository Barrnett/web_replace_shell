#!/bin/sh


pktdst=silent_prg_config_setup.exe
pktdst_bak=silent_bak_prg_config_setup.exe
pktdst_db=install_mysql_db.tgz
datadir=../prepare_program_config

echo
echo ׼������ $pktdst ��װ��
echo
if [ -d unierm_data ]; then
	rm -rf unierm_data
fi

if [ -r $pktdst ]; then
	rm -rf $pktdst
fi
if [ -r $pktdst_bak ]; then
	rm -rf $pktdst_bak
fi
if [ -r $pktdst_db ]; then
	rm -rf $pktdst_db
fi

if [ -d install_mysql_db ]; then
	rm -rf install_mysql_db
fi




#echo ��������
mkdir unierm_data
cp -R $datadir/fc_server_config	unierm_data 
cp -R $datadir/local_machine	unierm_data 
cp -R $datadir/probe_config	unierm_data 

cp -R program_config.sh probe_autorun.sh
tar zcf $pktdst unierm_data probe_autorun.sh


#echo �����ָ����ܵı���Ŀ¼
mv unierm_data bak_program_config

cp bak_program_config.sh probe_autorun.sh
tar zcf $pktdst_bak bak_program_config probe_autorun.sh
rm -rf bak_program_config

#echo �������ݿⰲװ��
mkdir install_mysql_db
cp -R ../prepare_db/linux_req/.                 install_mysql_db/
cp -R ../prepare_db/netems_db_setup.linux.rar   install_mysql_db/netems_db_setup.exe
cp -R ../prepare_db/mysql_db_setup.linux.rar    install_mysql_db/mysql_db_setup.exe

#ɾ��������CVS�ļ�
find install_mysql_db -name CVS | xargs rm -rf

tar zcf $pktdst_db install_mysql_db
rm -rf install_mysql_db


# ���Ƶ�����Ŀ¼
mv $pktdst ../prepare_packet
mv $pktdst_db ../prepare_packet
../copy_to_version.sh $pktdst_bak

echo ������ $pktdst $pktdst_bak install_mysql_db.tgz �������!!!
echo
