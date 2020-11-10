#!/bin/bash


if [ -d "/opt/intel/openvino" ]
then
	hddl_folder=/opt/intel/openvino/deployment_tools/inference_engine/external/hddl
else
	hddl_folder=/opt/intel/computer_vision_sdk/deployment_tools/inference_engine/external/hddl
fi
#limt this plugin in openvino 2020

#if [ -z $(ls /opt/intel|grep 2020) ]
#then

#	echo "Error OpenVino 2020 related version not found!!! "
#	exit -1
#else
#	echo "OpenVino 2020 related version found " 
	hddl_folder=/opt/intel/openvino/deployment_tools/inference_engine/external/hddl
#fi
cur_dir=$(cd $(dirname $0); pwd)
OSVer=$(echo $(lsb_release -rs))

if [ $OSVer != "16.04" ] && [ "$OSVer" != "18.04" ]
then 
	echo "OS Version Error !!"
	echo "Only Support Ubuntu 16.04 and Ubuntu 18.04"
	exit -1
fi

libdir=${cur_dir}/${OSVer}
echo "Lib Dir =${libdir}"


echo "HDDL_INSTALL_FOLDER = ${hddl_folder}"

install_lib=${hddl_folder}/lib
install_bin=${hddl_folder}/bin
install_cfg=${hddl_folder}/config
install_dev=/etc/udev/rules.d

#echo "${cur_dir}"
if [ -d ${hddl_folder} ]
then

	if [ -f "${install_lib}/libbsl.so.ori" ]
	then
		echo "Please remove previous install first!!"
		exit -1
	fi
	
	#backup original file
	sudo mv ${install_lib}/libbsl.so.0 ${install_lib}/libbsl.so.0.ori
	sudo mv ${install_lib}/libbsl.so ${install_lib}/libbsl.so.ori
	if [ -f ${install_bin}/bsl_reset ]
	then
		sudo mv ${install_bin}/bsl_reset ${install_bin}/bsl_reset.ori
	fi
	sudo mv ${install_cfg}/bsl.json ${install_cfg}/bsl.json.ori
	
	#copy new file

	sudo cp ${libdir}/libbsl.so.0 ${install_lib}
	sudo cp ${libdir}/libbsl.so ${install_lib}
	sudo cp ${libdir}/bsl_reset ${install_bin}
	sudo cp ${cur_dir}/ieimcu.rules ${install_dev}
	sudo cp ${cur_dir}/bsl.json ${install_cfg}

else
	echo "Erro OpenVino HDDL not exist"
	exit -1
fi

#sudo udevadm control --reload-rules
#sudo udevadm trigger
#sudo ldconfig

echo "Hddl bsl_reset install complete"
echo "Please reboot PC"

exit 0
