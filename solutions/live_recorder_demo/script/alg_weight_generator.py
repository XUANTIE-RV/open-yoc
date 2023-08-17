#!/usr/bin/python3

import sys
import serial
import time
import datetime
import sys
import os
import json
import hashlib

# weight bin concatenate all the input weights file into one whole bin file
#
# bin format as follows:
#|--4 bytes--|-- 16 bytes --|-- 4 bytes --|-- x bytes --|--512*M-x-4 bytes--|-- N1 bytes --|-- N2 bytes --|   .....  |-- Nm bytes --|
#|-- magic --|--md5 check --| header size |    header   |      paddings     |    weight1   |    weight1   |   .....  |    weight1   |
# 
# magic: 0x57454754 - 'WEGT'
# md5 check: md5 checksum from header size till end of image
# header size: size of header and paddings
# header: in json format, e.g.:
#               {'model_facedet.params': [512, 169340], 'model_single_ir.params': [169852, 158112]}
#
#

if (len(sys.argv) < 2 or not os.path.isdir(sys.argv[1])) :
    print("Command format: python alg_weight_generator.py <weights folder> [<output file name>], e.g.:")
    print("python alg_weight_generator.py weights/ weight")
    exit(-1)

output_file = sys.argv[2] if len(sys.argv) > 2 else "weight"
weight_files = []
folder_name = sys.argv[1]
weight_files = os.listdir(folder_name)

def align(weight, align_len):
    size = len(weight)
    aligned_len = (size + align_len - 1) // align_len * align_len
    padding_size = aligned_len - size
    weight += bytes(padding_size)           # padding with all zeros
    return size + padding_size, weight

def md5_checksum(bin):
    md5 = hashlib.md5()
    md5.update(bin)
    digest = md5.hexdigest()

    md5Digest = b''
    for i in range(0, len(digest), 2):
        # print(md5Result[i : i+2], bytes(int(md5Result[i : i+2], 16)))
        md5Digest += bytes([int(digest[i : i+2], 16)])

    if (len(md5Digest) != 16):
        raise Exception("md5 len not 16: %d" % len(md5Digest))
    
    return md5Digest

init_offset = 512
offset = init_offset
weights = []
header = {}
for file_name in weight_files:
    file_path = os.path.join(folder_name, file_name)
    print("concatenate weight %s" % file_name)
    with open(file_path, 'rb') as f:
        wt = f.read()
        size = len(wt)

        param = []
        param.append(offset)
        param.append(size)
        header[file_name] = param

        print("offset=%d size=%d" %(offset, size))

        size, wt = align(wt, 4)        # offset for next weight, address align with 4
        offset += size
        weights.append(wt)


# find a propriate offset for the first weight, multiply of 512
while 4 + len(bytes(str(header), 'utf-8')) > init_offset :
    init_offset += 512
    for key in header.keys():
        # increase each weight offset
        header[key][0] += 512

magic_bytes = b'WEGT'
header_bytes = bytes(json.dumps(header), 'utf-8')
header_len_bytes = len(header_bytes).to_bytes(4, byteorder='little', signed=False)

header_padding_len, header_padding_bytes = align(header_bytes, init_offset - 4 - 16 - 4)       # magic/md5/header size/header occupies the first init_offset bytes

header_weight_bin = header_padding_bytes + b''.join(weights)
md5_cksum = md5_checksum(header_len_bytes + header_weight_bin)

whole_bin = magic_bytes + md5_cksum + header_len_bytes + header_weight_bin

with open(output_file, "wb") as f:
    f.write(whole_bin)
