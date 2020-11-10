1.pre-requirement module install 
	$sudo apt-get install libudev-dev cmake
2.in iEiMustangR11Linux folder
	$mkdir build
	$cd build
	$cmake ..
	$make -j8
3.copy the rule file to  
	$cd ..
	$sudo cp iEiR11.rules /etc/udev/rules.d
4.in ubuntu 16.04 , power on to auto execuate ,add binary file path to /etc/rc.local before "exist 0"
	example
	$sudo gedit /etc/rc.local
	add <iEiMustangR11Linux binary path>  to /etc/rc.local before exist 0
