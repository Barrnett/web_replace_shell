#!/bin/sh

rm -rf import
mkdir import

mkdir import/ByPassManager
cp ByPassManager/BypassTool.h 			import/ByPassManager/
cp ByPassManager/BypassToolManager.h 	import/ByPassManager/


mkdir import/LicenseChecker
cp LicenseInfo/CheckLicenseCommon.h		import/LicenseChecker/
cp LicenseChecker/LicenseChecker.h			import/LicenseChecker/
cp LicenseChecker/LicenseCheckerExport.h	import/LicenseChecker/
cp LicenseChecker/LicenseGenrator.h			import/LicenseChecker/
cp LicenseChecker/LicenseOrigInfo.h			import/LicenseChecker/

cp cpf_wrappers/cpf/export/cpf		import -rf
cp cpf_wrappers/cpf2/export/cpf2	import -rf

cp packetio_wrappers/PacketIO/export/PacketIO		import -rf
cp packetio_wrappers/PacketIOUI/export/PacketIOUI	import -rf

#mkdir import/TeeChart
#cp TeeChartLib/*.h import/TeeChart -rf

cp import ../security_probe -R

