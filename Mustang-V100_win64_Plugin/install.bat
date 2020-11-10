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

if exist "%HDDL_PATH%\bin\bsl_ori.dll" (
	echo Mustang-V100 Plugin has been installed
	echo Please Uninstall first
	exit /B 1
	pause
)

::backup original bsl_reset related file 
cd "%HDDL_PATH%\bin"
ren bsl.dll bsl_ori.dll
if exist "%HDDL_PATH%\bin\bsl_reset.exe" (
	ren bsl_reset.exe  bsl_reset_ori.exe
)  
::ren bsl_reset.exe  bsl_reset_ori.exe 
ren win_address_checktool.exe  win_address_checktool_ori.exe 

cd "%HDDL_PATH%\config"
ren  bsl.json bsl_ori.json 

::copy new bsl_reset related file
cd "%ROOT%"
copy /y "%ROOT%\bsl.dll" "%HDDL_PATH%\bin"
copy /y "%ROOT%\bsl_reset.exe" "%HDDL_PATH%\bin"
copy /y "%ROOT%\win_address_checktool.exe" "%HDDL_PATH%\bin"
copy /y "%ROOT%\bsl.json" "%HDDL_PATH%\config"

echo Iei hddl bsl_reset install complete

pause