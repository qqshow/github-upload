gcc -fPIC -g -shared -o libcommunicate.so communicate.c -lpthread

gcc -o netlink main.c /mnt/hgfs/linuxrt/rtbackup/trunk/communicate/libcommunicate.so
