build arduino from source
sudo apt-get install git
sudo apt-get install openjdk-7-jdk
sudo apt-get install gcc-avr avr-libc
sudo apt-get install make
sudo apt-get install ant 
cd /home/USER/sandbox/Arduino/build
ant
ant run

to install hardware
dmesg tail
ls -l /dev/ttyACM0 
usermod -a -G dialout jbreslin
sudo usermod -a -G dialout jbreslin
ls -ld /run/lock
sudo chmod o+rwx /run/lock/
 

