#/bin/bash

cp generated/images.zip 1.zip
cp 1.zip 2.zip
product image 1.zip -l -v "1000" -p
product image 2.zip -l -v "2000" -p
echo "++++++++++++++Generate FOTA BIN..."
product diff -f 1.zip 2.zip -v "2000" -r -o fota.bin
echo "++++++++++++++Burning images.zip..."
product flash 1.zip -a -f ch2601_flash.elf -x .gdbinit
echo "++++++++++++++Burning fota.bin..."
product flashbin fota.bin 0x18049000 -f ch2601_flash.elf -x .gdbinit
rm -rf 1.zip 2.zip fota.bin
echo "++++++++++++++over..."
