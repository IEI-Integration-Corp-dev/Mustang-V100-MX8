@echo off
echo Instal Iei HDDL BSL Reset

set ROOT=%~dp0
echo curpath=%ROOT%


::set "HDDL_PATH=C:\Intel\computer_vision_sdk\deployment_tools\inference_engine\external\hddl"
set "OPENVINO_INSTALL=C:\Program Files (x86)\IntelSWTools\openvino" 
::set "HDDL_PATH=C:\Program Files (x86)\IntelSWTools\openvino\deployment_tools\inference_engine\external\hddl"
if not exist "%OPENVINO_INSTALL%" (
	set "HDDL_PATH=C:\Intel\computer_vision_sdk\deployment_tools\inference_engine\external\hddl"
)else (
	set "HDDL_PATH=C:\Program Files (x86)\IntelSWTools\openvino\deployment_tools\inference_engine\external\hddl"
)

if not exist "%HDDL_PATH%" (
	echo OpenVino HDDL not exist
	exit /B 1
	pause
)
echo %HDDL_PATH% exist

::restore original bsl_reset related file 
cd "%HDDL_PATH%\bin"
if not exist "%HDDL_PATH%\bin\bsl_ori.dll" (
	echo remove bsl.dll fail
	exit /B 1
	pause
)
del bsl.dll
ren bsl_ori.dll bsl.dll 

if not exist "%HDDL_PATH%\bin\bsl_reset_ori.exe" (
	echo remove bsl_reset.exe fail
	exit /B 1
	pause
)
del bsl_reset.exe
ren bsl_reset_ori.exe bsl_reset.exe 

if not exist "%HDDL_PATH%\bin\win_address_checktool_ori.exe" (
	echo remove win_address_checktool.exe fail
	exit /B 1
	pause
)
del win_address_checktool.exe
ren win_address_checktool_ori.exe win_address_checktool.exe 

cd "%HDDL_PATH%\config"
if not exist "%HDDL_PATH%\config\bsl_ori.json" (
	echo remove bsl.json fail
	exit /B 1
	pause
)
del bsl.json
ren  bsl_ori.json bsl.json

echo Iei hddl bsl_reset uninstall complete

pause