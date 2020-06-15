#!/bin/sh

if [ -z $1 ];then
    echo "Please input filename"
    exit
fi

FILE_NAME=$1
VAR_NAME=`echo ${FILE_NAME} | awk -F"." '{printf "%s\n",$1}'`
FILE_SIZE=`ls -l ${FILE_NAME} | awk '{print $5}'`

echo "const unsigned char local_audio_${VAR_NAME}[${FILE_SIZE}];"

echo "const unsigned char local_audio_${VAR_NAME}[${FILE_SIZE}] = {" > ${FILE_NAME}.c
hexdump -ve '16/1 "0x%.2X, ""\n"' ${FILE_NAME} | sed 's/0x,//g' >> ${FILE_NAME}.c
echo "};" >> ${FILE_NAME}.c
