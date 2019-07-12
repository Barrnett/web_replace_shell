#!/bin/sh


echo 复制环境库

cp -R $ver/env_setup.exe                  $publish_setup
cp -R prepare_packet/install_mysql_db.tgz $publish_setup
if [ ${ARCH} -eq 32 ]; then
	cp -R prepare_packet/menu.sh             		 $publish_setup
else
	cp -R prepare_packet/prg_install_menu.sh           $publish_setup
	cp -R prepare_packet/unierm_setup_path.ini  		 $publish_setup
fi


cp -R prepare_packet/vsetup.sh   $publish_root

BACKUP_CDIR=`pwd` #记住当前目录
cd ${publish_root}
tar zcf setup.tar.gz setup
chmod +x vsetup.sh
cat vsetup.sh setup.tar.gz > ${public_file_name}.upl3
chmod +x ${public_file_name}.upl3
mv ${public_file_name}.upl3 ../
cd ../
rm ${public_file_name} -rf
cd ${BACKUP_CDIR}

