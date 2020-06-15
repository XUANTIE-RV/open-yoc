#!/bin/sh

AUDIO_FILE_LIST=`ls *.mp3`

echo "#ifndef _AUDIO_RES_H_" > audio_res.h
echo "#define _AUDIO_RES_H_" >> audio_res.h

#### enum define ####
echo "typedef enum {" >> audio_res.h
for i in ${AUDIO_FILE_LIST};do
    ENUM_NAME=`echo $i | awk -F"." '{printf "%s,\n",$1}' | tr '[a-z]' '[A-Z]'`
    echo "LOCAL_AUDIO_${ENUM_NAME}" >> audio_res.h
done
echo "} local_audio_name_t;" >> audio_res.h
echo "int local_audio_play(local_audio_name_t name);" >> audio_res.h

#### resource declare ####
for i in ${AUDIO_FILE_LIST};do
    echo "bin2c $i"
    RES_DECALARE=`sh bin2c.sh $i`
    echo "extern ${RES_DECALARE}">> audio_res.h
done

#### resource define ####
echo "#define AUDIO_RES_ARRAY \\" >> audio_res.h
for i in ${AUDIO_FILE_LIST};do
    VAR_NAME=`echo $i | awk -F"." '{printf "%s\n",$1}'`
    echo "{local_audio_${VAR_NAME},sizeof(local_audio_${VAR_NAME})}, \\" >> audio_res.h
done
echo "{NULL,0}" >> audio_res.h

echo "#endif" >> audio_res.h
