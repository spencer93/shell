if [ $# \< 1 ]; then
   echo "mk FILENAME (without .c)"
   exit
fi

arm-none-eabi-as -mcpu=arm926ej-s us.s -o us.o
arm-none-eabi-gcc -c -mcpu=arm926ej-s -o $1.o $1.c #2> /dev/null
arm-none-eabi-ld -T u.ld us.o $1.o eoslib -Ttext=0x80000000 -o $1
echo "mounting..."
mount -o loop sdimage /mnt
echo "mount complete."

echo "copying to /mnt/bin/"
rm /mnt/bin/$1
cp -av $1 /mnt/bin

echo "copying complete."
#ls -l /mnt/bin

echo "unmounting /mnt"
umount /mnt
echo "umount complete."

rm $1 *.o

echo ready to go?
read dummy

qemu-system-arm -M versatilepb -m 256M -sd sdimage -kernel wanix \
-serial mon:stdio
