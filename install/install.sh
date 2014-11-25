#!/bin/bash
#This script is distributed to install real time backup module... 
#AUTHOR:lb 20141125
if [ $(echo $0 | grep '^/') ]; then  
    path=$(dirname $0)  
else  
    path=$(pwd)/$(dirname $0)  
fi  
BASE=`dirname $(pwd)/$0`
UNAME=`uname -r`

echo "install module" >> /tmp/install
echo "`date`" >>/tmp/install
#cp module
mkdir -p /lib/modules/$UNAME/extra/rtbackup/
install -m 755 $path/rtbackup.ko /lib/modules/$UNAME/extra/rtbackup/

cp -rf /etc/rc.d/rc.sysinit /etc/rc.d/rc.sysinit.lbbak
cat /etc/rc.d/rc.sysinit | grep rtbackup >/dev/null 2>&1
if [ $? -ne 0 ] 
then 
	echo "modprobe rtbackup" >> /etc/rc.d/rc.sysinit
fi

#depmod
/sbin/depmod

#load rtbackup.ko into kernel
modprobe rtbackup
if [ $? -eq 0 ] 
then 
	echo "sucess" >>/tmp/install
fi
