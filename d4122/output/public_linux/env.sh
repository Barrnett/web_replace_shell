#!/bin/bash
 
echo $HadComing
# 修改了外部的调用方式，为了防止重复进入，增加下列部分
if [ -z $HadComing ] ; then
#        echo exit because already HadComing = $HadComing
        cur_date=`date +%Y%m%d`
        cur_time=`date +%H%M%S`
#        return
fi
export HadComing="yes"
export path_win_bin=$PWD/win_for_linux_setup
 
if [[ $OEM = "unierm" ]]; then
	export ver=5.0.55.1.0
	export VERSION_FILE_XML_SRC=unierm_version_wanren.xml
	export UPGRADE_TOOL_XML_SRC=Upgrade_Tool_wanren_64.xml
	export UPGRADE_TOOL_XML_CORE_SRC=Upgrade_Tool_wanren_64.xml.core
else	
	export ver=3.0.55.1.0
	export VERSION_FILE_XML_SRC=unierm_version.xml
	export UPGRADE_TOOL_XML_SRC=Upgrade_Tool_64.xml
	export UPGRADE_TOOL_XML_CORE_SRC=Upgrade_Tool_64.xml.core
fi
export VERSION_FILE_XML_DST=unierm_version.xml
export UPGRADE_TOOL_XML_DST=Upgrade_Tool.xml
export UPGRADE_TOOL_XML_CORE_DST=Upgrade_Tool.xml.core
export ARCH=`getconf LONG_BIT`
ver_short=${ver:4}
export path_root_linux=$PWD
export publish_root_all=v${ver_short}_${cur_date}_${cur_time}_linux${ARCH}
export publish_root=${publish_root_all}/${OEM}_v${ver}_${cur_date}_${cur_time}_linux${ARCH}
export public_file_name=${OEM}_v${ver}_${cur_date}_${cur_time}_linux${ARCH}
export publish_setup=${publish_root}/setup
export path_bin_root=${path_root_linux}/../bin_${ARCH}
export path_ini_root=${path_root_linux}/../exe_ini_path
export THIS_OEM_INFO_PATH=${path_root_linux}/package_oem/${OEM}
if [ ! -d $publish_root ]; then
 	mkdir -p $publish_root
fi
 
if [ ! -d $publish_setup ]; then
 	mkdir -p $publish_setup
fi
 
