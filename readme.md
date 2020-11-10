
#hddl_bsl build environment
base on openvino_2020.1.023 
this Plugin will limit to openvino 2020 related version
======================================
Description:	Ubuntu 18.04.3 LTS
kernel 5.0.0-37-generic
======================================
Description:	Ubuntu 16.04.3 LTS
kernel 4.15.0-76-generic


#install procdure

1. $sudo ./install.sh
2. reboot pc
3. start hddldaemon

#uninstall procdure

1. $sudo ./uninstall.sh
2. reboot pc


# libbsl

---
this project includes some tools and a library.

1. `libbsl`: A library to reset myraidx for HDDL.
2. `bsl_reset`: A tool to reset devices based on `libbsl`
3. `bsl.json`: A confi for different reset device

for different device configure
### for iei usb mcu device reset (iei pcie interface series)
  "usb-mcu": {
    "enabled": true,
    "comment": "iei msp-430 mcu controller"
  },

 

