
修改install_db.sh文件，增加参数输入，修改my.cnf文件内容
修改menu64.sh 增加配置文件读入路径，copy配置文件到etc
修改prepare_packet\unierm_install.sh，增加传入参数
修改package_unierm_data\program_config.sh
修改package_unierm_data\bak_program_config.sh





v5.0.21.7.1（20110612）
	将bak_program_config和program_config分别打成2个包，在做update时，需要复制bak_program_config包
	将httpd.conf放入升级包中，需要的版本可以覆盖源文件
	关闭http的修改8080端口功能


5.0.20.2
	因为和上一版本相比，要升级iaf_packet.ko驱动，当前驱动是在安装系统是装好的（安装环境包）里边还包括lampp相关部分，是不可升级的。
	1、将lampp的安装从component_install中分离出来，	让lampp和mysql等一样从menu中安装。
	2、增加一个env_setup.exe，可安装原来的component_install相关部分，去除lampp
	3、同时修改Upgrade_Tool.xml脚本，让本版本中的env_setup.exe组件可以进行安装

	删除了package_web_ROOT\www目录下和其他目录重复的文件，可以防止每次在cvs commit时排除文件的麻烦