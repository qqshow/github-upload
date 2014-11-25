#!/bin/bash
#This script is distributed to uninstall real time backup module... 
#AUTHOR:lb 20141125
BASE=`dirname $(pwd)/$0`
UNAME=`uname -r`

echo "uninstall module" >> /tmp/install
echo "`date`" >>/tmp/install

rmmod rtbackup
if [ $? -eq 0 ] 
then 
	echo "rmmod failed." >>/tmp/install
fi

#rmmod
rm -rf /lib/modules/$UNAME/extra/rtbackup/

#rm in sysinit
sed -i -e '/rtbackup/d'  /etc/rc.d/rc.sysinit

#rm config file
rm -rf /etc/rtb.cnf

lsmod | grep rtbackup
if [ $? -ne 0 ] 
then 
	echo "sucess" >>/tmp/install
	echo "0"
	exit
fi
echo "1"
