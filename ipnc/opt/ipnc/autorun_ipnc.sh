#!/bin/sh

cd /opt/ipnc
echo ' inside autorun '
insmod sbull.ko
umount /mnt/ramdisk 2>/dev/null
rmmod sbull.ko 2>/dev/null
insmod sbull.ko
sleep 1
mkdosfs -v /dev/sbulla 2>/dev/null
mkdir /mnt/ramdisk 2>/dev/null
mount -t vfat /dev/sbulla /mnt/ramdisk 2>/dev/null
#insmod g_file_storage.ko file=/dev/sbulla stall=0 removable=1

amixer sset 'PGA' 80
sleep 1

cd /opt/ipnc
./boot_proc 1&
sleep 2
./boa -c /etc &
sleep 2
#./boot_proc 2&
#sleep 2


if [ -b "/dev/disk/by-path/platform-mmci-omap-hs.1-part1" ] &&
   [ -b "/dev/disk/by-path/platform-mmci-omap-hs.1-part2" ]
then
	! test -d "/media/hi_mmc/1" && mkdir -p /media/hi_mmc/1
	! test -d "/media/hi_mmc/2" && mkdir -p /media/hi_mmc/2
	mount -t ext4 /dev/disk/by-path/platform-mmci-omap-hs.1-part1 /media/hi_mmc/1
	mount -t ext4 /dev/disk/by-path/platform-mmci-omap-hs.1-part2 /media/hi_mmc/2
fi


./log_mod &

#sleep 20
./nvcsdk &

#sleep 2
./sip &

#sleep 5
./vd &

./pid_status &

#sleep 10
./h264_pack &

#sleep 10
./monitor &

exit 0

