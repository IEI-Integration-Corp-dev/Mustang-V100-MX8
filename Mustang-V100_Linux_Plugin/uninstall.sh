#!/bin/bash

cur_dir=$(cd $(dirname $0); pwd)
#hddl_folder=/opt/intel/computer_vision_sdk/deployment_tools/inference_engine/external/hddl
#hddl_folder=/opt/intel/openvino/deployment_tools/inference_engine/external/hddl
#openvino 2019 r1 change folder name computer_vision_sdk to openvino
if [ -d "/opt/intel/openvino" ]
then
	hddl_folder=/opt/intel/openvino/deployment_tools/inference_engine/external/hddl
else
	hddl_folder=/opt/intel/computer_vision_sdk/deployment_tools/inference_engine/external/hddl
fi
#limt this plugin in openvino 2020

#if [ -z $(ls /opt/intel|grep 2020) ]
#then

#	echo "Error OpenVino 2020 not found!!!" 
#	echo "This Plugin is for OpenVino 2020 and above" 
#	exit -1
#else
#	echo "OpenVino 2020 found " 
#	hddl_folder=/opt/intel/openvino/deployment_tools/inference_engine/external/hddl
#fi


echo "HDDL_INSTALL_FOLDER = ${hddl_folder}"

install_lib=${hddl_folder}/lib
install_bin=${hddl_folder}/bin
install_cfg=${hddl_folder}/config
#install_dev=/etc/udev/rules.d

#echo "${cur_dir}"
if [ -d ${hddl_folder} ]
then
	if [ -f "${install_lib}/libbsl.so.0.ori" ]
	then
		sudo rm ${install_lib}/libbsl.so.0
		sudo mv ${install_lib}/libbsl.so.0.ori ${install_lib}/libbsl.so.0
	else
		echo "Error!! remove libblsl.so.0!!"
		exit -1
	fi

	if [ -f "${install_lib}/libbsl.so.ori" ]
	then
		sudo rm ${install_lib}/libbsl.so
		sudo mv ${install_lib}/libbsl.so.ori ${install_lib}/libbsl.so
	else
		echo "Error remove libblsl.so!!"
		exit -1
	fi

	if [ -f "${install_bin}/bsl_reset.ori" ]
	then
		sudo rm ${install_bin}/bsl_reset
		sudo mv ${install_bin}/bsl_reset.ori ${install_bin}/bsl_reset
	else
		echo "Error remove bsl_reset!!"
		exit -1
	fi


	if [ -f "${install_cfg}/bsl.json.ori" ]
	then
		sudo rm ${install_cfg}/bsl.json
		sudo mv ${install_cfg}/bsl.json.ori ${install_cfg}/bsl.json
	else
		echo "Error remove bsl.json!!"
		exit -1
	fi
else
	echo "Erro OpenVino HDDL not exist"
	exit -1
fi

#sudo udevadm control --reload-rules
#sudo udevadm trigger
#sudo ldconfig

echo "Hddl bsl_reset uninstall complete"
echo "Please reboot PC"

exit 0
