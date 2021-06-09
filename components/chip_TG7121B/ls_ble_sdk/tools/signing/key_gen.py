from ecdsa import SigningKey, NIST256p
import sys
import shutil
import hashlib
import subprocess
import os


def save_to_file(file_name, contents):
    fh = open(file_name, 'wb')
    fh.write(contents)
    fh.close()

sk = SigningKey.generate(curve=NIST256p)
sk_pem = sk.to_pem()
save_to_file('signing_key.pem',sk_pem)
vk = sk.verifying_key
vk_pem = vk.to_pem()
save_to_file('verifying_key.pem',vk_pem)
vk_string = vk.to_string()
save_to_file('verifying_key.bin',vk_string)
os.popen('python '+os.path.split(os.path.realpath(__file__))[0]+ '/bin2array/bin2array.py -O verifying_key.txt verifying_key.bin')