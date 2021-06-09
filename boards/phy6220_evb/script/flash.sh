#!/bin/sh

#flash
if [ "$1" = "flash" ];then
     product flash ./generated/images.zip -w prim -f ./script/PHY6220_Flash.elf
elif [ "$1" = "flashall" ];then
     product flash ./generated/images.zip -a -f ./script/PHY6220_Flash.elf
else
  echo "err param"
fi






