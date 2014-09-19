rtbackup
========

Typical compilation steps:

    $ wget http://kernel.org/linux-x.x.x.tar.gz
    $ tar xvf linux-x.x.x.tar.gz
    $ cd linux-x.x.x
    $ make menuconfig
    $ make modules_prepare
    $ cd /path/to/rtbackup
    $ make linux-x86 KDIR=/path/to/kernel


To compile against the currently running kernel (kernel headers installed):

    $ make linux-x86 KDIR=/lib/modules/$(uname -r)/build


If a specific toolchain is desired for cross-compilation, provide the
CROSS_COMPILE variable during make:

    $ make android-arm CROSS_COMPILE=arm-linux-androideabi- KDIR=/path/to/kernel




Commands
========
